#include "QskLinearLayoutEngine.h"

#include "QskLayoutHint.h"
#include "QskLayoutConstraint.h"
#include "QskSizePolicy.h"
#include "QskQuick.h"

#include <qvector.h>
#include <qvarlengtharray.h>
#include <vector>

#ifdef QSK_LAYOUT_COMPAT
#include <cmath>
#endif

static constexpr qreal qskDefaultSpacing()
{
    // should be a skin hint
    return 5.0;
}

namespace
{
    class Range
    {
      public:
        inline qreal end() const { return start + length; }

        qreal start = 0.0;
        qreal length = 0.0;
    };

    class CellData
    {
      public:
        QskLayoutHint hint;
        int stretch = 0;
        bool growFlag = false; // using stretch: -1
    };

    class CellTable
    {
      public:
        void invalidate();

        void reset( int count, qreal constraint );
        void addCellData( int index, const CellData& );
        void finish();

        bool setSpacing( qreal spacing );
        qreal spacing() const { return m_spacing; }

        void setExtraSpacingAt( Qt::Edges edges ) { m_extraSpacingAt = edges; }

        QVector< Range > cellRanges( qreal size ) const;
        QskLayoutHint boundingHint() const { return m_boundingHint; }

        inline qreal constraint() const { return m_constraint; }
        inline int count() const { return m_cells.size(); }

      private:
        QVector< Range > distributed( int which, qreal offset, qreal extra ) const;
        QVector< Range > minimumExpanded( qreal size ) const;
        QVector< Range > preferredStretched( qreal size ) const;

        QskLayoutHint m_boundingHint;
        qreal m_constraint = -2;

        qreal m_spacing = 0;
        Qt::Edges m_extraSpacingAt;

        int m_sumStretches = 0;
        std::vector< CellData > m_cells;
    };

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

        QVector< Range > rows;
        QVector< Range > columns;
    };
}

void CellTable::invalidate()
{
    m_cells.clear();
    m_constraint = -2;
}

void CellTable::reset( int count, qreal constraint )
{
    m_cells.assign( count, CellData() );
    m_constraint = constraint;
}

void CellTable::addCellData( int index, const CellData& data )
{
    auto& combinedData = m_cells[ index ];

    combinedData.growFlag |= data.growFlag;
    combinedData.stretch = qMax( combinedData.stretch, data.stretch );

    m_sumStretches += data.stretch;

    combinedData.hint.intersect( data.hint );
}

void CellTable::finish()
{
    qreal minimum = 0.0;
    qreal preferred = 0.0;
    qreal maximum = 0.0;

    if ( !m_cells.empty() )
    {
        for ( auto& cellData : m_cells )
        {
            minimum += cellData.hint.minimum();
            preferred += cellData.hint.preferred();

            if ( cellData.stretch == 0 && !cellData.growFlag )
                maximum += cellData.hint.preferred();
            else
                maximum += cellData.hint.maximum();  // overflow ???
        }

        const qreal spacing = ( m_cells.size() - 1 ) * m_spacing;

        minimum += spacing;
        preferred += spacing;
        maximum += spacing;
    }

    m_boundingHint.setMinimum( minimum );
    m_boundingHint.setPreferred( preferred );
    m_boundingHint.setMaximum( maximum );
}

bool CellTable::setSpacing( qreal spacing )
{
    if ( m_spacing != spacing )
    {
        m_spacing = spacing;
        return true;
    }

    return false;
}

QVector< Range > CellTable::cellRanges( qreal size ) const
{
    QVector< Range > ranges;

    if ( size <= m_boundingHint.minimum() )
    {
        ranges = distributed( Qt::MinimumSize, 0.0, 0.0 );
    }
    else if ( size < m_boundingHint.preferred() )
    {
        ranges = minimumExpanded( size );
    }
    else if ( size <= m_boundingHint.maximum() )
    {
        ranges = preferredStretched( size );
    }
    else
    {
        const qreal padding = size - m_boundingHint.maximum();

        qreal offset = 0.0;
        qreal extra = 0.0;;

        if ( m_extraSpacingAt == Qt::LeftEdge )
        {
            offset = padding;
        }
        else if ( m_extraSpacingAt == Qt::RightEdge )
        {
            offset = 0.0;
        }
        else if ( m_extraSpacingAt == ( Qt::LeftEdge | Qt::RightEdge ) )
        {
            offset = 0.5 * padding;
        }
        else
        {
            extra = padding / m_cells.size();
        }

        ranges = distributed( Qt::MaximumSize, offset, extra );
    }

    return ranges;
}

