/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLayoutEngine2D.h"
#include "QskLayoutChain.h"
#include "QskLayoutElement.h"
#include "QskFunctions.h"

#include <qguiapplication.h>

namespace
{
    class LayoutData
    {
      public:

        QRectF geometryAt( const QRect& grid ) const
        {
            const auto x1 = columns[ grid.left() ].start;
            const auto x2 = columns[ grid.right() ].end();
            const auto y1 = rows[ grid.top() ].start;
            const auto y2 = rows[ grid.bottom() ].end();

            return QRectF( rect.x() + x1, rect.y() + y1, x2 - x1, y2 - y1 );
        }

        Qt::LayoutDirection direction;

        QRectF rect;
        QskLayoutChain::Segments rows;
        QskLayoutChain::Segments columns;
    };
}

class QskLayoutEngine2D::PrivateData
{
  public:
    PrivateData()
        : defaultAlignment( Qt::AlignLeft | Qt::AlignVCenter )
        , extraSpacingAt( 0 )
        , visualDirection( Qt::LeftToRight )
        , constraintType( -1 )
        , blockInvalidate( false )
    {
    }

    inline QskLayoutChain& layoutChain( Qt::Orientation orientation )
    {
        return ( orientation == Qt::Horizontal ) ? columnChain : rowChain;
    }

    inline Qt::Alignment effectiveAlignment( Qt::Alignment alignment ) const
    {
        const auto align = static_cast< Qt::Alignment >( defaultAlignment );

        if ( !( alignment & Qt::AlignVertical_Mask ) )
            alignment |= ( align & Qt::AlignVertical_Mask );

        if ( !( alignment & Qt::AlignHorizontal_Mask ) )
            alignment |= ( align & Qt::AlignHorizontal_Mask );

        return alignment;
    }

    QskLayoutChain columnChain;
    QskLayoutChain rowChain;

    QSizeF layoutSize;

    QskLayoutChain::Segments rows;
    QskLayoutChain::Segments columns;

    const LayoutData* layoutData = nullptr;

    unsigned int defaultAlignment : 8;
    unsigned int extraSpacingAt : 4;
    unsigned int visualDirection : 4;

    int constraintType : 3;

    /*
        Some weired controls do lazy updates inside of their sizeHint calculation
        that lead to LayoutRequest events. While being in the process of
        updating the tables we can't - and don't need to - handle invalidations
        because of them.
     */
    bool blockInvalidate : 1;
};

QskLayoutEngine2D::QskLayoutEngine2D()
    : m_data( new PrivateData )
{
    m_data->columnChain.setSpacing( defaultSpacing( Qt::Horizontal ) );
    m_data->rowChain.setSpacing( defaultSpacing( Qt::Vertical ) );
}

QskLayoutEngine2D::~QskLayoutEngine2D()
{
}

bool QskLayoutEngine2D::setVisualDirection( Qt::LayoutDirection direction )
{
    if ( m_data->visualDirection != direction )
    {
        m_data->visualDirection = direction;
        return true;
    }

    return false;
}

Qt::LayoutDirection QskLayoutEngine2D::visualDirection() const
{
    return static_cast< Qt::LayoutDirection >( m_data->visualDirection );
}

bool QskLayoutEngine2D::setDefaultAlignment( Qt::Alignment alignment )
{
    if ( defaultAlignment() != alignment )
    {
        m_data->defaultAlignment = alignment;
        return true;
    }

    return false;
}

Qt::Alignment QskLayoutEngine2D::defaultAlignment() const
{
    return static_cast< Qt::Alignment >( m_data->defaultAlignment );
}

qreal QskLayoutEngine2D::defaultSpacing( Qt::Orientation ) const
{
    return 5.0; // should be from the skin
}

bool QskLayoutEngine2D::setSpacing(
    qreal spacing, Qt::Orientations orientations )
{
    if ( spacing < 0.0 )
        spacing = 0.0;

    bool isModified = false;

    for ( auto o : { Qt::Horizontal, Qt::Vertical } )
    {
        if ( orientations & o )
            isModified |= m_data->layoutChain( o ).setSpacing( spacing );
    }

    if ( isModified )
        invalidate( LayoutCache );

    return isModified;
}

qreal QskLayoutEngine2D::spacing( Qt::Orientation orientation ) const
{
    return m_data->layoutChain( orientation ).spacing();
}

