#include "QskLevelingSensorNodes.h"
#include "QskLevelingSensorUtility.h"
#include "QskSGNodeUtility.h"
#include <QskLevelingSensor.h>
#include <QskLevelingSensorSkinlet.h>

#include <QskArcMetrics.h>
#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxNode.h>
#include <QskBoxShapeMetrics.h>
#include <QskGradient.h>
#include <QskGraphic.h>
#include <QskScaleEngine.h>
#include <QskScaleTickmarks.h>
#include <QskTextColors.h>
#include <QskTextNode.h>
#include <QskTextOptions.h>
#include <QskTickmarksNode.h>

#include <QFontMetrics>
#include <QSGClipNode>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>
#include <qmath.h>
#include <QTransform>

#include <array>
#include <optional>
#include <utility>

namespace
{
    template< typename T >
    struct State;

    template<>
    struct State< QskLevelingSensor >
    {
        qreal r1 = 0.0;
        qreal r2 = 0.0;
        qreal cX = 0.0;
        qreal cY = 0.0;
        qreal sX = 0.0;
        qreal sY = 0.0;
        qreal sZ = 0.0;

        explicit State( const QskLevelingSensor* const sensor )
            : r1( QskLevelingSensorSkinlet::innerRadius( sensor ) )
            , r2( QskLevelingSensorSkinlet::outerRadius( sensor ) )
            , cX( QskLevelingSensorSkinlet::center( sensor ).x() )
            , cY( QskLevelingSensorSkinlet::center( sensor ).y() )
            , sX( r1 / sensor->angle().x() )
            , sY( r1 / sensor->angle().y() )
            , sZ( r1 / sensor->angle().z() )
        {
        }

        Q_REQUIRED_RESULT QVector2D center() const noexcept
        {
            return { ( float ) cX, ( float ) cY};
        }

        Q_REQUIRED_RESULT QVector3D scale() const noexcept
        {
            return { ( float ) sX, ( float ) sY, ( float ) sZ };
        }
    };

    template<>
    struct State< QskAspect::Subcontrol > : State< QskLevelingSensor >
    {
        qreal rX = 0.0;
        qreal rY = 0.0;
        qreal rZ = 0.0;
        qreal tX = 0.0;
        qreal tY = 0.0;
        qreal tZ = 0.0;

        State( const QskLevelingSensor* const sensor, QskAspect::Subcontrol subcontrol )
            : State< QskLevelingSensor >( sensor )
            , rX( sensor->subControlRotation( subcontrol ).x() )
            , rY( sensor->subControlRotation( subcontrol ).y() )
            , rZ( sensor->subControlRotation( subcontrol ).z() )
            , tX( rY * sX )
            , tY( rX * sY )
            , tZ( 0.0 )
        {
        }       

        Q_REQUIRED_RESULT QVector3D translation() const noexcept
        {
            return { ( float ) tX, ( float ) tY, ( float ) tZ };
        }

        Q_REQUIRED_RESULT QMatrix4x4 matrix() const noexcept
        {
            return matrix_deg( rZ, cX, cY );
        }
    };

    Q_REQUIRED_RESULT std::optional< std::pair< qreal, qreal > > minmax(
        const QVector< qreal >& tickmarks )
    {
        if ( tickmarks.empty() )
        {
            return {};
        }

        const auto [ pmin, pmax ] = std::minmax_element( tickmarks.begin(), tickmarks.end() );
        return std::make_pair( *pmin, *pmax );
    }

    Q_REQUIRED_RESULT std::optional< std::pair< qreal, qreal > > minmax(
        const QskScaleTickmarks& tickmarks )
    {
        if ( tickmarks.tickCount() == 0 )
        {
            return {};
        }

        const auto majorTicks = tickmarks.majorTicks();
        const auto mediumTicks = tickmarks.mediumTicks();
        const auto minorTicks = tickmarks.minorTicks();

        const auto item = !majorTicks.empty()    ? majorTicks[ 0 ]
                          : !mediumTicks.empty() ? mediumTicks[ 0 ]
                          : !minorTicks.empty()  ? minorTicks[ 0 ]
                                                 : std::numeric_limits< qreal >::quiet_NaN();

        Q_ASSERT_X( !std::isnan( item ), __func__, "NaN should not be possible!" );

        std::array< qreal, 6 > local = { item };

        if ( const auto opt = minmax( tickmarks.majorTicks() ) )
        {
            local[ 0 ] = opt->first;
            local[ 1 ] = opt->second;
        }
        if ( const auto opt = minmax( tickmarks.mediumTicks() ) )
        {
            local[ 2 ] = opt->first;
            local[ 3 ] = opt->second;
        }
        if ( const auto opt = minmax( tickmarks.minorTicks() ) )
        {
            local[ 4 ] = opt->first;
            local[ 5 ] = opt->second;
        }

        const auto [ pmin, pmax ] = std::minmax_element( local.begin(), local.end() );
        return std::make_pair( *pmin, *pmax );
    }

}

