/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGridLayoutEngine.h"
#include "QskLayoutHint.h"
#include "QskLayoutChain.h"
#include "QskSizePolicy.h"
#include "QskQuick.h"

#include <qvector.h>

#include <vector>
#include <functional>

static inline qreal qskSegmentLength(
    const QskLayoutChain::Segments& s, int start, int end )
{
    return s[ end ].start - s[ start ].start + s[ end ].length;
}

namespace
{
    class Settings
    {
      public:
        class Setting
        {
          public:
            inline bool isDefault() const
            {
                return m_stretch < 0 && m_hint.isDefault();
            }

            bool setStretch( int stretch )
            {
                if ( stretch != m_stretch )
                {
                    m_stretch = stretch;
                    return true;
                }
                return false;
            }

            bool setHint( Qt::SizeHint which, qreal size )
            {
                if ( size != m_hint.size( which ) )
                {
                    m_hint.setSize( which, size );
                    return true;
                }
                return false;
            }

            QskLayoutChain::CellData cell() const
            {
                QskLayoutChain::CellData cell;
                cell.hint = m_hint.normalized();
                cell.stretch = m_stretch;
                cell.canGrow = m_stretch != 0;
                cell.isValid = true;

                return cell;
            }

            inline int stretch() const { return m_stretch; }
            inline QskLayoutHint hint() const { return m_hint; }

            int position = -1;

          private:
            int m_stretch = -1;
            QskLayoutHint m_hint;
        };

        int maxPosition() const
        {
            if ( m_settings.empty() )
                return -1;

            return m_settings.back().position;
        }

        void clear()
        {
            m_settings.clear();
        }

        bool setStretchAt( int index, int stretch )
        {
            auto setStretch = [stretch]( Setting& s )
                { return s.setStretch( stretch ); };

            return setValueAt( index, setStretch );
        }

        bool setHintAt( int index, Qt::SizeHint which, qreal size )
        {
            auto setHint = [which, size]( Setting& s )
                { return s.setHint( which, size ); };

            return setValueAt( index, setHint );
        }

        Setting settingAt( int index ) const
        {
            auto it = lowerBound( index );
            if ( it != m_settings.end() )
                return *it;

            return Setting();
        }

        const std::vector< Setting >& settings() const { return m_settings; }

      private:
        inline bool setValueAt( int pos,
            const std::function< bool( Setting& ) > modify )
        {
            if ( pos < 0 )
                return false;

            bool isModified;

            auto it = lowerBound( pos );

            if ( it != m_settings.end() && it->position == pos )
            {
                isModified = modify( *it );

                if ( isModified && it->isDefault() )
                    m_settings.erase( it );
            }
            else
            {
                Setting setting;
                isModified = modify( setting );

                if ( isModified )
                {
                    setting.position = pos;
                    m_settings.insert( it, setting );
                }
            }

            return isModified;
        }

        inline std::vector< Setting >::iterator lowerBound( int index ) const
        {
            auto cmp = []( const Setting& setting, const int& pos )
                { return setting.position < pos; };

            auto& settings = const_cast< std::vector< Setting >& >( m_settings );
            return std::lower_bound( settings.begin(), settings.end(), index, cmp );
        }

        std::vector< Setting > m_settings; // a flat map
    };
}

namespace
{
    class Element
    {
      public:
        Element( QQuickItem*, const QRect& );
        Element( const QSizeF& spacing, const QRect& );

        Element& operator=( const Element& );

        QSizeF spacing() const;
        QQuickItem* item() const;

        QRect grid() const;
        void setGrid( const QRect& );

        QRect minimumGrid() const;

        bool isIgnored() const;
        QskLayoutChain::CellData cell( Qt::Orientation ) const;

        void transpose();

      private:

        union
        {
            QQuickItem* m_item;
            QSizeF m_spacing;
        };

        QRect m_grid;
        bool m_isSpacer;
    };
}