bool QskLayoutEngine2D::setExtraSpacingAt( Qt::Edges edges )
{
    if ( edges == extraSpacingAt() )
        return false;

    m_data->extraSpacingAt = edges;

    {
        int fillMode = 0;

        if ( edges & Qt::LeftEdge )
            fillMode |= QskLayoutChain::Leading;

        if ( edges & Qt::RightEdge )
            fillMode |= QskLayoutChain::Trailing;

        m_data->columnChain.setFillMode( fillMode );
    }

    {
        int fillMode = 0;

        if ( edges & Qt::TopEdge )
            fillMode |= QskLayoutChain::Leading;

        if ( edges & Qt::BottomEdge )
            fillMode |= QskLayoutChain::Trailing;

        m_data->rowChain.setFillMode( fillMode );
    }

    m_data->layoutSize = QSize();
    m_data->rows.clear();
    m_data->columns.clear();

    return true;
}

Qt::Edges QskLayoutEngine2D::extraSpacingAt() const
{
    return static_cast< Qt::Edges >( m_data->extraSpacingAt );
}

void QskLayoutEngine2D::setGeometries( const QRectF& rect )
{
    if ( rowCount() < 1 || columnCount() < 1 )
        return;

    if ( m_data->layoutSize != rect.size() )
    {
        m_data->layoutSize = rect.size();
        updateSegments( rect.size() );
    }

    /*
        In case we have items that send LayoutRequest events on
        geometry changes - what doesn't make much sense - we
        better make a ( implicitely shared ) copy of the rows/columns.
     */
    LayoutData data;
    data.rows = m_data->rows;
    data.columns = m_data->columns;
    data.rect = rect;

    data.direction = visualDirection();
    if ( data.direction == Qt::LayoutDirectionAuto )
        data.direction = QGuiApplication::layoutDirection();

    m_data->layoutData = &data;
    layoutItems();
    m_data->layoutData = nullptr;
}

QRectF QskLayoutEngine2D::geometryAt(
    const QskLayoutElement* element, const QRect& grid ) const
{
    const auto layoutData = m_data->layoutData;

    if ( layoutData && element )
    {
        auto rect = layoutData->geometryAt( grid );

        const auto size = element->constrainedSize( rect.size() );
        const auto alignment = m_data->effectiveAlignment( element->alignment() );

        rect = qskAlignedRectF( rect, size, alignment );

        if ( layoutData->direction == Qt::RightToLeft )
        {
            const auto& r = layoutData->rect;
            rect.moveRight( r.right() - ( rect.left() - r.left() ) );
        }

        return rect;
    }

    return QRectF( 0, 0, -1, -1 );
}

qreal QskLayoutEngine2D::widthForHeight( qreal height ) const
{
    const QSizeF constraint( -1, height );
    return sizeHint( Qt::PreferredSize, constraint ).width();
}

qreal QskLayoutEngine2D::heightForWidth( qreal width ) const
{
    const QSizeF constraint( width, -1 );
    return sizeHint( Qt::PreferredSize, constraint ).height();
}

QSizeF QskLayoutEngine2D::sizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( constraint.isValid() )
        return constraint; // should never happen

    if ( effectiveCount( Qt::Horizontal ) <= 0 )
        return QSizeF( 0.0, 0.0 );

    auto requestType = constraintType();

    switch ( requestType )
    {
        case QskSizePolicy::HeightForWidth:
        {
            if ( constraint.height() >= 0 )
            {
                qWarning( "QskLayoutEngine2D: HeightForWidth conflict." );
                return QSizeF();
            }

            if ( constraint.width() <= 0 )
                requestType = QskSizePolicy::Unconstrained;

            break;
        }
        case QskSizePolicy::WidthForHeight:
        {
            if ( constraint.width() >= 0 )
            {
                qWarning( "QskLayoutEngine2D: WidthForHeight conflict." );
                return QSizeF();
            }

            if ( constraint.height() <= 0 )
                requestType = QskSizePolicy::Unconstrained;

            break;
        }
        default:
        {
            if ( constraint.height() >= 0 || constraint.width() >= 0 )
            {
                /*
                    As none of the elements has the Constrained flag the constraint
                    will have no effect and we ignore it to make use of the cached
                    results from unconstrained requests
                 */
#if 0
                qWarning( "QskLayoutEngine2D: constraint will be ignored." );
#endif
            }
        }
    }

    auto& rowChain = m_data->rowChain;
    auto& columnChain = m_data->columnChain;

    m_data->blockInvalidate = true;

    switch( requestType )
    {
        case QskSizePolicy::HeightForWidth:
        {
            setupChain( Qt::Horizontal );

            const auto constraints = columnChain.segments( constraint.width() );
            setupChain( Qt::Vertical, constraints );

            break;
        }
        case QskSizePolicy::WidthForHeight:
        {
            setupChain( Qt::Vertical );

            const auto constraints = rowChain.segments( constraint.height() );
            setupChain( Qt::Horizontal, constraints );

            break;
        }
        default:
        {
            setupChain( Qt::Horizontal );
            setupChain( Qt::Vertical );
        }
    }

    m_data->blockInvalidate = false;

    QSizeF hint;

    if ( constraint.width() <= 0.0 )
        hint.rwidth() = columnChain.boundingMetrics().metric( which );

    if ( constraint.height() <= 0.0 )
        hint.rheight() = rowChain.boundingMetrics().metric( which );

    return hint;
}

