/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkinlet.h"

#include "QskArcNode.h"
#include "QskAspect.h"
#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxClipNode.h"
#include "QskBoxNode.h"
#include "QskBoxShapeMetrics.h"
#include "QskColorFilter.h"
#include "QskControl.h"
#include "QskFunctions.h"
#include "QskGradient.h"
#include "QskGraphicNode.h"
#include "QskGraphic.h"
#include "QskSGNode.h"
#include "QskTextColors.h"
#include "QskTextNode.h"
#include "QskTextOptions.h"

#include <qquickwindow.h>
#include <qsgsimplerectnode.h>

static inline QRectF qskSubControlRect( const QskSkinlet* skinlet,
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl )
{
    if ( auto control = skinnable->controlCast() )
    {
        const auto r = control->contentsRect();
        return skinlet->subControlRect( skinnable, r, subControl );
    }

    return QRectF();
}

static inline QSGNode* qskUpdateGraphicNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QskGraphic& graphic, const QskColorFilter& colorFilter,
    const QRectF& rect, Qt::Orientations mirrored )
{
    if ( rect.isEmpty() )
        return nullptr;

    const auto control = skinnable->owningControl();
    if ( control == nullptr )
        return nullptr;

    auto mode = QskTextureRenderer::OpenGL;

    auto graphicNode = static_cast< QskGraphicNode* >( node );
    if ( graphicNode == nullptr )
        graphicNode = new QskGraphicNode();

    if ( control->testUpdateFlag( QskControl::PreferRasterForTextures ) )
        mode = QskTextureRenderer::Raster;

    /*
       Aligning the rect according to scene coordinates, so that
       we don't run into rounding issues downstream, where values
       will be floored/ceiled ending up with a slightly different
       aspect ratio.
     */
    QRectF r(
        control->mapToScene( rect.topLeft() ),
        rect.size() * control->window()->effectiveDevicePixelRatio() );

    r = qskInnerRect( r );
    r.moveTopLeft( control->mapFromScene( r.topLeft() ) );

    graphicNode->setGraphic( control->window(), graphic,
        colorFilter, mode, r, mirrored );

    return graphicNode;
}

static inline bool qskIsBoxVisible( const QskBoxBorderMetrics& borderMetrics,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient )
{
    if ( gradient.isVisible() )
        return true;

    return !borderMetrics.isNull() && borderColors.isVisible();
}

static inline bool qskIsArcVisible( const QskArcMetrics& arcMetrics,
    const QskArcBorderMetrics& borderMetrics,
    const QskArcBorderColors& borderColors, const QskGradient& gradient )
{
    return ( !arcMetrics.isNull() && gradient.isVisible() )
        || ( !borderMetrics.isNull() && borderColors.isVisible() );
}

static inline QskTextColors qskTextColors(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl )
{
    /*
        Would be more efficient to have QskTextColors hints instead of
        storing the colors as seperated hints. TODO ...
     */

    QskSkinHintStatus status;

    QskTextColors c;
    c.textColor = skinnable->color( subControl, &status );
#if 1
    if ( !status.isValid() )
        c.textColor = skinnable->color( subControl | QskAspect::TextColor );
#endif

    c.styleColor = skinnable->color( subControl | QskAspect::StyleColor );
    c.linkColor = skinnable->color( subControl | QskAspect::LinkColor );

    return c;
}

class QskSkinlet::PrivateData
{
  public:
    PrivateData( QskSkin* skin )
        : skin( skin )
        , ownedBySkinnable( false )
    {
    }

    QskSkin* skin;
    QVector< quint8 > nodeRoles;

    bool ownedBySkinnable : 1;
};

QskSkinlet::QskSkinlet( QskSkin* skin )
    : m_data( new PrivateData( skin ) )
{
}

QskSkinlet::~QskSkinlet()
{
}

QskSkin* QskSkinlet::skin() const
{
    return m_data->skin;
}

void QskSkinlet::setOwnedBySkinnable( bool on )
{
    m_data->ownedBySkinnable = on;
}

bool QskSkinlet::isOwnedBySkinnable() const
{
    return m_data->ownedBySkinnable;
}

void QskSkinlet::setNodeRoles( const QVector< quint8 >& nodeRoles )
{
    m_data->nodeRoles = nodeRoles;
}

void QskSkinlet::appendNodeRoles( const QVector< quint8 >& nodeRoles )
{
    m_data->nodeRoles += nodeRoles;
}

const QVector< quint8 >& QskSkinlet::nodeRoles() const
{
    return m_data->nodeRoles;
}