Element::Element( QQuickItem* item, const QRect& grid )
    : m_item( item )
    , m_grid( grid )
    , m_isSpacer( false )
{
}

Element::Element( const QSizeF& spacing, const QRect& grid )
    : m_spacing( spacing )
    , m_grid( grid )
    , m_isSpacer( true )
{
}

Element& Element::operator=( const Element& other )
{
    m_isSpacer = other.m_isSpacer;

    if ( other.m_isSpacer )
        m_spacing = other.m_spacing;
    else
        m_item = other.m_item;

    m_grid = other.m_grid;

    return *this;
}

inline QSizeF Element::spacing() const
{
    return m_isSpacer ? m_spacing : QSizeF();
}

inline QQuickItem* Element::item() const
{
    return m_isSpacer ? nullptr : m_item;
}

QRect Element::grid() const
{
    return m_grid;
}

void Element::setGrid( const QRect& grid )
{
    m_grid = grid;
}

QRect Element::minimumGrid() const
{
    return QRect( m_grid.left(), m_grid.top(),
        qMax( m_grid.width(), 1 ), qMax( m_grid.height(), 1 ) );
}

bool Element::isIgnored() const
{
    return !( m_isSpacer || qskIsVisibleToLayout( m_item ) );
}

QskLayoutChain::CellData Element::cell( Qt::Orientation orientation ) const
{
    QskLayoutChain::CellData cell;
    cell.isValid = true;

    if ( m_isSpacer )
    {
        const qreal value = ( orientation == Qt::Horizontal )
            ? m_spacing.width() : m_spacing.height();

        cell.hint.setMinimum( value );
        cell.hint.setPreferred( value );
        cell.hint.setMaximum( value );
    }
    else
    {
        const auto policy = qskSizePolicy( m_item ).policy( orientation );

        cell.canGrow = policy & QskSizePolicy::GrowFlag;

        if ( policy & QskSizePolicy::ExpandFlag )
            cell.stretch = 1;
    }

    return cell;
}

void Element::transpose()
{
    m_grid.setRect( m_grid.top(), m_grid.left(),
        m_grid.height(), m_grid.width() );
}

class QskGridLayoutEngine::PrivateData
{
  public:
    inline Element* elementAt( int index ) const
    {
        const int count = this->elements.size();
        if ( index < 0 || index >= count )
            return nullptr;

        return const_cast< Element* >( &this->elements[index] );
    }

    int insertElement( QQuickItem* item, QSizeF spacing, QRect grid )
    {
        // -1 means unlimited, while 0 does not make any sense
        if ( grid.width() == 0 )
            grid.setWidth( 1 );

        if ( grid.height() == 0 )
            grid.setHeight( 1 );

        if ( item )
        {
            elements.push_back( Element( item, grid ) );
        }
        else
        {
            if ( spacing.width() < 0.0 )
                spacing.setWidth( 0.0 );

            if ( spacing.height() < 0.0 )
                spacing.setHeight( 0.0 );

            elements.push_back( Element( spacing, grid ) );
        }

        grid = effectiveGrid( elements.back() );

        rowCount = qMax( rowCount, grid.bottom() + 1 );
        columnCount = qMax( columnCount, grid.right() + 1 );

        return this->elements.size() - 1;
    }

    QRect effectiveGrid( const Element& element ) const
    {
        QRect r = element.grid();

        if ( r.width() <= 0 )
            r.setRight( qMax( this->columnCount - 1, r.left() ) );

        if ( r.height() <= 0 )
            r.setBottom( qMax( this->rowCount - 1, r.top() ) );

        return r;
    }

    Settings& settings( Qt::Orientation orientation ) const
    {
        auto that = const_cast< PrivateData* >( this );
        return ( orientation == Qt::Horizontal )
            ? that->columnSettings : that->rowSettings;
    }

    std::vector< Element > elements;

    Settings rowSettings;
    Settings columnSettings;

    int rowCount = 0;
    int columnCount = 0;
};

