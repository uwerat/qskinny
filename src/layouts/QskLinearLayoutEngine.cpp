/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLinearLayoutEngine.h"
#include "QskLayoutHint.h"
#include "QskLayoutChain.h"
#include "QskSizePolicy.h"
#include "QskQuick.h"

#include <qvector.h>

namespace
{
    class Element
    {
      public:
        Element( QQuickItem* item );
        Element( qreal spacing );

        Element& operator=( const Element& );

        qreal spacing() const;
        QQuickItem* item() const;

        Qt::Alignment alignment() const;
        void setAlignment( Qt::Alignment );

        bool retainSizeWhenHidden() const;
        void setRetainSizeWhenHidden( bool );

        int stretch() const;
        void setStretch( int );

        bool isIgnored() const;

        QskLayoutChain::CellData cell(
            Qt::Orientation, bool isLayoutOrientation ) const;

      private:

        union
        {
            QQuickItem* m_item;
            qreal m_spacing;
        };

        int m_stretch = -1;
        bool m_isSpacer;
    };
}

Element::Element( QQuickItem* item )
    : m_item( item )
    , m_isSpacer( false )
{
}

Element::Element( qreal spacing )
    : m_spacing( spacing )
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

    m_stretch = other.m_stretch;

    return *this;
}

inline qreal Element::spacing() const
{
    return m_isSpacer ? m_spacing : -1.0;
}

inline QQuickItem* Element::item() const
{
    return m_isSpacer ? nullptr : m_item;
}

inline int Element::stretch() const
{
    return m_stretch;
}

inline void Element::setStretch( int stretch )
{
    m_stretch = stretch;
}

bool Element::isIgnored() const
{
    return !( m_isSpacer || qskIsVisibleToLayout( m_item ) );
}

QskLayoutChain::CellData Element::cell(
    Qt::Orientation orientation, bool isLayoutOrientation ) const
{
    QskLayoutChain::CellData cell;
    cell.canGrow = true;
    cell.isValid = true;

    if ( !m_isSpacer )
    {
        const auto policy = qskSizePolicy( m_item ).policy( orientation );

        if ( isLayoutOrientation )
        {
            if ( m_stretch < 0 )
                cell.stretch = ( policy & QskSizePolicy::ExpandFlag ) ? 1 : 0;
            else
                cell.stretch = m_stretch;
        }

        cell.canGrow = policy & QskSizePolicy::GrowFlag;
    }
    else
    {
        if ( isLayoutOrientation )
        {
            cell.hint.setMinimum( m_spacing );
            cell.hint.setPreferred( m_spacing );

            if ( m_stretch <= 0 )
                cell.hint.setMaximum( m_spacing );

            cell.stretch = qMax( m_stretch, 0 );
        }
    }

    return cell;
}

class QskLinearLayoutEngine::PrivateData
{
  public:

    PrivateData( Qt::Orientation orientation, uint dimension )
        : dimension( dimension )
        , sumIgnored( -1 )
        , orientation( orientation )
    {
    }

    inline Element* elementAt( int index ) const
    {
        const int count = this->elements.size();
        if ( ( index < 0 ) || ( index >= count ) )
            return nullptr;

        return const_cast< Element* >( &this->elements[index] );
    }

    std::vector< Element > elements;

    uint dimension;

    mutable int sumIgnored : 30;
    unsigned int orientation : 2;
};

QskLinearLayoutEngine::QskLinearLayoutEngine(
        Qt::Orientation orientation, uint dimension )
    : m_data( new PrivateData( orientation, dimension ) )
{
}

QskLinearLayoutEngine::~QskLinearLayoutEngine()
{
}

bool QskLinearLayoutEngine::setOrientation( Qt::Orientation orientation )
{
    if ( m_data->orientation != orientation )
    {
        m_data->orientation = orientation;
        invalidate( LayoutCache );

        return true;
    }

    return false;
}

Qt::Orientation QskLinearLayoutEngine::orientation() const
{
    return static_cast< Qt::Orientation >( m_data->orientation );
}

bool QskLinearLayoutEngine::setDimension( uint dimension )
{
    if ( dimension < 1 )
        dimension = 1;

    if ( m_data->dimension != dimension )
    {
        m_data->dimension = dimension;
        invalidate( LayoutCache );

        return true;
    }

    return false;
}

uint QskLinearLayoutEngine::dimension() const
{
    return m_data->dimension;
}

int QskLinearLayoutEngine::count() const
{
    return m_data->elements.size();
}

bool QskLinearLayoutEngine::setStretchFactorAt( int index, int stretchFactor )
{
    if ( auto element = m_data->elementAt( index ) )
    {
        if ( stretchFactor < 0 )
            stretchFactor = -1;

        if ( element->stretch() != stretchFactor )
        {
            element->setStretch( stretchFactor );
            invalidate( LayoutCache );

            return true;
        }
    }

    return false;
}