void QskSkinlet::updateNode( QskSkinnable* skinnable, QSGNode* parentNode ) const
{
    using namespace QskSGNode;

    QSGNode* oldNode;
    QSGNode* newNode;

    if ( const auto control = skinnable->controlCast() )
    {
        // background

        oldNode = findChildNode( parentNode, BackgroundRole );

        newNode = nullptr;
        if ( control->autoFillBackground() )
            newNode = updateBackgroundNode( control, oldNode );

        replaceChildNode( BackgroundRole, parentNode, oldNode, newNode );

        // debug

        oldNode = findChildNode( parentNode, DebugRole );

        newNode = nullptr;
        if ( control->testUpdateFlag( QskQuickItem::DebugForceBackground ) )
            newNode = updateDebugNode( control, oldNode );

        replaceChildNode( DebugRole, parentNode, oldNode, newNode );
    }

    for ( int i = 0; i < m_data->nodeRoles.size(); i++ )
    {
        const auto nodeRole = m_data->nodeRoles[ i ];

        Q_ASSERT( nodeRole < FirstReservedRole );

        oldNode = QskSGNode::findChildNode( parentNode, nodeRole );
        newNode = updateSubNode( skinnable, nodeRole, oldNode );

        replaceChildNode( nodeRole, parentNode, oldNode, newNode );
    }
}

QSGNode* QskSkinlet::updateBackgroundNode(
    const QskControl* control, QSGNode* node ) const
{
    const auto rect = control->rect();
    if ( rect.isEmpty() )
        return nullptr;

    const auto gradient = control->background();
    if ( !gradient.isValid() )
        return nullptr;

    auto boxNode = static_cast< QskBoxNode* >( node );
    if ( boxNode == nullptr )
        boxNode = new QskBoxNode();

    boxNode->setBoxData( rect, gradient );
    return boxNode;
}

QSGNode* QskSkinlet::updateDebugNode(
    const QskControl* control, QSGNode* node ) const
{
    if ( control->size().isEmpty() )
        return nullptr;

    auto rectNode = static_cast< QSGSimpleRectNode* >( node );
    if ( rectNode == nullptr )
    {
        rectNode = new QSGSimpleRectNode();

        QColor color;
        if ( control->inherits( "QskFocusIndicator" ) )
        {
            color = QColor( Qt::gray );
            color.setAlpha( 60 );
        }
        else
        {
            static int idx = 0;
            idx = ( idx + 3 ) % 14;

            // maybe using something random based on web colors ???
            color = QColor( Qt::GlobalColor( 4 + idx ) );
            color.setAlpha( 200 );
        }

        rectNode->setColor( color );
    }

    const auto r = control->rect();
    if ( rectNode->rect() != r )
        rectNode->setRect( r );

    return rectNode;
}

void QskSkinlet::replaceChildNode( quint8 role,
    QSGNode* parentNode, QSGNode* oldNode, QSGNode* newNode ) const
{
    QskSGNode::replaceChildNode(
        m_data->nodeRoles, role, parentNode, oldNode, newNode );
}

QSGNode* QskSkinlet::updateBoxNode( const QskSkinnable* skinnable,
    QSGNode* node, QskAspect::Subcontrol subControl ) const
{
    const auto rect = qskSubControlRect( this, skinnable, subControl );
    return updateBoxNode( skinnable, node, rect, subControl );
}

QSGNode* QskSkinlet::updateBoxNode( const QskSkinnable* skinnable,
    QSGNode* node, const QRectF& rect, QskAspect::Subcontrol subControl )
{
    const auto fillGradient = skinnable->gradientHint( subControl );
    return updateBoxNode( skinnable, node, rect, fillGradient, subControl );
}

QSGNode* QskSkinlet::updateBoxNode( const QskSkinnable* skinnable,
    QSGNode* node, const QRectF& rect, const QskGradient& fillGradient,
    QskAspect::Subcontrol subControl )
{
    const auto margins = skinnable->marginHint( subControl );

    const auto boxRect = rect.marginsRemoved( margins );
    if ( boxRect.isEmpty() )
        return nullptr;

    auto borderMetrics = skinnable->boxBorderMetricsHint( subControl );
    borderMetrics = borderMetrics.toAbsolute( boxRect.size() );

    const auto borderColors = skinnable->boxBorderColorsHint( subControl );

    if ( !qskIsBoxVisible( borderMetrics, borderColors, fillGradient ) )
        return nullptr;

    auto shape = skinnable->boxShapeHint( subControl );
    shape = shape.toAbsolute( boxRect.size() );

    auto boxNode = static_cast< QskBoxNode* >( node );
    if ( boxNode == nullptr )
        boxNode = new QskBoxNode();

    boxNode->setBoxData( boxRect, shape, borderMetrics, borderColors, fillGradient );

    return boxNode;
}