using Q = QskLevelingSensor;
using R = QskLevelingSensorSkinlet::NodeRole;

using namespace QskSGNode;

template< typename Root, typename... Children >
inline Q_REQUIRED_RESULT Root* ensureNodes( QSGNode* root = nullptr )
{
    return ensureNodes< AppendMode::Recursive, Root, Children... >( root );
}

float QskLevelingSensorSkinlet::outerRadius( const QskSkinnable* const skinnable )
{
    const auto* const sensor = static_cast< const Q* >( skinnable );
    const auto contentsRect = sensor->contentsRect();
    return 0.5f * qMin( contentsRect.width(), contentsRect.height() );
}

float QskLevelingSensorSkinlet::innerRadius( const QskSkinnable* const skinnable )
{
    const auto scale = skinnable->strutSizeHint( Q::Horizon );
    return outerRadius( skinnable ) * scale.width();
}

QPointF QskLevelingSensorSkinlet::center( const QskSkinnable* const skinnable )
{
    const auto* const sensor = static_cast< const Q* >( skinnable );
    return sensor->contentsRect().center();
}

QskLevelingSensorSkinlet::QskLevelingSensorSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( {
        OuterDisk,
        Horizon,
        HorizonClip,
        TickmarksX,
        TickmarksXLabels,
        TickmarksY,
        TickmarksYLabels,
        TickmarksZ,
        TickmarksZLabels,
    } );
}

template< QskLevelingSensorSkinlet::NodeRole >
Q_REQUIRED_RESULT QRectF QskLevelingSensorSkinlet::subControlRect(
    const QskLevelingSensor* const sensor, const QRectF& contentsRect ) const = delete;

template<>
Q_REQUIRED_RESULT QRectF QskLevelingSensorSkinlet::subControlRect< R::OuterDisk >(
    const QskLevelingSensor* const sensor, const QRectF& contentsRect ) const
{
    const auto radius = outerRadius( sensor );
    const auto scale = sensor->strutSizeHint( Q::OuterDisk );
    const auto width = 2 * radius * scale.width();
    const auto height = width;
    const auto x = contentsRect.center().x() - width / 2;
    const auto y = contentsRect.center().y() - height / 2;
    return { x, y, width, height };
}

template<>
Q_REQUIRED_RESULT QRectF QskLevelingSensorSkinlet::subControlRect< R::Horizon >(
    const QskLevelingSensor* const sensor, const QRectF& contentsRect ) const
{
    const auto scale = sensor->strutSizeHint( Q::Horizon );
    const auto width = 2 * innerRadius( sensor ) * scale.width();
    const auto height = width;
    return { center( sensor ).x() - width / 2, center( sensor ).y() - height / 2, width, height };
}

QRectF QskLevelingSensorSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto* const sensor = static_cast< const Q* >( skinnable );

    if ( subControl == Q::OuterDisk )
    {
        return subControlRect< OuterDisk >( sensor, contentsRect );
    }
    if ( subControl == Q::Horizon )
    {
        return subControlRect< Horizon >( sensor, contentsRect );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

template< QskLevelingSensorSkinlet::NodeRole >
QSGNode* QskLevelingSensorSkinlet::updateSubNode( const QskLevelingSensor* const sensor,
    const quint8 nodeRole, QSGNode* const node ) const = delete;

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::OuterDisk >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto subControl = Q::OuterDisk;
    const auto contentsRect = sensor->contentsRect();
    const auto boxRect = subControlRect< OuterDisk >( sensor, contentsRect );
    const auto boxShapeMetrics = QskBoxShapeMetrics{ boxRect.width() / 2 };
    const auto boxBorderMetrics = sensor->boxBorderMetricsHint( subControl );
    const auto boxBorderColors = sensor->boxBorderColorsHint( subControl );
    const auto boxGradient = sensor->gradientHint( subControl );

    auto* const root = ensureNodes< QSGTransformNode, QskBoxNode >( node );
    auto* const bNode = static_cast< QskBoxNode* >( root->firstChild() );

    const auto size = outerRadius( sensor ) * sensor->strutSizeHint( Q::OuterDisk ).width();
    updateBoxNode( sensor, bNode, { 0, 0, 2 * size, 2 * size }, boxShapeMetrics, boxBorderMetrics,
        boxBorderColors, boxGradient );

    const auto cX = center( sensor ).x();
    const auto cY = center( sensor ).y();
    const auto rZ = 0.0;

    const auto matrix = matrix_deg( 0.0, cX, cY ) *
                        matrix_deg( rZ, 0, 0 ) *
                        matrix_deg( 0.0, -size, -size );

    root->setMatrix( matrix );
    return root;
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::Horizon >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto subControl = Q::Horizon;
    const State< QskAspect::Subcontrol > state( sensor, subControl );

    const auto dY = 2 * sensor->angle().y();
    const auto pY = qBound( 0.0, 0.5 + ( -state.rX / dY ), 1.0 );

    const auto shape = QskBoxShapeMetrics{ state.r1 };
    const auto metrics = sensor->boxBorderMetricsHint( subControl );
    const auto colors = sensor->boxBorderColorsHint( subControl );

    auto gradient = sensor->gradientHint( Q::Horizon );
    gradient.setDirection( QskGradient::Linear );
    gradient.setLinearDirection( Qt::Vertical );
    gradient.setStops( { { 0.0, gradient.startColor() }, { pY, gradient.startColor() },
        { pY, gradient.endColor() }, { 1.0, gradient.endColor() } } );

    auto* const tNode = ensureNodes< QSGTransformNode, QskBoxNode >( node );
    auto* const boxNode = static_cast< QskBoxNode* >( tNode->firstChild() );
    updateBoxNode(
        sensor, boxNode, { 0, 0, 2 * state.r1, 2 * state.r1 }, shape, metrics, colors, gradient );

    const auto matrix = matrix_deg( 0, state.cX, state.cY ) * matrix_deg( state.rZ, 0, 0 ) *
                        matrix_deg( 0, -state.r1, -state.r1 );

    tNode->setMatrix( matrix );
    return tNode;
}