QVector< Range > CellTable::distributed(
    int which, qreal offset, const qreal extra ) const
{
    qreal fillSpacing = 0.0;

    QVector< Range > ranges( m_cells.size() );

    for ( int i = 0; i < ranges.count(); i++ )
    {
        auto& range = ranges[i];

        offset += fillSpacing;
        fillSpacing = m_spacing;

        range.start = offset;
        range.length = m_cells[i].hint.size( which ) + extra;

        offset += range.length;
    }

    return ranges;
}

QVector< Range > CellTable::minimumExpanded( qreal size ) const
{
    QVector< Range > ranges( m_cells.size() );

    qreal fillSpacing = 0.0;
    qreal offset = 0.0;

    /*
        We have different options how to distribute the availabe space

        - according to the preferred sizes

        - items with a larger preferred size are stretchier: this is
          what QSK_LAYOUT_COMPAT does ( compatible with QGridLayoutEngine )

        - somehow using the stretch factors
     */

#if QSK_LAYOUT_COMPAT

    /*
         Code does not make much sense, but this is what QGridLayoutEngine does.
         The implementation is intended to help during the migration, but is supposed
         to be removed then TODO ...
     */
    qreal sumFactors = 0.0;
    QVarLengthArray< qreal > factors( m_cells.size() );

    const qreal desired = m_boundingHint.preferred() - m_boundingHint.minimum();
    const qreal available = size - m_boundingHint.minimum();

    for ( uint i = 0; i < m_cells.size(); i++ )
    {
        const auto& hint = m_cells[i].hint;

        const qreal l = hint.preferred() - hint.minimum();

        factors[i] = l * std::pow( available / desired, l / desired );
        sumFactors += factors[i];
    }


    for ( uint i = 0; i < m_cells.size(); i++ )
    {
        const auto& hint = m_cells[i].hint;

        auto& range = ranges[i];

        offset += fillSpacing;
        fillSpacing = m_spacing;

        range.start = offset;
        range.length = hint.minimum() + available * ( factors[i] / sumFactors );

        offset += range.length;
    }
#else
    const qreal factor = ( size - m_boundingHint.minimum() ) /
        ( m_boundingHint.preferred() - m_boundingHint.minimum() );

    for ( uint i = 0; i < m_cells.size(); i++ )
    {
        const auto& hint = m_cells[i].hint;

        auto& range = ranges[i];

        offset += fillSpacing;
        fillSpacing = m_spacing;

        range.start = offset;
        range.length = hint.minimum() + factor * ( hint.preferred() - hint.minimum() );

        offset += range.length;
    }
#endif

    return ranges;
}

QVector< Range > CellTable::preferredStretched( qreal size ) const
{
    const int count = m_cells.size();
    auto sumSizes = size - ( count - 1 ) * m_spacing;

    qreal sumFactors = 0.0;
    QVarLengthArray< qreal > factors( count );

    for ( int i = 0; i < count; i++ )
    {
        const auto& hint = m_cells[i].hint;

        if ( hint.preferred() >= hint.maximum() )
        {
            factors[i] = 0.0;
        }
        else
        {
            if ( m_sumStretches == 0 )
                factors[i] = m_cells[i].growFlag ? 1.0 : 0.0;
            else
                factors[i] = m_cells[i].stretch;
        }

        sumFactors += factors[i];
    }

    QVector< Range > ranges( count );

    Q_FOREVER
    {
        bool done = true;

        for ( int i = 0; i < count; i++ )
        {
            if ( factors[i] < 0.0 )
                continue;

            const auto size = sumSizes * factors[i] / sumFactors;

            const auto& hint = m_cells[i].hint;
            const auto boundedSize =
                qBound( hint.preferred(), size, hint.maximum() );

            if ( boundedSize != size )
            {
                ranges[i].length = boundedSize;
                sumSizes -= boundedSize;
                sumFactors -= factors[i];
                factors[i] = -1.0;

                done = false;
            }
        }

        if ( done )
            break;
    }

    qreal offset = 0;
    qreal fillSpacing = 0.0;

    for ( int i = 0; i < count; i++ )
    {
        auto& range = ranges[i];
        const auto& factor = factors[i];

        offset += fillSpacing;
        fillSpacing = m_spacing;

        range.start = offset;

        if ( factor >= 0.0 )
        {
            if ( factor > 0.0 )
                range.length = sumSizes * factor / sumFactors;
            else
                range.length = m_cells[i].hint.preferred();
        }

        offset += range.length;
    }

    return ranges;
}