void QskLayoutEngine2D::setupChain( Qt::Orientation orientation ) const
{
    setupChain( orientation, QskLayoutChain::Segments() );
}

void QskLayoutEngine2D::setupChain( Qt::Orientation orientation,
    const QskLayoutChain::Segments& constraints ) const
{
    const auto count = effectiveCount( orientation );
    const qreal constraint =
        constraints.isEmpty() ? -1.0 : constraints.last().end();

    auto& chain = m_data->layoutChain( orientation );

    if ( ( chain.constraint() == constraint ) && ( chain.count() == count ) )
    {
        return; // already up to date
    }

    chain.reset( count, constraint );
    setupChain( orientation, constraints, chain );
    chain.finish();

#if 0
    qDebug() << "==" << this << orientation << chain.count();

    for ( int i = 0; i < chain.count(); i++ )
        qDebug() << i << ":" << chain.cell( i );
#endif
}

void QskLayoutEngine2D::updateSegments( const QSizeF& size ) const
{
    auto& rowChain = m_data->rowChain;
    auto& columnChain = m_data->columnChain;

    auto& rows = m_data->rows;
    auto& columns = m_data->columns;

    m_data->blockInvalidate = true;

    switch( constraintType() )
    {
        case QskSizePolicy::WidthForHeight:
        {
            setupChain( Qt::Vertical );
            rows = rowChain.segments( size.height() );

            setupChain( Qt::Horizontal, rows );
            columns = columnChain.segments( size.width() );

            break;
        }
        case QskSizePolicy::HeightForWidth:
        {
            setupChain( Qt::Horizontal );
            columns = columnChain.segments( size.width() );

            setupChain( Qt::Vertical, m_data->columns );
            rows = rowChain.segments( size.height() );

            break;
        }
        default:
        {
            setupChain( Qt::Horizontal );
            columns = columnChain.segments( size.width() );

            setupChain( Qt::Vertical );
            rows = rowChain.segments( size.height() );
        }
    }

    m_data->blockInvalidate = false;
}

void QskLayoutEngine2D::invalidate( int what )
{
    if ( m_data->blockInvalidate )
        return;

    if ( what & ElementCache )
    {
        m_data->constraintType = -1;
        invalidateElementCache();
    }

    if ( what & LayoutCache )
    {
        m_data->rowChain.invalidate();
        m_data->columnChain.invalidate();

        m_data->layoutSize = QSize();
        m_data->rows.clear();
        m_data->columns.clear();
    }
}

QskSizePolicy::ConstraintType QskLayoutEngine2D::constraintType() const
{
    if ( m_data->constraintType < 0 )
    {
        auto constraintType = QskSizePolicy::Unconstrained;

        for ( int i = 0; i < count(); i++ )
        {
            const auto type = sizePolicyAt( i ).constraintType();

            if ( type != QskSizePolicy::Unconstrained )
            {
                if ( constraintType == QskSizePolicy::Unconstrained )
                {
                    constraintType = type;
                }
                else if ( constraintType != type )
                {
                    qWarning( "QskLayoutEngine2D: conflicting constraints");
                    constraintType = QskSizePolicy::Unconstrained;
                }
            }
        }

        m_data->constraintType = constraintType;
    }

    return static_cast< QskSizePolicy::ConstraintType >( m_data->constraintType );
}
