/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLinearLayoutEngine.h"

#include "QskLayoutHint.h"
#include "QskLayoutConstraint.h"
#include "QskLayoutChain.h"
#include "QskSizePolicy.h"
#include "QskQuick.h"

#include <qvector.h>

static constexpr qreal qskDefaultSpacing()
{
    // should be a skin hint
    return 5.0;
}

namespace
{
    class CellGeometries
    {
      public:
        void invalidate()
        {
            boundingSize = QSizeF();
            rows.clear();
            columns.clear();
        }

        QRectF geometryAt( int row, int col ) const
        {
            return QRectF(
                columns[ col ].start, rows[ row ].start,
                columns[ col ].length, rows[ row ].length );
        }

        QSizeF boundingSize;

        QVector< QskLayoutChain::Range > rows;
        QVector< QskLayoutChain::Range > columns;
    };

    class EntryData
    {
      public:
        EntryData( QQuickItem* item );
        EntryData( qreal spacing );

        EntryData& operator=( const EntryData& );

        bool isIgnored() const;

        qreal spacer() const { return m_isSpacer ? m_spacer : -1.0; }
        QQuickItem* item() const { return m_isSpacer ? nullptr : m_item; }

        inline Qt::Alignment alignment() const
            { return static_cast< Qt::Alignment >( m_alignment ); }
        inline void setAlignment( Qt::Alignment alignment ) { m_alignment = alignment; }

        inline int stretch() const { return m_stretch; }
        inline void setStretch( int stretch ) { m_stretch = stretch; }

        bool retainSizeWhenHidden() const { return m_retainSizeWhenHidden; }
        void setRetainSizeWhenHidden( bool on ) { m_retainSizeWhenHidden = on; }

      private:

        union
        {
            QQuickItem* m_item;
            qreal m_spacer;
        };

        int m_stretch;

        unsigned int m_alignment : 8;
        bool m_isSpacer : 1;
        bool m_retainSizeWhenHidden : 1;
    };

    class EntryTable
    {
      public:
        EntryTable( Qt::Orientation, uint dimension );

        bool setOrientation( Qt::Orientation orientation );
        Qt::Orientation orientation() const;

        bool setDimension( uint dimension );
        uint dimension() const;

        bool setDefaultAlignment( Qt::Alignment );
        Qt::Alignment defaultAlignment() const;

        Qt::Alignment effectiveAlignmentAt( int index ) const;

        void insertItem( int index, QQuickItem* );
        void insertSpacer( int index, qreal spacing );

        QQuickItem* itemAt( int index ) const;
        int spacerAt( int index ) const;

        bool removeAt( int index );

        bool isIgnoredAt( int index ) const;

        bool setAlignmentAt( int index, Qt::Alignment );
        Qt::Alignment alignmentAt( int index ) const;

        bool setStretchFactorAt( int index, int stretchFactor );
        int stretchFactorAt( int index ) const;

        bool setRetainSizeWhenHiddenAt( int index, bool on );
        bool retainSizeWhenHiddenAt( int index ) const;

        inline int count() const { return m_entries.size(); }

        int effectiveCount() const;
        int effectiveCount( Qt::Orientation orientation ) const;

        void updateLayoutChain( Qt::Orientation,
            const QVector< QskLayoutChain::Range >& constraints,
            QskLayoutChain& ) const;

        QskLayoutConstraint::Type constraintType() const;

        void invalidate();

      private:
        QskLayoutChain::Cell cell( const EntryData&,
            Qt::Orientation, qreal constraint ) const;

        inline EntryData* entryAt( int index ) const
        {
            if ( index < 0 || index >= count() )
            {
                // qWarning, TODO ...
                return nullptr;
            }

            return const_cast< EntryData* >( &m_entries[index] );
        }

        std::vector< EntryData > m_entries;

        uint m_dimension;
        mutable int m_sumIgnored : 19;
        mutable int m_constraintType : 3;

        unsigned int m_defaultAlignment : 8;
        unsigned int m_orientation : 2;
    };

}

EntryData::EntryData( QQuickItem* item )
    : m_item( item )
    , m_stretch( -1 )
    , m_alignment( 0 )
    , m_isSpacer( false )
    , m_retainSizeWhenHidden( false )
{
}

EntryData::EntryData( qreal spacing )
    : m_spacer( spacing )
    , m_stretch( -1 )
    , m_alignment( 0 )
    , m_isSpacer( true )
    , m_retainSizeWhenHidden( false )
{
}