QskGridLayoutEngine::QskGridLayoutEngine()
    : m_data( new PrivateData() )
{
}

QskGridLayoutEngine::~QskGridLayoutEngine()
{
}

int QskGridLayoutEngine::count() const
{
    return m_data->elements.size();
}

bool QskGridLayoutEngine::setStretchFactor(
    int pos, int stretch, Qt::Orientation orientation )
{
    if ( pos < 0 )
        return false;

    if ( stretch < 0 )
        stretch = -1;

    if ( !m_data->settings( orientation ).setStretchAt( pos, stretch ) )
        return false;

    if ( orientation == Qt::Horizontal )
    {
        if ( pos >= m_data->columnCount )
            m_data->columnCount = pos + 1;
    }
    else
    {
        if ( pos >= m_data->rowCount )
            m_data->rowCount = pos + 1;
    }

    invalidate();
    return true;
}

int QskGridLayoutEngine::stretchFactor(
    int pos, Qt::Orientation orientation ) const
{
    const auto setting = m_data->settings( orientation ).settingAt( pos );
    return ( setting.position == pos ) ? setting.stretch() : 0;
}

bool QskGridLayoutEngine::setRowSizeHint(
    int row, Qt::SizeHint which, qreal height )
{
    if ( !m_data->rowSettings.setHintAt( row, which, height ) )
        return false;

    if ( row >= m_data->rowCount )
        m_data->rowCount = row + 1;

    invalidate();
    return true;
}

qreal QskGridLayoutEngine::rowSizeHint( int row, Qt::SizeHint which ) const
{
    const auto& settings = m_data->rowSettings;
    return settings.settingAt( row ).hint().size( which );
}

bool QskGridLayoutEngine::setColumnSizeHint(
    int column, Qt::SizeHint which, qreal width )
{
    if ( !m_data->columnSettings.setHintAt( column, which, width ) )
        return false;

    if ( column >= m_data->columnCount )
        m_data->columnCount = column + 1;

    invalidate();
    return true;
}

qreal QskGridLayoutEngine::columnSizeHint( int column, Qt::SizeHint which ) const
{
    const auto& settings = m_data->columnSettings;
    return settings.settingAt( column ).hint().size( which );
}

int QskGridLayoutEngine::insertItem( QQuickItem* item, const QRect& grid )
{
    invalidate();
    return m_data->insertElement( item, QSizeF(), grid );
}

int QskGridLayoutEngine::insertSpacer( const QSizeF& spacing, const QRect& grid )
{
    return m_data->insertElement( nullptr, spacing, grid );
}

bool QskGridLayoutEngine::removeAt( int index )
{
    const auto element = m_data->elementAt( index );
    if ( element == nullptr )
        return false;

    const auto grid = element->minimumGrid();

    auto& elements = m_data->elements;
    elements.erase( elements.begin() + index );

    // doing a lazy recalculation instead ??

    if ( grid.bottom() >= m_data->rowCount
        || grid.right() >= m_data->columnCount )
    {
        int maxRow = m_data->rowSettings.maxPosition();
        int maxColumn = m_data->columnSettings.maxPosition();

        for ( const auto& element : elements )
        {
            const auto grid = element.minimumGrid();

            maxRow = qMax( maxRow, grid.bottom() );
            maxColumn = qMax( maxColumn, grid.right() );
        }

        m_data->rowCount = maxRow + 1;
        m_data->columnCount = maxColumn + 1;
    }

    invalidate();
    return true;
}

bool QskGridLayoutEngine::clear()
{
    m_data->elements.clear();
    m_data->rowSettings.clear();
    m_data->columnSettings.clear();

    invalidate();
    return true;
}

int QskGridLayoutEngine::indexAt( int row, int column ) const
{
    if ( row < m_data->rowCount && column < m_data->columnCount )
    {
        for ( uint i = 0; i < m_data->elements.size(); i++ )
        {
            const auto grid = m_data->effectiveGrid( m_data->elements[i] );
            if ( grid.contains( column, row ) )
                return i;
        }
    }

    return -1;
}