int QskLinearLayoutEngine::stretchFactorAt( int index ) const
{
    if ( const auto element = m_data->elementAt( index ) )
        return element->stretch();

    return -1;
}

int QskLinearLayoutEngine::insertItem( QQuickItem* item, int index )
{
    auto& elements = m_data->elements;

    if ( index < 0 || index > count() )
    {
        index = elements.size();
        elements.emplace_back( item );
    }
    else
    {
        elements.emplace( elements.begin() + index, item );
    }

    invalidate();
    return index;
}

int QskLinearLayoutEngine::insertSpacerAt( int index, qreal spacing )
{
    spacing = qMax( spacing, static_cast< qreal >( 0.0 ) );

    auto& elements = m_data->elements;

    if ( index < 0 || index > count() )
    {
        index = elements.size();
        elements.emplace_back( spacing );
    }
    else
    {
        elements.emplace( elements.begin() + index, spacing );
    }

    invalidate( LayoutCache );
    return index;
}

bool QskLinearLayoutEngine::removeAt( int index )
{
    auto element = m_data->elementAt( index );
    if ( element == nullptr )
        return false;

    if ( element->isIgnored() )
        m_data->sumIgnored--;

    const auto itemType = qskSizePolicy( element->item() ).constraintType();

    int invalidationMode = LayoutCache;

    if ( itemType > QskSizePolicy::Unconstrained )
        invalidationMode |= ElementCache;

    m_data->elements.erase( m_data->elements.begin() + index );
    invalidate( invalidationMode );

    return true;
}

bool QskLinearLayoutEngine::clear()
{
    if ( count() <= 0 )
        return false;

    m_data->elements.clear();
    invalidate();

    return true;
}

QQuickItem* QskLinearLayoutEngine::itemAt( int index ) const
{
    if ( const auto element = m_data->elementAt( index ) )
        return element->item();

    return nullptr;
}

qreal QskLinearLayoutEngine::spacerAt( int index ) const
{
    if ( const auto element = m_data->elementAt( index ) )
        return element->spacing();

    return -1.0;
}

void QskLinearLayoutEngine::layoutItems()
{
    uint row = 0;
    uint col = 0;

    for ( const auto& element : m_data->elements )
    {
        if ( element.isIgnored() )
            continue;

        if ( auto item = element.item() )
        {
            if ( requiresAdjustment( item ) )
            {
                const QRect grid( col, row, 1, 1 );
                layoutItem( item, grid );
            }
        }

        if ( m_data->orientation == Qt::Horizontal )
        {
            if ( ++col == m_data->dimension )
            {
                col = 0;
                row++;
            }
        }
        else
        {
            if ( ++row == m_data->dimension )
            {
                row = 0;
                col++;
            }
        }
    }
}

int QskLinearLayoutEngine::effectiveCount( Qt::Orientation orientation ) const
{
    const uint cellCount = effectiveCount();

    if ( orientation == m_data->orientation )
    {
        return qMin( cellCount, m_data->dimension );
    }
    else
    {
        int count = cellCount / m_data->dimension;
        if ( cellCount % m_data->dimension )
            count++;

        return count;
    }
}

int QskLinearLayoutEngine::effectiveCount() const
{
    if ( m_data->sumIgnored < 0 )
    {
        m_data->sumIgnored = 0;

        for ( const auto& element : m_data->elements )
        {
            if ( element.isIgnored() )
                m_data->sumIgnored++;
        }
    }

    return m_data->elements.size() - m_data->sumIgnored;
}

void QskLinearLayoutEngine::invalidateElementCache()
{
    m_data->sumIgnored = -1;
}

void QskLinearLayoutEngine::setupChain( Qt::Orientation orientation,
    const QskLayoutChain::Segments& constraints, QskLayoutChain& chain ) const
{
    uint index1 = 0;
    uint index2 = 0;

    const bool isLayoutOrientation = ( orientation == m_data->orientation );

    qreal constraint = -1.0;

    for ( const auto& element : m_data->elements )
    {
        if ( element.isIgnored() )
            continue;

        if ( !constraints.isEmpty() )
            constraint = constraints[index1].length;

        auto cell = element.cell( orientation, isLayoutOrientation );

        if ( element.item() )
            cell.hint = layoutHint( element.item(), orientation, constraint );

        chain.expandCell( index2, cell );

        if ( isLayoutOrientation )
        {
            if ( ++index2 == m_data->dimension )
            {
                index2 = 0;
                index1++;
            }
        }
        else
        {
            if ( ++index1 == m_data->dimension )
            {
                index1 = 0;
                index2++;
            }
        }
    }
}