EntryData& EntryData::operator=( const EntryData& other )
{
    m_isSpacer = other.m_isSpacer;

    if ( other.m_isSpacer )
        m_spacer = other.m_spacer;
    else
        m_item = other.m_item;

    m_stretch = other.m_stretch;
    m_alignment = other.m_alignment;
    m_retainSizeWhenHidden = other.m_retainSizeWhenHidden;

    return *this;
}

bool EntryData::isIgnored() const
{
    if ( !m_isSpacer && !m_retainSizeWhenHidden )
        return !qskIsVisibleToParent( m_item );

    return false;
}

EntryTable::EntryTable( Qt::Orientation orientation, uint dimension )
    : m_dimension( dimension )
    , m_sumIgnored( -1 )
    , m_constraintType( -1 )
    , m_defaultAlignment( Qt::AlignLeft | Qt::AlignVCenter )
    , m_orientation( orientation )
{
}

bool EntryTable::setOrientation( Qt::Orientation orientation )
{
    if ( m_orientation != orientation )
    {
        m_orientation = orientation;
        return true;
    }

    return false;
}

Qt::Orientation EntryTable::orientation() const
{
    return static_cast< Qt::Orientation >( m_orientation );
}

bool EntryTable::setDimension( uint dimension )
{
    if ( m_dimension != dimension )
    {
        m_dimension = dimension;
        return true;
    }

    return false;
}

uint EntryTable::dimension() const
{
    return m_dimension;
}

bool EntryTable::setDefaultAlignment( Qt::Alignment alignment )
{
    if ( defaultAlignment() != alignment )
    {
        m_defaultAlignment = alignment;
        return true;
    }

    return false;
}

Qt::Alignment EntryTable::defaultAlignment() const
{
    return static_cast< Qt::Alignment >( m_defaultAlignment );
}

Qt::Alignment EntryTable::effectiveAlignmentAt( int index ) const
{
    Qt::Alignment alignment;

    if ( const auto entry = entryAt( index ) )
        alignment = entry->alignment();

    if ( !( alignment & Qt::AlignVertical_Mask ) )
        alignment |= ( defaultAlignment() & Qt::AlignVertical_Mask );

    if ( !( alignment & Qt::AlignHorizontal_Mask ) )
        alignment |= ( defaultAlignment() & Qt::AlignHorizontal_Mask );

    return alignment;
}

bool EntryTable::isIgnoredAt( int index ) const
{
    if ( index >= 0 && index < count() )
        return m_entries[index].isIgnored();

    return false;
}

void EntryTable::insertItem( int index, QQuickItem* item )
{
    if ( index < 0 || index > count() )
        m_entries.emplace_back( item );
    else
        m_entries.emplace( m_entries.begin() + index, item );

    invalidate();
}

void EntryTable::insertSpacer( int index, qreal spacing )
{
    spacing = qMax( spacing, static_cast< qreal >( 0.0 ) );

    if ( index < 0 || index > count() )
        m_entries.emplace_back( spacing );
    else
        m_entries.emplace( m_entries.begin() + index, spacing );
}

bool EntryTable::removeAt( int index )
{
    if ( index >= 0 && index < count() )
    {
        m_entries.erase( m_entries.begin() + index );
        invalidate();

        return true;
    }

    return false;
}

QQuickItem* EntryTable::itemAt( int index ) const
{
    if ( const auto entry = entryAt( index ) )
        return entry->item();

    return nullptr;
}

int EntryTable::spacerAt( int index ) const
{
    if ( const auto entry = entryAt( index ) )
        return entry->spacer();

    return -1;
}

bool EntryTable::setAlignmentAt( int index, Qt::Alignment alignment )
{
    if ( auto entry = entryAt( index ) )
    {
        if ( alignment != entry->alignment() )
            entry->setAlignment( alignment );

        return true;
    }

    return false;
}

Qt::Alignment EntryTable::alignmentAt( int index ) const
{
    if ( const auto entry = entryAt( index ) )
        return entry->alignment();

    return Qt::Alignment();
}

bool EntryTable::setStretchFactorAt( int index, int stretchFactor )
{
    if ( auto entry = entryAt( index ) )
    {
        if ( stretchFactor < 0 )
            stretchFactor = -1;

        if ( entry->stretch() != stretchFactor )
        {
            entry->setStretch( stretchFactor );
            return true;
        }
    }

    return false;
}

int EntryTable::stretchFactorAt( int index ) const
{
    if ( const auto entry = entryAt( index ) )
        return entry->stretch();

    return -1;
}