QQuickItem* QskGridLayoutEngine::itemAt( int index ) const
{
    if ( const auto element = m_data->elementAt( index ) )
        return element->item();

    return nullptr;
}

QSizeF QskGridLayoutEngine::spacerAt( int index ) const
{
    if ( const auto element = m_data->elementAt( index ) )
        return element->spacing();

    return QSizeF();
}

QQuickItem* QskGridLayoutEngine::itemAt( int row, int column ) const
{
    return itemAt( indexAt( row, column ) );
}

bool QskGridLayoutEngine::setGridAt( int index, const QRect& grid )
{
    if ( auto element = m_data->elementAt( index ) )
    {
        if ( element->grid() != grid )
        {
            element->setGrid( grid );
            invalidate();

            return true;
        }
    }

    return false;
}

QRect QskGridLayoutEngine::gridAt( int index ) const
{
    if ( auto element = m_data->elementAt( index ) )
        return element->grid();

    return QRect();
}

QRect QskGridLayoutEngine::effectiveGridAt( int index ) const
{
    if ( auto element = m_data->elementAt( index ) )
        return m_data->effectiveGrid( *element );

    return QRect();
}

void QskGridLayoutEngine::invalidateElementCache()
{
}

void QskGridLayoutEngine::layoutItems()
{
    for ( const auto& element : m_data->elements )
    {
        auto item = element.item();

        if ( requiresAdjustment( item ) )
        {
            const auto grid = m_data->effectiveGrid( element );
            layoutItem( item, grid );
        }
    }
}

void QskGridLayoutEngine::transpose()
{
    for ( auto& element : m_data->elements )
        element.transpose();

    qSwap( m_data->columnSettings, m_data->rowSettings );
    qSwap( m_data->columnCount, m_data->rowCount );

    invalidate();
}

int QskGridLayoutEngine::effectiveCount(
    Qt::Orientation orientation ) const
{
    return ( orientation == Qt::Horizontal )
        ? m_data->columnCount : m_data->rowCount;
}

void QskGridLayoutEngine::setupChain( Qt::Orientation orientation,
    const QskLayoutChain::Segments& constraints, QskLayoutChain& chain ) const
{
    /*
        We collect all information from the simple elements first
        before adding those that occupy more than one cell
     */
    QVarLengthArray< const Element* > postponed;
    postponed.reserve( m_data->elements.size() );

    for ( const auto& element : m_data->elements )
    {
        if ( element.isIgnored() )
            continue;

        auto grid = m_data->effectiveGrid( element );
        if ( orientation == Qt::Horizontal )
            grid.setRect( grid.y(), grid.x(), grid.height(), grid.width() );

        if ( grid.height() == 1 )
        {
            qreal constraint = -1.0;
            if ( !constraints.isEmpty() )
                constraint = qskSegmentLength( constraints, grid.left(), grid.right() );

            auto cell = element.cell( orientation );
            if ( element.item() )
                cell.hint = layoutHint( element.item(), orientation, constraint );

            chain.expandCell( grid.top(), cell );
        }
        else
        {
            postponed += &element;
        }
    }

    const auto& settings = m_data->settings( orientation );

    for ( const auto& setting : settings.settings() )
        chain.shrinkCell( setting.position, setting.cell() );

    for ( const auto element : postponed )
    {
        auto grid = m_data->effectiveGrid( *element );
        if ( orientation == Qt::Horizontal )
            grid.setRect( grid.y(), grid.x(), grid.height(), grid.width() );

        qreal constraint = -1.0;
        if ( !constraints.isEmpty() )
            constraint = qskSegmentLength( constraints, grid.left(), grid.right() );

        auto cell = element->cell( orientation );
        cell.hint = layoutHint( element->item(), orientation, constraint );

        chain.expandCells( grid.top(), grid.height(), cell );
    }
}
