#include "QskLevelingSensorSkinlet.h"
#include "QskLevelingSensor.h"
#include "QskLevelingSensorNodes.h"
#include "QskLevelingSensorUtility.h"
#include "QskSGNodeUtility.h"

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
    };

}

using Q = QskLevelingSensor;
using R = QskLevelingSensorSkinlet::NodeRole;

using namespace QskSGNode;

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

    auto* const root = ensure< QSGTransformNode, par< 1, QskBoxNode > >::node( node );
    auto* const bNode = static_cast< QskBoxNode* >( root->firstChild() );

    const auto size = outerRadius( sensor ) * sensor->strutSizeHint( Q::OuterDisk ).width();
    updateBoxNode( sensor, bNode, { 0, 0, 2 * size, 2 * size }, boxShapeMetrics, boxBorderMetrics,
        boxBorderColors, boxGradient );

    const auto cX = center( sensor ).x();
    const auto cY = center( sensor ).y();
    const auto rZ = 0.0;

    const auto matrix = matrix_deg( 0.0, 0.0, 0.0, cX, cY, 0 ) *
                        matrix_deg( 0.0, 0.0, rZ, 0, 0, 0 ) *
                        matrix_deg( 0.0, 0.0, 0.0, -size, -size, 0 );

    root->setMatrix( matrix );
    return root;
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::Horizon >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto subControl = Q::Horizon;
    const auto r1 = innerRadius( sensor );
    const auto cX = center( sensor ).x();
    const auto cY = center( sensor ).y();
    const auto rX = sensor->subControlRotation( subControl ).x();
    const auto rZ = 0.0;
    const auto dY = 2 * sensor->angle().y();
    const auto p = qBound( 0.0, 0.5 + ( -rX / dY ), 1.0 );

    const auto shape = QskBoxShapeMetrics{ r1 };
    const auto bmetrics = sensor->boxBorderMetricsHint( subControl );
    const auto bcolors = sensor->boxBorderColorsHint( subControl );

    auto gradient = sensor->gradientHint( Q::Horizon );
    gradient.setDirection( QskGradient::Linear );
    gradient.setLinearDirection( Qt::Vertical );
    gradient.setStops( { { 0.0, gradient.startColor() }, { p, gradient.startColor() },
        { p, gradient.endColor() }, { 1.0, gradient.endColor() } } );

    auto* const tNode = ensure< QSGTransformNode, par< 1, QskBoxNode > >::node( node );
    auto* const boxNode = static_cast< QskBoxNode* >( tNode->firstChild() );
    updateBoxNode( sensor, boxNode, { 0, 0, 2 * r1, 2 * r1 }, shape, bmetrics, bcolors, gradient );

    const auto matrix = matrix_deg( 0, 0, 0, cX, cY, 0 ) * matrix_deg( 0, 0, rZ, 0, 0, 0 ) *
                        matrix_deg( 0, 0, 0, -r1, -r1, 0 );

    tNode->setMatrix( matrix );
    return tNode;
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::TickmarksX >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto subControl = Q::TickmarksX;
    const auto color = sensor->color( subControl );
    const auto scale = sensor->strutSizeHint( subControl );

    const auto cX = center( sensor ).x();
    const auto cY = center( sensor ).y();

    const auto rotation = sensor->subControlRotation( subControl );

    const auto r1 = innerRadius( sensor );
    const auto r2 = outerRadius( sensor );
    const auto r3 = r1 * scale.height();

    const auto sX = r1 / sensor->angle().x();
    const auto sY = r1 / sensor->angle().y();

    const auto tX = static_cast< float >( rotation.y() * sX );
    const auto tY = static_cast< float >( rotation.x() * sY );

    auto* const clipping =
        ensure< PolygonClipNode, par< 1, QSGTransformNode, par< 1, LinearTickmarksNode > > >::node(
            node );
    auto* const transform = static_cast< QSGTransformNode* >( clipping->firstChild() );
    auto* const tickmarks = static_cast< LinearTickmarksNode* >( transform->firstChild() );

    auto size = qvariant_cast< QVector3D >( sensor->effectiveSkinHint( subControl ) ) * r3;

    clipping->setGeometryProperties( r1, cX, cY );

    tickmarks->setMaterialProperties( color );
    tickmarks->setGeometryProperties(
        sensor->tickmarks( Qt::XAxis ), size, { sX, 0.0f }, { tX, tY } );

    const auto matrix = matrix_deg( 0, 0, rotation.z(), cX, cY, 0 );
    transform->setMatrix( matrix );
    return clipping;
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::TickmarksY >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto subControl = Q::TickmarksY;
    const State< QskAspect::Subcontrol > state( sensor, subControl );

    const auto color = sensor->color( subControl );
    const auto scale = sensor->strutSizeHint( subControl );

    const auto r3 = state.r1 * scale.width();

    const auto rotation = sensor->subControlRotation( subControl );

    auto* const cNode =
        ensure< PolygonClipNode, par< 1, QSGTransformNode, par< 1, LinearTickmarksNode > > >::node(
            node );
    auto* const tNode = static_cast< QSGTransformNode* >( cNode->firstChild() );
    auto* const lNode = static_cast< LinearTickmarksNode* >( tNode->firstChild() );

    auto size = qvariant_cast< QVector3D >( sensor->effectiveSkinHint( subControl ) ) * r3;

    cNode->setGeometryProperties( state.r1, state.cX, state.cY );

    const auto sY = static_cast< float >( state.r1 / sensor->angle().y() );
    lNode->setMaterialProperties( color );
    lNode->setGeometryProperties( sensor->tickmarks( Qt::YAxis ), size, { 0.0f, sY },
        { ( float ) state.tX, ( float ) state.tY } );

    const auto matrix = matrix_deg( 0, 0, state.rZ, state.cX, state.cY );
    tNode->setMatrix( matrix );
    return cNode;
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::TickmarksZ >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto subControl = Q::TickmarksZ;
    const auto color = sensor->color( subControl );
    const auto scale = sensor->strutSizeHint( subControl );

    const auto r1 = innerRadius( sensor );
    const auto r2 = outerRadius( sensor );
    const auto r3 =
        qvariant_cast< QVector3D >( sensor->effectiveSkinHint( subControl ) ) * ( r2 - r1 ) +
        QVector3D{ r1, r1, r1 };

    auto* const transform = ensure< QSGTransformNode, par< 1, RadialTickmarksNode > >::node( node );
    auto* const tickmarksNode = static_cast< RadialTickmarksNode* >( transform->firstChild() );
    tickmarksNode->setMaterialProperties( color );
    tickmarksNode->setGeometryProperties( sensor->tickmarks( Qt::ZAxis ), r1, r3 );

    const auto rZ = sensor->subControlRotation( subControl ).z();
    const auto tX = center( sensor ).x();
    const auto tY = center( sensor ).y();

    const auto matrix = matrix_deg( 0.0, 0.0, rZ, tX, tY );
    transform->setMatrix( matrix );
    return transform;
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::TickmarksXLabels >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto subControl = Q::TickmarksXLabels;

    const auto r1 = innerRadius( sensor );
    const auto r3 = static_cast< float >( r1 * sensor->strutSizeHint( Q::TickmarksX ).height() );
    const auto sX = r1 / sensor->angle().x();
    const auto sY = r1 / sensor->angle().y();

    const auto rotation = sensor->subControlRotation( subControl );

    const auto rZ = rotation.z();
    const auto cX = center( sensor ).x();
    const auto cY = center( sensor ).y();
    const auto tX = rotation.y() * sX;
    const auto tY = r3 + rotation.x() * sY;

    auto* const cNode = ensure< PolygonClipNode,
        par< 1, QSGTransformNode, par< 1, LinearTickmarksLabelsNode > > >::node( node );
    auto* const tNode = static_cast< QSGTransformNode* >( cNode->firstChild() );
    auto* const lNode = static_cast< LinearTickmarksLabelsNode* >( tNode->firstChild() );
    tNode->setMatrix( matrix_deg( 0.0, 0.0, rZ, cX, cY ) );
    cNode->setGeometryProperties( r1, center( sensor ).x(), center( sensor ).y() );
    lNode->update(
        sensor, subControl, sensor->tickmarkLabels( Qt::XAxis ), { sX, 0.0 }, { tX, tY } );
    return cNode;
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::TickmarksYLabels >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto subControl = Q::TickmarksYLabels;
    const auto r1 = innerRadius( sensor );
    const auto r3 = static_cast< float >( r1 * sensor->strutSizeHint( Q::TickmarksY ).width() );
    const auto cX = static_cast< float >( center( sensor ).x() );
    const auto cY = static_cast< float >( center( sensor ).y() );

    const auto scale = sensor->strutSizeHint( subControl );
    const auto angles = sensor->angle();
    const auto rotation = sensor->subControlRotation( subControl );
    const auto sX = r1 / angles.x();
    const auto sY = r1 / angles.y();
    const auto tX = static_cast< float >( rotation.y() * sX );
    const auto tY = static_cast< float >( rotation.x() * sY );

    auto* const cNode = ensure< PolygonClipNode,
        par< 1, QSGTransformNode, par< 1, LinearTickmarksLabelsNode > > >::node( node );
    auto* const tNode = static_cast< QSGTransformNode* >( cNode->firstChild() );
    auto* const lNode = static_cast< LinearTickmarksLabelsNode* >( tNode->firstChild() );
    cNode->setGeometryProperties( r1, cX, cY );
    tNode->setMatrix( matrix_deg( 0.0, 0.0, rotation.z(), cX, cY ) );
    lNode->update( sensor, subControl, sensor->tickmarkLabels( Qt::YAxis ),
        { 0.0, r1 / sensor->angle().y() }, { r3 + tX, tY } );
    return cNode;
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::TickmarksZLabels >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto subControl = Q::TickmarksZLabels;
    auto* const tNode =
        ensure< QSGTransformNode, par< 1, RadialTickmarksLabelsNode > >::node( node );
    auto* const lNode = static_cast< RadialTickmarksLabelsNode* >( tNode->firstChild() );
    const auto r1 = innerRadius( sensor );
    const auto r3 = static_cast< float >( r1 * sensor->strutSizeHint( subControl ).width() );
    const auto cX = static_cast< float >( center( sensor ).x() );
    const auto cY = static_cast< float >( center( sensor ).y() );
    const auto rZ = sensor->subControlRotation( subControl ).z();
    lNode->update( sensor, subControl, sensor->tickmarkLabels( Qt::ZAxis ), { r3, r3 } );
    tNode->setMatrix( matrix_deg( 0.0, 0.0, rZ, cX, cY ) );
    return tNode;
}