bool EntryTable::setRetainSizeWhenHiddenAt( int index, bool on )
{
    if ( auto entry = entryAt( index ) )
    {
        if ( on != entry->retainSizeWhenHidden() )
        {
            const bool isIgnored = entry->isIgnored();

            entry->setRetainSizeWhenHidden( on );

            if ( isIgnored != entry->isIgnored() )
            {
                if ( m_sumIgnored >= 0 )
                    m_sumIgnored += on ? 1 : -1;

                return true;
            }
        }
    }

    return false;
}

bool EntryTable::retainSizeWhenHiddenAt( int index ) const
{
    if ( const auto entry = entryAt( index ) )
        return entry->retainSizeWhenHidden();

    return false;
}

void EntryTable::invalidate()
{
    m_sumIgnored = -1;
    m_constraintType = -1;
}

int EntryTable::effectiveCount() const
{
    if ( m_sumIgnored < 0 )
    {
        m_sumIgnored = 0;

        for ( const auto& entry : m_entries )
        {
            if ( entry.isIgnored() )
                m_sumIgnored++;
        }
    }

    return m_entries.size() - m_sumIgnored;
}

int EntryTable::effectiveCount( Qt::Orientation orientation ) const
{
    const uint cellCount = effectiveCount();

    if ( orientation == m_orientation )
    {
        return qMin( cellCount, m_dimension );
    }
    else
    {
        int count = cellCount / m_dimension;
        if ( cellCount % m_dimension )
            count++;

        return count;
    }
}

QskLayoutConstraint::Type EntryTable::constraintType() const
{
    if ( m_constraintType < 0 )
    {
        m_constraintType = QskLayoutConstraint::Unconstrained;

        for ( const auto& entry : m_entries )
        {
            const auto entryType = QskLayoutConstraint::constraintType( entry.item() );

            if ( entryType != QskLayoutConstraint::Unconstrained )
            {
                if ( m_constraintType == QskLayoutConstraint::Unconstrained )
                {
                    m_constraintType = entryType;
                }
                else if ( m_constraintType != entryType )
                {
                    qWarning( "QskLinearLayoutEngine: conflicting constraints");
                    m_constraintType = QskLayoutConstraint::Unconstrained;
                }
            }
        }
    }

    return static_cast< QskLayoutConstraint::Type >( m_constraintType );
}

QskLayoutChain::Cell EntryTable::cell( const EntryData& entry,
    Qt::Orientation orientation, qreal constraint ) const
{
    QskLayoutChain::Cell cell;
    cell.canGrow = true;

    if ( const auto item = entry.item() )
    {
        cell.hint = QskLayoutConstraint::layoutHint( item, orientation, constraint );

        const auto policy = QskLayoutConstraint::sizePolicy( item ).policy( orientation );

        if ( orientation == m_orientation )
        {
            if ( entry.stretch() < 0 )
                cell.stretch = ( policy & QskSizePolicy::ExpandFlag ) ? 1 : 0;
            else
                cell.stretch = entry.stretch();
        }

        cell.canGrow = policy & QskSizePolicy::GrowFlag;
    }
    else
    {
        if ( orientation == m_orientation )
        {
            cell.hint.setMinimum( entry.spacer() );
            cell.hint.setPreferred( entry.spacer() );

            if ( entry.stretch() <= 0 )
                cell.hint.setMaximum( entry.spacer() );

            cell.stretch = qMax( entry.stretch(), 0 );
        }
    }

    return cell;
}

void EntryTable::updateLayoutChain( Qt::Orientation orientation,
    const QVector< QskLayoutChain::Range >& constraints,
    QskLayoutChain& chain ) const
{
    const auto count = effectiveCount( orientation );
    const qreal constraint =
        constraints.isEmpty() ? -1.0 : constraints.last().end();

    if ( ( chain.constraint() == constraint )
        && ( chain.count() == count ) )
    {
        return; // already up to date
    }

    chain.reset( count, constraint );

    uint index1 = 0;
    uint index2 = 0;

    for ( const auto& entry : m_entries )
    {
        if ( entry.isIgnored() )
            continue;

        const qreal cellConstraint =
            constraints.isEmpty() ? -1.0 : constraints[index1].length;

        const auto cell = this->cell( entry, orientation, cellConstraint );
        chain.expandTo( index2, cell );

        if ( m_orientation != orientation )
        {
            if ( ++index1 == m_dimension )
            {
                index1 = 0;
                index2++;
            }
        }
        else
        {
            if ( ++index2 == m_dimension )
            {
                index2 = 0;
                index1++;
            }
        }
    }

    chain.finish();
}