Q_REQUIRED_RESULT QSGNode* updateLinearTickmarksNode(
    const QskLevelingSensor* const sensor,
    const QskAspect::Subcontrol subControl,
    const QskScaleTickmarks& tickmarks, 
    const Qt::Orientation orientation,
    QSGNode* const node )
{
    const auto state = State< QskAspect::Subcontrol >( sensor, subControl );
    const auto color = sensor->color( subControl );
    const auto scale = sensor->strutSizeHint( subControl );
    const auto width = state.r1 * scale.width();
    const auto height = state.r1 * scale.height();
    const auto alignment = sensor->alignmentHint( subControl );

    const auto opt = minmax( tickmarks );
    const auto min = opt ? opt->first : 0.0;
    const auto max = opt ? opt->second : 0.0;
    const auto interval = QskIntervalF{min,max};

    const auto rect =
        orientation == Qt::Horizontal
            ? QRectF{ QPointF{ min * state.sX, -height }, QPointF{ max * state.sX, +height } }
            : QRectF{ QPointF{ -width, min * state.sY }, QPointF{ +width, max * state.sY } };

    const auto translation = QPointF{state.tX + state.cX, state.tY + state.cY};

    auto* const cNode = ensureNodes< RadialClipNode, QSGTransformNode, QskTickmarksNode >( node );
    auto* const tNode = static_cast< QSGTransformNode* >( cNode->firstChild() );
    auto* const qNode = static_cast< QskTickmarksNode* >( tNode->firstChild() );
       
    cNode->setGeometryProperties( state.r1, state.cX, state.cY );
    tNode->setMatrix( matrix_deg( state.rZ, translation.x(), translation.y() ));
    qNode->update( color, rect, interval, tickmarks, 1, orientation, alignment );

    return cNode;
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::TickmarksX >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    return updateLinearTickmarksNode(
        sensor, Q::TickmarksX, sensor->tickmarks( Qt::XAxis ), Qt::Horizontal, node );
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::TickmarksY >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    return updateLinearTickmarksNode(
        sensor, Q::TickmarksY, sensor->tickmarks( Qt::YAxis ), Qt::Vertical, node );
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::TickmarksZ >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto subControl = Q::TickmarksZ;
    const State< QskAspect::Subcontrol > state( sensor, subControl );
    const auto color = sensor->color( subControl );
    const auto scale = sensor->strutSizeHint( subControl );

    const auto r3 = qvariant_cast< QVector3D >( sensor->effectiveSkinHint( subControl ) ) *
                        ( state.r2 - state.r1 ) +
                    QVector3D{ ( float ) state.r1, ( float ) state.r1, ( float ) state.r1 };

    auto* const transform = ensureNodes< QSGTransformNode, RadialTickmarksNode >( node );
    auto* const tickmarksNode = static_cast< RadialTickmarksNode* >( transform->firstChild() );
    tickmarksNode->setMaterialProperties( color );
    tickmarksNode->setGeometryProperties( sensor->tickmarks( Qt::ZAxis ), state.r1, r3 );

    const auto matrix = matrix_deg( state.rZ, state.cX, state.cY );
    transform->setMatrix( matrix );
    return transform;
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::TickmarksXLabels >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto subControl = Q::TickmarksXLabels;
    const State< QskAspect::Subcontrol > state( sensor, subControl );
    const auto r3 = state.r1 * sensor->strutSizeHint( Q::TickmarksX ).height();    
    const auto dX = qFastCos(qDegreesToRadians(90 + state.rZ)) * r3;
    const auto dY = qFastSin(qDegreesToRadians(90 + state.rZ)) * r3;

    auto* const cNode = ensureNodes< RadialClipNode, QSGTransformNode, LinearTickmarksLabelsNode >( node );
    auto* const tNode = static_cast< QSGTransformNode* >( cNode->firstChild() );
    auto* const lNode = static_cast< LinearTickmarksLabelsNode* >( tNode->firstChild() );
    cNode->setGeometryProperties( state.r1, state.cX, state.cY );
    tNode->setMatrix( matrix_deg( state.rZ, state.cX + state.tX + dX, state.cY + state.tY + dY) );
    lNode->update( sensor, subControl, sensor->tickmarkLabels( Qt::XAxis ), { state.scale().x(), 0.0 } );
    return cNode;
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::TickmarksYLabels >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto subControl = Q::TickmarksYLabels;
    const State< QskAspect::Subcontrol > state( sensor, subControl );
    const auto r3 = state.r1 * sensor->strutSizeHint( Q::TickmarksY ).width();
    const auto dX = qFastCos(qDegreesToRadians(state.rZ)) * r3;
    const auto dY = qFastSin(qDegreesToRadians(state.rZ)) * r3;

    auto* const cNode = ensureNodes< RadialClipNode, QSGTransformNode, LinearTickmarksLabelsNode >( node );
    auto* const tNode = static_cast< QSGTransformNode* >( cNode->firstChild() );
    auto* const lNode = static_cast< LinearTickmarksLabelsNode* >( tNode->firstChild() );
    cNode->setGeometryProperties( state.r1, state.cX, state.cY );
    tNode->setMatrix( matrix_deg( state.rZ, state.cX + state.tX + dX, state.cY + state.tY + dY) );
    lNode->update( sensor, subControl, sensor->tickmarkLabels( Qt::YAxis ), { 0.0, state.scale().y() } );
    return cNode;
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::TickmarksZLabels >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto subControl = Q::TickmarksZLabels;
    const State< QskAspect::Subcontrol > state( sensor, subControl );
    auto* const tNode = ensureNodes< QSGTransformNode, RadialTickmarksLabelsNode >( node );
    auto* const lNode = static_cast< RadialTickmarksLabelsNode* >( tNode->firstChild() );
    const auto r3 = static_cast< float >( state.r1 * sensor->strutSizeHint( subControl ).width() );
    lNode->update( sensor, subControl, sensor->tickmarkLabels( Qt::ZAxis ), { r3, r3 } );
    tNode->setMatrix( state.matrix() );
    return tNode;
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::HorizonClip >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto cX = center( sensor ).x();
    const auto cY = center( sensor ).y();
    const auto r1 = innerRadius( sensor );

    auto* const clipNode = ensureNodes< RadialClipNode >( node );
    clipNode->setGeometryProperties( r1, cX, cY );
    return clipNode;
}