template<>
QSGNode* QskLevelingSensorSkinlet::updateSubNode< R::HorizonClip >(
    const QskLevelingSensor* const sensor, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto cX = center( sensor ).x();
    const auto cY = center( sensor ).y();
    const auto r1 = innerRadius( sensor );

    auto* const clipNode = ensure< PolygonClipNode >::node( node );
    clipNode->setGeometryProperties( r1, cX, cY );
    return clipNode;
}

QSGNode* QskLevelingSensorSkinlet::updateSubNode(
    const QskSkinnable* const skinnable, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto* const sensor = static_cast< const Q* >( skinnable );

    const auto subControl = [ nodeRole, sensor ]() {
        switch ( static_cast< R >( nodeRole ) )
        {
            case OuterDisk:
                return Q::OuterDisk;
            case Horizon:
                return Q::Horizon;
            case HorizonClip:
                return Q::Horizon;
            case TickmarksX:
                return Q::TickmarksX;
            case TickmarksXLabels:
                return Q::TickmarksXLabels;
            case TickmarksY:
                return Q::TickmarksY;
            case TickmarksYLabels:
                return Q::TickmarksYLabels;
            case TickmarksZ:
                return Q::TickmarksZ;
            case TickmarksZLabels:
                return Q::TickmarksZLabels;
            default:
                return QskAspect::NoSubcontrol;
        }
    }();

    if ( qvariant_cast< bool >( sensor->effectiveSkinHint( subControl | QskAspect::Option ) ) )
    {
        return nullptr;
    }

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