// ---------

static inline void qskUpdateLayoutChain( Qt::Orientation orientation,
    const QVector< QskLayoutChain::Range >& constraints,
    const EntryTable& entryTable, QskLayoutChain& chain )
{
    entryTable.updateLayoutChain( orientation, constraints, chain );
}

static inline void qskUpdateLayoutChain( Qt::Orientation orientation,
    const EntryTable& entryTable, QskLayoutChain& chain )
{
    const QVector< QskLayoutChain::Range > constraints;
    entryTable.updateLayoutChain( orientation, constraints, chain );
}

class QskLinearLayoutEngine::PrivateData
{
  public:

    PrivateData( Qt::Orientation orientation, uint dimension )
        : entryTable( orientation, dimension )
        , blockInvalidate( false )
    {
        rowChain.setSpacing( qskDefaultSpacing() );
        colChain.setSpacing( qskDefaultSpacing() );
    }

    EntryTable entryTable;

    Qt::LayoutDirection visualDirection = Qt::LeftToRight;
    Qt::Edges extraSpacingAt;

    QskLayoutChain colChain;
    QskLayoutChain rowChain;

    CellGeometries geometries;

    /*
        Some weired controls do lazy updates inside of their sizeHint calculation
        that lead to LayoutRequest events. While being in the process of
        updating the tables we can't - and don't need to - handle invalidations
        because of them.
     */
    bool blockInvalidate : 1;
};

QskLinearLayoutEngine::QskLinearLayoutEngine( Qt::Orientation orientation, uint dimension )
    : m_data( new PrivateData( orientation, dimension ) )
{
}

QskLinearLayoutEngine::~QskLinearLayoutEngine()
{
}

bool QskLinearLayoutEngine::setOrientation( Qt::Orientation orientation )
{
    const bool isModified = m_data->entryTable.setOrientation( orientation );
    if ( isModified )
        invalidate( CellCache | LayoutCache );

    return isModified;
}

Qt::Orientation QskLinearLayoutEngine::orientation() const
{
    return m_data->entryTable.orientation();
}

bool QskLinearLayoutEngine::setDimension( uint dimension )
{
    if ( dimension < 1 )
        dimension = 1;

    const bool isModified =
        m_data->entryTable.setDimension( dimension );

    if ( isModified )
        invalidate( CellCache | LayoutCache );

    return isModified;
}

uint QskLinearLayoutEngine::dimension() const
{
    return m_data->entryTable.dimension();
}

int QskLinearLayoutEngine::count() const
{
    return m_data->entryTable.count();
}

int QskLinearLayoutEngine::rowCount() const
{
    return m_data->entryTable.effectiveCount( Qt::Vertical );
}

int QskLinearLayoutEngine::columnCount() const
{
    return m_data->entryTable.effectiveCount( Qt::Horizontal );
}

bool QskLinearLayoutEngine::setRetainSizeWhenHiddenAt( int index, bool on )
{
    const bool isModified =
        m_data->entryTable.setRetainSizeWhenHiddenAt( index, on );

    if ( isModified )
        invalidate( CellCache | LayoutCache );

    return isModified;
}

bool QskLinearLayoutEngine::retainSizeWhenHiddenAt( int index ) const
{
    return m_data->entryTable.retainSizeWhenHiddenAt( index );
}

bool QskLinearLayoutEngine::setStretchFactorAt( int index, int stretchFactor )
{
    const bool isModified =
        m_data->entryTable.setStretchFactorAt( index, stretchFactor );

    if ( isModified )
        invalidate( CellCache | LayoutCache );

    return isModified;
}

int QskLinearLayoutEngine::stretchFactorAt( int index ) const
{
    return m_data->entryTable.stretchFactorAt( index );
}

bool QskLinearLayoutEngine::setAlignmentAt( int index, Qt::Alignment alignment )
{
    return m_data->entryTable.setAlignmentAt( index, alignment );
}

Qt::Alignment QskLinearLayoutEngine::alignmentAt( int index ) const
{
    return m_data->entryTable.alignmentAt( index );
}