QSGNode* QskLevelingSensorSkinlet::updateSubNode(
    const QskSkinnable* const skinnable, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto* const sensor = static_cast< const Q* >( skinnable );

    switch ( static_cast< R >( nodeRole ) )
    {
        case OuterDisk:
            return updateSubNode< OuterDisk >( sensor, nodeRole, node );
        case Horizon:
            return updateSubNode< Horizon >( sensor, nodeRole, node );
        case HorizonClip:
            return updateSubNode< HorizonClip >( sensor, nodeRole, node );
        case TickmarksX:
            return updateSubNode< TickmarksX >( sensor, nodeRole, node );
        case TickmarksXLabels:
            return updateSubNode< TickmarksXLabels >( sensor, nodeRole, node );
        case TickmarksY:
            return updateSubNode< TickmarksY >( sensor, nodeRole, node );
        case TickmarksYLabels:
            return updateSubNode< TickmarksYLabels >( sensor, nodeRole, node );
        case TickmarksZ:
            return updateSubNode< TickmarksZ >( sensor, nodeRole, node );
        case TickmarksZLabels:
            return updateSubNode< TickmarksZLabels >( sensor, nodeRole, node );
        default:
            return Inherited::updateSubNode( sensor, nodeRole, node );
    }
}

#include "moc_QskLevelingSensorSkinlet.cpp"