QSGNode* QskSkinlet::updateArcNode( const QskSkinnable* skinnable,
    QSGNode* node, QskAspect::Subcontrol subControl ) const
{
    const auto rect = qskSubControlRect( this, skinnable, subControl );
    return updateArcNode( skinnable, node, rect, subControl );
}

QSGNode* QskSkinlet::updateArcNode( const QskSkinnable* skinnable,
    QSGNode* node, const QRectF& rect, QskAspect::Subcontrol subControl )
{
    const auto fillGradient = skinnable->gradientHint( subControl );
    return updateArcNode( skinnable, node, rect, fillGradient, subControl );
}

QSGNode* QskSkinlet::updateArcNode( const QskSkinnable* skinnable,
    QSGNode* node, const QRectF& rect, const QskGradient& fillGradient,
    QskAspect::Subcontrol subControl )
{
    auto arcMetrics = skinnable->arcMetricsHint( subControl );
    return updateArcNode( skinnable, node ,rect, fillGradient, arcMetrics,
        subControl );
}

QSGNode* QskSkinlet::updateArcNode( const QskSkinnable* skinnable,
    QSGNode* node, const QRectF& rect, const QskGradient& fillGradient,
    const QskArcMetrics& arcMetrics, QskAspect::Subcontrol subControl )
{
    const auto control = skinnable->owningControl();
    if ( control == nullptr )
        return nullptr;

    const auto margins = skinnable->marginHint( subControl );

    const auto arcRect = rect.marginsRemoved( margins );

    if ( arcRect.isEmpty() )
        return nullptr;

    auto absoluteArcMetrics = arcMetrics.toAbsolute( arcRect.size() );

    auto arcBorderMetrics = skinnable->arcBorderMetricsHint( subControl );
    arcBorderMetrics = arcBorderMetrics.toAbsolute( arcRect.size() );

    const auto arcBorderColors = skinnable->arcBorderColorsHint( subControl );

    if ( !qskIsArcVisible( arcMetrics, arcBorderMetrics, arcBorderColors,
        fillGradient ) )
        return nullptr;

    auto arcNode = static_cast< QskArcNode* >( node );

    if ( arcNode == nullptr )
        arcNode = new QskArcNode();

    arcNode->setArcData( rect, absoluteArcMetrics, arcBorderMetrics,
        arcBorderColors, fillGradient, control->window() );

    return arcNode;
}

QSGNode* QskSkinlet::updateArcNode( const QskSkinnable* skinnable,
    QSGNode* node, qreal startAngle, qreal spanAngle,
    QskAspect::Subcontrol subControl ) const
{
    const auto rect = qskSubControlRect( this, skinnable, subControl );
    return updateArcNode( skinnable, node, rect, startAngle, spanAngle,
        subControl );
}

QSGNode* QskSkinlet::updateArcNode( const QskSkinnable* skinnable,
    QSGNode* node, const QRectF& rect, qreal startAngle, qreal spanAngle,
    QskAspect::Subcontrol subControl )
{
    const auto fillGradient = skinnable->gradientHint( subControl );
    return updateArcNode( skinnable, node, rect,
        fillGradient, startAngle, spanAngle, subControl );
}

QSGNode* QskSkinlet::updateArcNode( const QskSkinnable* skinnable,
    QSGNode* node, const QRectF& rect, const QskGradient& fillGradient,
    qreal startAngle, qreal spanAngle, QskAspect::Subcontrol subControl )
{
    auto arcMetrics = skinnable->arcMetricsHint( subControl );
    arcMetrics.setStartAngle( startAngle );
    arcMetrics.setSpanAngle( spanAngle );

    return updateArcNode( skinnable, node ,rect,
        fillGradient, arcMetrics, subControl );
}

QSGNode* QskSkinlet::updateBoxClipNode( const QskSkinnable* skinnable,
    QSGNode* node, QskAspect::Subcontrol subControl ) const
{
    const auto rect = qskSubControlRect( this, skinnable, subControl );
    return updateBoxClipNode( skinnable, node, rect, subControl );
}