bool QskLinearLayoutEngine::setSpacing( qreal spacing, Qt::Orientations orientations )
{
    if ( spacing < 0.0 )
        spacing = 0.0;

    bool isModified = false;

    if ( orientations & Qt::Horizontal )
        isModified |= m_data->colChain.setSpacing( spacing );

    if ( orientations & Qt::Vertical )
        isModified |= m_data->rowChain.setSpacing( spacing );

    if ( isModified )
        invalidate( CellCache | LayoutCache );

    return isModified;
}

qreal QskLinearLayoutEngine::spacing( Qt::Orientation orientation ) const
{
    if ( orientation == Qt::Horizontal )
        return m_data->colChain.spacing();
    else
        return m_data->rowChain.spacing();
}

bool QskLinearLayoutEngine::setExtraSpacingAt( Qt::Edges edges )
{
    if ( edges == m_data->extraSpacingAt )
        return false;

    m_data->extraSpacingAt = edges;

    Qt::Edges colEdges = edges & ~( Qt::TopEdge | Qt::BottomEdge );
    m_data->colChain.setExtraSpacingAt( colEdges );

    /*
        FlowLayoutInfo does not have an orientation, so we always
        set the position for potential fill spaces using Left/Right.
        Maybe introducing another enum might be a good idea. TODO ...
     */

    Qt::Edges rowEdges;
    if ( edges & Qt::TopEdge )
        rowEdges |= Qt::LeftEdge;

    if ( edges & Qt::BottomEdge )
        rowEdges |= Qt::RightEdge;

    m_data->rowChain.setExtraSpacingAt( rowEdges );

    invalidate( LayoutCache );
    return true;
}

Qt::Edges QskLinearLayoutEngine::extraSpacingAt() const
{
    return m_data->extraSpacingAt;
}

bool QskLinearLayoutEngine::setDefaultAlignment( Qt::Alignment alignment )
{
    return m_data->entryTable.setDefaultAlignment( alignment );
}

Qt::Alignment QskLinearLayoutEngine::defaultAlignment() const
{
    return m_data->entryTable.defaultAlignment();
}

void QskLinearLayoutEngine::insertItem( QQuickItem* item, int index )
{
    m_data->entryTable.insertItem( index, item );
    invalidate( CellCache | LayoutCache );
}

void QskLinearLayoutEngine::addItem( QQuickItem* item )
{
    insertItem( item, -1 );
}

void QskLinearLayoutEngine::insertSpacerAt( int index, qreal spacing )
{
    m_data->entryTable.insertSpacer( index, spacing );
    invalidate( CellCache | LayoutCache );
}

void QskLinearLayoutEngine::addSpacer( qreal spacing )
{
    insertSpacerAt( -1, spacing );
}

void QskLinearLayoutEngine::removeAt( int index )
{
    if ( m_data->entryTable.removeAt( index ) )
        invalidate( CellCache | LayoutCache );
}

QQuickItem* QskLinearLayoutEngine::itemAt( int index ) const
{
    return m_data->entryTable.itemAt( index );
}

int QskLinearLayoutEngine::spacerAt( int index ) const
{
    return m_data->entryTable.spacerAt( index );
}

void QskLinearLayoutEngine::invalidate( int what )
{
    if ( m_data->blockInvalidate )
        return;

    if ( what & EntryCache )
        m_data->entryTable.invalidate();

    if ( what & CellCache )
    {
        m_data->rowChain.invalidate();
        m_data->colChain.invalidate();
    }

    if ( what & LayoutCache )
        m_data->geometries.invalidate();
}

void QskLinearLayoutEngine::setGeometries( const QRectF& rect )
{
    if ( m_data->entryTable.count() == 0 )
        return;

    if ( m_data->geometries.boundingSize != rect.size() )
    {
        m_data->blockInvalidate = true;
        updateCellGeometries( rect.size() );
        m_data->blockInvalidate = false;
    }

    /*
        In case we have items that send LayoutRequest events on
        geometry changes - what doesn't make much sense - we
        better make a ( implicitely shared ) copy of the geometries.
     */
    const auto geometries = m_data->geometries;

    uint row = 0;
    uint col = 0;

    const auto& entryTable = m_data->entryTable;

    for ( int i = 0; i < entryTable.count(); i++ )
    {
        if ( entryTable.isIgnoredAt( i ) )
            continue;

        if ( auto item = entryTable.itemAt( i ) )
        {
            QRectF r = geometries.geometryAt( row, col );
            r.translate( rect.x(), rect.y() );

            const auto alignment = entryTable.effectiveAlignmentAt( i );

            r = QskLayoutConstraint::itemRect( item, r, alignment );

            if ( m_data->visualDirection == Qt::RightToLeft )
                r.moveRight( rect.right() - ( r.left() - rect.left() ) );

            qskSetItemGeometry( item, r );
        }

        if ( entryTable.orientation() == Qt::Horizontal )
        {
            if ( ++col == entryTable.dimension() )
            {
                col = 0;
                row++;
            }
        }
        else
        {
            if ( ++row == entryTable.dimension() )
            {
                row = 0;
                col++;
            }
        }
    }
}