namespace
{
    class EntryData
    {
      public:
        EntryData( QQuickItem* item );
        EntryData( qreal spacing );

        EntryData& operator=( const EntryData& );

        bool isIgnored() const;
        bool isConstrained( Qt::Orientation ) const;

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

        void updateCellTable( Qt::Orientation,
            const QVector< Range >& constraints, CellTable& ) const;

        QskLayoutConstraint::Type constraintType() const;

        void invalidate();

      private:
        CellData cellData( const EntryData&,
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
        mutable int m_constrainedType : 3;

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

bool EntryData::isConstrained( Qt::Orientation orientation ) const
{
    if ( m_isSpacer )
        return false;

    switch( QskLayoutConstraint::constraintType( m_item ) )
    {
        case QskLayoutConstraint::WidthForHeight:
            return orientation == Qt::Horizontal;

        case QskLayoutConstraint::HeightForWidth:
            return orientation == Qt::Vertical;

        default:
            return false;
    }
}

EntryTable::EntryTable( Qt::Orientation orientation, uint dimension )
    : m_dimension( dimension )
    , m_sumIgnored( -1 )
    , m_constrainedType( -1 )
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
    m_constrainedType = -1;
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
    if ( m_constrainedType < 0 )
    {
        m_constrainedType = QskLayoutConstraint::Unconstrained;

        for ( const auto& entry : m_entries )
        {
            const auto itemType = QskLayoutConstraint::constraintType( entry.item() );

            if ( itemType != QskLayoutConstraint::Unconstrained )
            {
                if ( m_constrainedType == QskLayoutConstraint::Unconstrained )
                {
                    m_constrainedType = itemType;
                }
                else if ( m_constrainedType != itemType )
                {
                    qWarning( "QskLinearLayoutEngine: conflicting constraints");
                    m_constrainedType = QskLayoutConstraint::Unconstrained;
                }
            }
        }
    }

    return static_cast< QskLayoutConstraint::Type >( m_constrainedType );
}

CellData EntryTable::cellData( const EntryData& entry,
    Qt::Orientation orientation, qreal constraint ) const
{
    int stretch = 0;
    bool growFlag = true;
    qreal minimum, preferred, maximum;

    if ( const auto item = entry.item() )
    {
        const auto policy = QskLayoutConstraint::sizePolicy( item ).policy( orientation );

        if ( constraint >= 0.0 )
        {
            if ( !entry.isConstrained( orientation ) )
                constraint = -1.0;
        }

        const auto expandFlags = QskSizePolicy::GrowFlag | QskSizePolicy::ExpandFlag;

        if ( ( policy & QskSizePolicy::ShrinkFlag ) &&
            ( policy & expandFlags ) && ( policy & QskSizePolicy::IgnoreFlag ) )
        {
            // we don't need to calculate the preferred size

            minimum = QskLayoutConstraint::sizeHint(
                item, Qt::MinimumSize, orientation, constraint );

            maximum = QskLayoutConstraint::sizeHint(
                item, Qt::MaximumSize, orientation, constraint );

            preferred = minimum;
        }
        else
        {
            preferred = QskLayoutConstraint::sizeHint(
                item, Qt::PreferredSize, orientation, constraint );

            minimum = maximum = preferred;

            if ( policy & QskSizePolicy::ShrinkFlag )
            {
                minimum = QskLayoutConstraint::sizeHint(
                    item, Qt::MinimumSize, orientation, constraint );
            }

            if ( policy & expandFlags )
            {
                maximum = QskLayoutConstraint::sizeHint(
                    item, Qt::MaximumSize, orientation, constraint );
            }

            if ( policy & QskSizePolicy::IgnoreFlag )
                preferred = minimum;
        }

        if ( orientation == m_orientation )
        {
            if ( entry.stretch() < 0 )
                stretch = ( policy & QskSizePolicy::ExpandFlag ) ? 1 : 0;
            else
                stretch = entry.stretch();
        }

        growFlag = policy & QskSizePolicy::GrowFlag;
    }
    else
    {
        // a spacer

        if ( orientation == m_orientation )
        {
            minimum = preferred = maximum = entry.spacer();

            // >= 0 ???
            if ( entry.stretch() > 0 )
                maximum = QskLayoutConstraint::unlimited;

            stretch = qMax( entry.stretch(), 0 );
        }
        else
        {
            minimum = 0.0;
            preferred = 0.0;
            maximum = QskLayoutConstraint::unlimited;
        }
    }

    CellData cellData;
    cellData.hint = QskLayoutHint( minimum, preferred, maximum );
    cellData.stretch = stretch;
    cellData.growFlag = growFlag;

    return cellData;
}

void EntryTable::updateCellTable( Qt::Orientation orientation,
    const QVector< Range >& constraints, CellTable& cellTable ) const
{
    const auto count = effectiveCount( orientation );
    const qreal constraint =
        constraints.isEmpty() ? -1.0 : constraints.last().end();

    if ( ( cellTable.constraint() == constraint )
        && ( cellTable.count() == count ) )
    {
        return; // already up to date
    }

    cellTable.reset( count, constraint );

    uint index1 = 0;
    uint index2 = 0;

    for ( const auto& entry : m_entries )
    {
        if ( entry.isIgnored() )
            continue;

        const qreal cellConstraint =
            constraints.isEmpty() ? -1.0 : constraints[index1].length;

        const auto data = cellData( entry, orientation, cellConstraint );
        cellTable.addCellData( index2, data );

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

    cellTable.finish();
}

// ---------

static inline void qskUpdateCellTable( Qt::Orientation orientation,
    const QVector< Range >& constraints,
    const EntryTable& entryTable, CellTable& cellTable )
{
    entryTable.updateCellTable( orientation, constraints, cellTable );
}

static inline void qskUpdateCellTable( Qt::Orientation orientation,
    const EntryTable& entryTable, CellTable& cellTable )
{
    entryTable.updateCellTable( orientation, QVector< Range >(), cellTable );
}

class QskLinearLayoutEngine::PrivateData
{
  public:

    PrivateData( Qt::Orientation orientation, uint dimension )
        : entryTable( orientation, dimension )
        , blockInvalidate( false )
    {
        rowTable.setSpacing( qskDefaultSpacing() );
        colTable.setSpacing( qskDefaultSpacing() );
    }

    EntryTable entryTable;

    Qt::LayoutDirection visualDirection = Qt::LeftToRight;
    Qt::Edges extraSpacingAt;

    CellTable colTable;
    CellTable rowTable;

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

void QskLinearLayoutEngine::setOrientation( Qt::Orientation orientation )
{
    if ( m_data->entryTable.setOrientation( orientation ) )
        invalidate( CellCache | LayoutCache );
}

Qt::Orientation QskLinearLayoutEngine::orientation() const
{
    return m_data->entryTable.orientation();
}

void QskLinearLayoutEngine::setDimension( uint dimension )
{
    if ( dimension < 1 )
        dimension = 1;

    if ( m_data->entryTable.setDimension( dimension ) )
        invalidate( CellCache | LayoutCache );
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

void QskLinearLayoutEngine::setRetainSizeWhenHiddenAt( int index, bool on )
{
    if ( m_data->entryTable.setRetainSizeWhenHiddenAt( index, on ) )
        invalidate( CellCache | LayoutCache );
}

bool QskLinearLayoutEngine::retainSizeWhenHiddenAt( int index ) const
{
    return m_data->entryTable.retainSizeWhenHiddenAt( index );
}

void QskLinearLayoutEngine::setStretchFactorAt( int index, int stretchFactor )
{
    if ( m_data->entryTable.setStretchFactorAt( index, stretchFactor ) )
        invalidate( CellCache | LayoutCache );
}

int QskLinearLayoutEngine::stretchFactorAt( int index ) const
{
    return m_data->entryTable.stretchFactorAt( index );
}

void QskLinearLayoutEngine::setAlignmentAt( int index, Qt::Alignment alignment )
{
    m_data->entryTable.setAlignmentAt( index, alignment );
}

Qt::Alignment QskLinearLayoutEngine::alignmentAt( int index ) const
{
    return m_data->entryTable.alignmentAt( index );
}

void QskLinearLayoutEngine::setSpacing( qreal spacing, Qt::Orientations orientations )
{
    if ( spacing < 0.0 )
        spacing = 0.0;

    bool doInvalidate = false;

    if ( orientations & Qt::Horizontal )
        doInvalidate |= m_data->colTable.setSpacing( spacing );

    if ( orientations & Qt::Vertical )
        doInvalidate |= m_data->rowTable.setSpacing( spacing );

    if ( doInvalidate )
        invalidate( CellCache | LayoutCache );
}

qreal QskLinearLayoutEngine::spacing( Qt::Orientation orientation ) const
{
    if ( orientation == Qt::Horizontal )
        return m_data->colTable.spacing();
    else
        return m_data->rowTable.spacing();
}

void QskLinearLayoutEngine::setExtraSpacingAt( Qt::Edges edges )
{
    if ( edges == m_data->extraSpacingAt )
        return;

    m_data->extraSpacingAt = edges;

    Qt::Edges colEdges = edges & ~( Qt::TopEdge | Qt::BottomEdge );
    m_data->colTable.setExtraSpacingAt( colEdges );

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

    m_data->rowTable.setExtraSpacingAt( rowEdges );

    invalidate( LayoutCache );
}

Qt::Edges QskLinearLayoutEngine::extraSpacingAt() const
{
    return m_data->extraSpacingAt;
}

void QskLinearLayoutEngine::setDefaultAlignment( Qt::Alignment alignment )
{
    m_data->entryTable.setDefaultAlignment( alignment );
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
        m_data->rowTable.invalidate();
        m_data->colTable.invalidate();
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

    auto& colTable = m_data->colTable;
    auto& rowTable = m_data->rowTable;

    m_data->blockInvalidate = true;

    if ( ( constraint.width() >= 0 ) &&
        ( constraintType == QskLayoutConstraint::HeightForWidth ) )
    {
        qskUpdateCellTable( Qt::Horizontal, entryTable, colTable );

        const auto cellConstraints = colTable.cellRanges( constraint.width() );
        qskUpdateCellTable( Qt::Vertical, cellConstraints, entryTable, rowTable );
    }
    else if ( ( constraint.height() >= 0 ) &&
        ( constraintType == QskLayoutConstraint::WidthForHeight ) )
    {
        qskUpdateCellTable( Qt::Vertical, entryTable, rowTable );

        const auto cellConstraints = rowTable.cellRanges( constraint.height() );
        qskUpdateCellTable( Qt::Horizontal, cellConstraints, entryTable, colTable );
    }
    else
    {
        qskUpdateCellTable( Qt::Horizontal, entryTable, colTable );
        qskUpdateCellTable( Qt::Vertical, entryTable, rowTable );
    }

    m_data->blockInvalidate = false;

    const qreal width = colTable.boundingHint().size( which );
    const qreal height = rowTable.boundingHint().size( which );

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

void QskLinearLayoutEngine::setVisualDirection(Qt::LayoutDirection direction)
{
    m_data->visualDirection = direction;
}

Qt::LayoutDirection QskLinearLayoutEngine::visualDirection() const
{
    return m_data->visualDirection;
}

void QskLinearLayoutEngine::updateCellGeometries( const QSizeF& size )
{
    auto& geometries = m_data->geometries;
    geometries.boundingSize = size;

    auto& colTable = m_data->colTable;
    auto& rowTable = m_data->rowTable;
    auto& entryTable = m_data->entryTable;

    const QVector< Range > noConstraints;

    switch( entryTable.constraintType() )
    {
        case QskLayoutConstraint::WidthForHeight:
        {
            qskUpdateCellTable( Qt::Vertical, entryTable, rowTable );
            geometries.rows = rowTable.cellRanges( size.height() );

            qskUpdateCellTable( Qt::Horizontal, geometries.rows, entryTable, colTable );
            geometries.columns = colTable.cellRanges( size.width() );

            break;
        }
        case QskLayoutConstraint::HeightForWidth:
        {
            qskUpdateCellTable( Qt::Horizontal, entryTable, colTable );
            geometries.columns = colTable.cellRanges( size.width() );

            qskUpdateCellTable( Qt::Vertical, geometries.columns, entryTable, rowTable );
            geometries.rows = rowTable.cellRanges( size.height() );

            break;
        }
        default:
        {
            qskUpdateCellTable( Qt::Horizontal, entryTable, colTable );
            geometries.columns = colTable.cellRanges( size.width() );

            qskUpdateCellTable( Qt::Vertical, entryTable, rowTable );
            geometries.rows = rowTable.cellRanges( size.height() );
        }
    }
}

qreal QskLinearLayoutEngine::defaultSpacing( Qt::Orientation ) const
{
    return qskDefaultSpacing();
}