QSGNode* QskSkinlet::updateBoxClipNode( const QskSkinnable* skinnable,
    QSGNode* node, const QRectF& rect, QskAspect::Subcontrol subControl )
{
    auto clipNode = static_cast< QskBoxClipNode* >( node );
    if ( clipNode == nullptr )
        clipNode = new QskBoxClipNode();

    const auto margins = skinnable->marginHint( subControl );

    const auto clipRect = rect.marginsRemoved( margins );
    if ( clipRect.isEmpty() )
    {
        clipNode->setIsRectangular( true );
        clipNode->setClipRect( clipRect );
    }
    else
    {
        auto borderMetrics = skinnable->boxBorderMetricsHint( subControl );
        borderMetrics = borderMetrics.toAbsolute( clipRect.size() );

        auto shape = skinnable->boxShapeHint( subControl );
        shape = shape.toAbsolute( clipRect.size() );

        clipNode->setBox( clipRect, shape, borderMetrics );
    }

    return clipNode;
}

QSGNode* QskSkinlet::updateTextNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QRectF& rect, Qt::Alignment alignment,
    const QString& text, const QskTextOptions& textOptions,
    QskAspect::Subcontrol subControl )
{
    if ( text.isEmpty() || rect.isEmpty() )
        return nullptr;

    auto textNode = static_cast< QskTextNode* >( node );
    if ( textNode == nullptr )
        textNode = new QskTextNode();

    const auto colors = qskTextColors( skinnable, subControl );

    auto textStyle = Qsk::Normal;
    if ( colors.styleColor.alpha() == 0 )
    {
        textStyle = skinnable->flagHint< Qsk::TextStyle >(
            subControl | QskAspect::Style, Qsk::Normal );
    }

    auto font = skinnable->effectiveFont( subControl );

    switch ( textOptions.fontSizeMode() )
    {
        case QskTextOptions::FixedSize:
            break;

        case QskTextOptions::HorizontalFit:
            Q_UNIMPLEMENTED();
            break;

        case QskTextOptions::VerticalFit:
            font.setPixelSize( static_cast< int >( rect.height() * 0.5 ) );
            break;

        case QskTextOptions::Fit:
            Q_UNIMPLEMENTED();
            break;
    }

    textNode->setTextData( skinnable->owningControl(),
        text, rect, font, textOptions, colors, alignment, textStyle );

    return textNode;
}

QSGNode* QskSkinlet::updateTextNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QString& text, const QskTextOptions& textOptions,
    QskAspect::Subcontrol subControl ) const
{
    const auto rect = qskSubControlRect( this, skinnable, subControl );
    const auto alignment = skinnable->alignmentHint( subControl, Qt::AlignLeft );

    return updateTextNode( skinnable, node,
        rect, alignment, text, textOptions, subControl );
}

QSGNode* QskSkinlet::updateGraphicNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QskGraphic& graphic, QskAspect::Subcontrol subcontrol,
    Qt::Orientations mirrored ) const
{
    const auto rect = qskSubControlRect( this, skinnable, subcontrol );
    const auto alignment = skinnable->alignmentHint( subcontrol, Qt::AlignCenter );
    const auto colorFilter = skinnable->effectiveGraphicFilter( subcontrol );

    return updateGraphicNode( skinnable, node,
        graphic, colorFilter, rect, alignment, mirrored );
}

QSGNode* QskSkinlet::updateGraphicNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QskGraphic& graphic, const QskColorFilter& colorFilter,
    const QRectF& rect, Qt::Alignment alignment, Qt::Orientations mirrored )
{
    if ( graphic.isNull() )
        return nullptr;

    const auto size = graphic.defaultSize().scaled(
        rect.size(), Qt::KeepAspectRatio );

    const auto r = qskAlignedRectF( rect, size.width(), size.height(), alignment );
    return qskUpdateGraphicNode( skinnable, node, graphic, colorFilter, r, mirrored );
}

QSGNode* QskSkinlet::updateGraphicNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QskGraphic& graphic, const QskColorFilter& colorFilter,
    const QRectF& rect, Qt::Orientations mirrored )
{
    if ( graphic.isNull() )
        return nullptr;

    return qskUpdateGraphicNode( skinnable, node, graphic, colorFilter, rect, mirrored );
}

QSizeF QskSkinlet::hintWithoutConstraint(
    const QSizeF& hint, const QSizeF& constraint ) const
{
    /*
        This method is useful in situations, where a hint has been calculated
        from a constraint and we want to return the calculated part only 
     */
    QSizeF h;

    if ( constraint.width() < 0.0 )
        h.setWidth( hint.width() );

    if ( constraint.height() < 0.0 )
        h.setHeight( hint.height() );

    return h;
}

#include "moc_QskSkinlet.cpp"