QSizeF QskLinearLayoutEngine::sizeHint( Qt::SizeHint which, const QSizeF& constraint ) const
{
    const auto& entryTable = m_data->entryTable;

    if ( entryTable.effectiveCount() == 0 )
        return QSizeF( 0.0, 0.0 );

    const auto constraintType = m_data->entryTable.constraintType();

    auto& colChain = m_data->colChain;
    auto& rowChain = m_data->rowChain;

    m_data->blockInvalidate = true;

    if ( ( constraint.width() >= 0 ) &&
        ( constraintType == QskLayoutConstraint::HeightForWidth ) )
    {
        qskUpdateLayoutChain( Qt::Horizontal, entryTable, colChain );

        const auto cellConstraints = colChain.geometries( constraint.width() );
        qskUpdateLayoutChain( Qt::Vertical, cellConstraints, entryTable, rowChain );
    }
    else if ( ( constraint.height() >= 0 ) &&
        ( constraintType == QskLayoutConstraint::WidthForHeight ) )
    {
        qskUpdateLayoutChain( Qt::Vertical, entryTable, rowChain );

        const auto cellConstraints = rowChain.geometries( constraint.height() );
        qskUpdateLayoutChain( Qt::Horizontal, cellConstraints, entryTable, colChain );
    }
    else
    {
        qskUpdateLayoutChain( Qt::Horizontal, entryTable, colChain );
        qskUpdateLayoutChain( Qt::Vertical, entryTable, rowChain );
    }

    m_data->blockInvalidate = false;

    const qreal width = colChain.boundingHint().size( which );
    const qreal height = rowChain.boundingHint().size( which );

    return QSizeF( width, height );
}

qreal QskLinearLayoutEngine::widthForHeight( qreal height ) const
{
    const QSizeF constraint( -1, height );
    return sizeHint( Qt::PreferredSize, constraint ).width();
}

qreal QskLinearLayoutEngine::heightForWidth( qreal width ) const
{
    const QSizeF constraint( width, -1 );
    return sizeHint( Qt::PreferredSize, constraint ).height();
}

bool QskLinearLayoutEngine::setVisualDirection(Qt::LayoutDirection direction)
{
    if ( m_data->visualDirection != direction )
    {
        m_data->visualDirection = direction;
        return true;
    }

    return false;
}

Qt::LayoutDirection QskLinearLayoutEngine::visualDirection() const
{
    return m_data->visualDirection;
}

void QskLinearLayoutEngine::updateCellGeometries( const QSizeF& size )
{
    auto& geometries = m_data->geometries;
    geometries.boundingSize = size;

    auto& colChain = m_data->colChain;
    auto& rowChain = m_data->rowChain;
    auto& entryTable = m_data->entryTable;

    const QVector< QskLayoutChain::Range > noConstraints;

    switch( entryTable.constraintType() )
    {
        case QskLayoutConstraint::WidthForHeight:
        {
            qskUpdateLayoutChain( Qt::Vertical, entryTable, rowChain );
            geometries.rows = rowChain.geometries( size.height() );

            qskUpdateLayoutChain( Qt::Horizontal, geometries.rows, entryTable, colChain );
            geometries.columns = colChain.geometries( size.width() );

            break;
        }
        case QskLayoutConstraint::HeightForWidth:
        {
            qskUpdateLayoutChain( Qt::Horizontal, entryTable, colChain );
            geometries.columns = colChain.geometries( size.width() );

            qskUpdateLayoutChain( Qt::Vertical, geometries.columns, entryTable, rowChain );
            geometries.rows = rowChain.geometries( size.height() );

            break;
        }
        default:
        {
            qskUpdateLayoutChain( Qt::Horizontal, entryTable, colChain );
            geometries.columns = colChain.geometries( size.width() );

            qskUpdateLayoutChain( Qt::Vertical, entryTable, rowChain );
            geometries.rows = rowChain.geometries( size.height() );
        }
    }
}

qreal QskLinearLayoutEngine::defaultSpacing( Qt::Orientation ) const
{
    return qskDefaultSpacing();
}
