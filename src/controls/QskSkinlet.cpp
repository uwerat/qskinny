/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSkinlet.h"

#include "QskArcNode.h"
#include "QskAspect.h"
#include "QskArcMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxNode.h"
#include "QskBoxClipNode.h"
#include "QskBoxRectangleNode.h"
#include "QskBoxShapeMetrics.h"
#include "QskBoxHints.h"
#include "QskColorFilter.h"
#include "QskControl.h"
#include "QskFunctions.h"
#include "QskGradient.h"
#include "QskGraphicNode.h"
#include "QskGraphic.h"
#include "QskLinesNode.h"
#include "QskRectangleNode.h"
#include "QskSGNode.h"
#include "QskStippleMetrics.h"
#include "QskTextColors.h"
#include "QskTextNode.h"
#include "QskTextOptions.h"
#include "QskSkinStateChanger.h"
#include "QskTextureRenderer.h"
#include "QskSetup.h"

#include <qquickwindow.h>
#include <qsgsimplerectnode.h>

static inline QRectF qskSceneAlignedRect( const QQuickItem* item, const QRectF& rect )
{
    const auto transform = item->itemTransform( nullptr, nullptr );
    if ( transform.type() > QTransform::TxTranslate )
        return rect;

    /*
       Aligning rect according to scene coordinates, so that
       we don't run into rounding issues downstream, where values
       will be floored/ceiled ending up with a slightly different
       aspect ratio.
     */

    const auto ratio = item->window()->devicePixelRatio();

    const auto pos = transform.map( rect.topLeft() ) * ratio;
    const auto size = rect.size() * ratio;

    const qreal x = qRound( pos.x() ) / ratio;
    const qreal y = qRound( pos.y() ) / ratio;
    const qreal w = qRound( size.width() ) / ratio;
    const qreal h = qRound( size.height() ) / ratio;

    return QRectF( item->mapFromScene( QPointF( x, y ) ), QSizeF( w, h ) );
}

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

static inline QSGNode* qskUpdateTextNode( const QskSkinnable* skinnable,
    QSGNode* node, const QRectF& rect, Qt::Alignment alignment,
    const QString& text, const QFont& font, const QskTextOptions& textOptions,
    const QskTextColors& textColors, Qsk::TextStyle textStyle )
{
    if ( text.isEmpty() || rect.isEmpty() )
        return nullptr;

    auto textNode = static_cast< QskTextNode* >( node );
    if ( textNode == nullptr )
        textNode = new QskTextNode();

    auto effectiveFont = font;
    switch ( textOptions.fontSizeMode() )
    {
        case QskTextOptions::FixedSize:
            break;

        case QskTextOptions::HorizontalFit:
            Q_UNIMPLEMENTED();
            break;

        case QskTextOptions::VerticalFit:
            effectiveFont.setPixelSize( static_cast< int >( rect.height() * 0.5 ) );
            break;

        case QskTextOptions::Fit:
            Q_UNIMPLEMENTED();
            break;
    }

    textNode->setTextData( skinnable->owningItem(),
        text, rect, effectiveFont, textOptions, textColors, alignment, textStyle );

    return textNode;
}

static inline QSGNode* qskUpdateGraphicNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QskGraphic& graphic, const QskColorFilter& colorFilter,
    const QRectF& rect, Qt::Orientations mirrored )
{
    if ( rect.isEmpty() )
        return nullptr;

    const auto item = skinnable->owningItem();
    if ( item == nullptr )
        return nullptr;

    auto graphicNode = static_cast< QskGraphicNode* >( node );
    if ( graphicNode == nullptr )
        graphicNode = new QskGraphicNode();

    const auto flag = QskQuickItem::PreferRasterForTextures;

    bool useRaster = qskSetup->testItemUpdateFlag( flag );
    if ( auto qItem = qobject_cast< const QskQuickItem* >( item ) )
        useRaster = qItem->testUpdateFlag( flag );

    graphicNode->setRenderHint( useRaster ? QskPaintedNode::Raster : QskPaintedNode::OpenGL );

    graphicNode->setMirrored( mirrored );

    const auto r = qskSceneAlignedRect( item, rect );
    graphicNode->setGraphic( item->window(), graphic, colorFilter, r );

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
    qreal borderWidth, const QColor borderColor, const QskGradient& gradient )
{
    if ( arcMetrics.isNull() )
        return false;

    if ( borderWidth > 0.0 && borderColor.isValid() && borderColor.alpha() > 0 )
        return true;

    return gradient.isVisible();
}

static inline bool qskIsLineVisible( const QColor& lineColor, qreal lineWidth )
{
    return ( lineWidth > 0.0 ) && lineColor.isValid() && ( lineColor.alpha() > 0 );
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

static inline QSGNode* qskUpdateBoxNode(
    const QskSkinnable*, QSGNode* node, const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient,
    const QskShadowMetrics& shadowMetrics, const QColor& shadowColor )
{
    if ( rect.isEmpty() )
        return nullptr;

    const auto size = rect.size();

    const auto absoluteMetrics = borderMetrics.toAbsolute( size );

    if ( qskIsBoxVisible( absoluteMetrics, borderColors, gradient ) )
    {
        const auto absoluteShape = shape.toAbsolute( size );
        const auto absoluteShadowMetrics = shadowMetrics.toAbsolute( size );

        auto boxNode = QskSGNode::ensureNode< QskBoxNode >( node );
        boxNode->updateNode( rect, absoluteShape, absoluteMetrics,
            borderColors, gradient, absoluteShadowMetrics, shadowColor );

        return boxNode;
    }

    return nullptr;
}

static inline QSGNode* qskUpdateArcNode(
    const QskSkinnable*, QSGNode* node, const QRectF& rect,
    qreal borderWidth, const QColor borderColor,
    const QskGradient& gradient, const QskArcMetrics& metrics )
{
    if ( rect.isEmpty() )
        return nullptr;

    if ( !qskIsArcVisible( metrics, borderWidth, borderColor, gradient ) )
        return nullptr;

    auto arcNode = QskSGNode::ensureNode< QskArcNode >( node );
    arcNode->setArcData( rect, metrics, borderWidth, borderColor,  gradient );

    return arcNode;
}

static inline QSGNode* qskUpdateLineNode(
    const QskSkinnable*, QSGNode* node, const QColor& lineColor,
    qreal lineWidth, QskStippleMetrics& lineStipple, const QLineF& line )
{
    if ( line.isNull() )
        return nullptr;

    if ( !qskIsLineVisible( lineColor, lineWidth ) )
        return nullptr;

    auto linesNode = QskSGNode::ensureNode< QskLinesNode >( node );
    linesNode->updateLine( lineColor, lineWidth, lineStipple,
        QTransform(), line.p1(), line.p2() );

    return linesNode;
}

static inline QSGNode* qskUpdateLinesNode(
    const QskSkinnable*, QSGNode* node, const QColor& lineColor,
    qreal lineWidth, QskStippleMetrics& lineStipple, const QVector< QLineF >& lines )
{
    if ( lines.isEmpty() )
        return nullptr;

    if ( !qskIsLineVisible( lineColor, lineWidth ) )
        return nullptr;

    auto linesNode = QskSGNode::ensureNode< QskLinesNode >( node );
    linesNode->updateLines( lineColor, lineWidth, lineStipple,
        QTransform(), lines );

    return linesNode;
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
        newNode = updateBackgroundNode( control, oldNode );

        replaceChildNode( BackgroundRole, parentNode, oldNode, newNode );

        // debug

        oldNode = findChildNode( parentNode, DebugRole );

        newNode = nullptr;
        if ( control->testUpdateFlag( QskQuickItem::DebugForceBackground ) )
            newNode = updateDebugNode( control, oldNode );

        replaceChildNode( DebugRole, parentNode, oldNode, newNode );
    }

    for ( const auto nodeRole : std::as_const( m_data->nodeRoles ) )
    {
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

    auto rectNode = QskSGNode::ensureNode< QskRectangleNode >( node );
    rectNode->updateNode( rect, gradient );

    return rectNode;
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

    const auto borderMetrics = skinnable->boxBorderMetricsHint( subControl );
    const auto borderColors = skinnable->boxBorderColorsHint( subControl );
    const auto shape = skinnable->boxShapeHint( subControl );
    const auto shadowMetrics = skinnable->shadowMetricsHint( subControl );
    const auto shadowColor = skinnable->shadowColorHint( subControl );

    return qskUpdateBoxNode( skinnable, node,
        boxRect, shape, borderMetrics, borderColors, fillGradient,
        shadowMetrics, shadowColor );
}

QSGNode* QskSkinlet::updateBoxNode(
    const QskSkinnable* skinnable, QSGNode* node, const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics,
    const QskBoxBorderColors& borderColors, const QskGradient& fillGradient )
{
    return qskUpdateBoxNode( skinnable, node,
        rect, shape, borderMetrics, borderColors, fillGradient,
        QskShadowMetrics(), QColor() );
}

QSGNode* QskSkinlet::updateBoxNode( const QskSkinnable* skinnable,
    QSGNode* node, const QRectF& rect, const QskBoxHints& hints )
{
    return qskUpdateBoxNode( skinnable, node, rect,
        hints.shape, hints.borderMetrics, hints.borderColors, hints.gradient,
        hints.shadowMetrics, hints.shadowColor );
}

QSGNode* QskSkinlet::updateInterpolatedBoxNode(
    const QskSkinnable* skinnable, QSGNode* node, const QRectF& rect,
    QskAspect aspect1, QskAspect aspect2, qreal ratio )
{
    QskBoxHints boxHints;
    QRectF r;

    ratio = qBound( 0.0, ratio, 1.0 );

    if ( qFuzzyIsNull( ratio ) )
    {
        const auto margins = skinnable->marginHint( aspect1 );
        r = rect.marginsRemoved( margins );

        boxHints = skinnable->boxHints( aspect1 ).toAbsolute( r.size() );
    }
    else if ( qFuzzyCompare( ratio, 1.0 ) )
    {
        const auto margins = skinnable->marginHint( aspect2 );
        r = rect.marginsRemoved( margins );

        boxHints = skinnable->boxHints( aspect2 ).toAbsolute( r.size() );
    }
    else
    {
        QskMargins margins = skinnable->marginHint( aspect1 );
        margins = margins.interpolated( skinnable->marginHint( aspect2 ), ratio );

        r = rect.marginsRemoved( margins );

        const auto boxHints1 = skinnable->boxHints( aspect1 ).toAbsolute( r.size() );
        const auto boxHints2 = skinnable->boxHints( aspect2 ).toAbsolute( r.size() );

        boxHints = boxHints1.interpolated( boxHints2, ratio );
    }

    return QskSkinlet::updateBoxNode( skinnable, node, r, boxHints );
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
    const auto metrics = skinnable->arcMetricsHint( subControl );
    const auto r = rect.marginsRemoved( skinnable->marginHint( subControl ) );

    const qreal borderWidth = skinnable->metric( subControl | QskAspect::Border );

    QColor borderColor;
    if ( borderWidth > 0.0 )
        borderColor = skinnable->color( subControl | QskAspect::Border );

    return qskUpdateArcNode( skinnable, node,
        r, borderWidth, borderColor, fillGradient, metrics );
}

QSGNode* QskSkinlet::updateArcNode(
    const QskSkinnable* skinnable, QSGNode* node, const QRectF& rect, 
    qreal borderWidth, const QColor& borderColor,
    const QskGradient& fillGradient, const QskArcMetrics& metrics )
{
    return qskUpdateArcNode( skinnable, node, rect, 
        borderWidth, borderColor, fillGradient, metrics );
}

QSGNode* QskSkinlet::updateArcNode( const QskSkinnable* skinnable,
    QSGNode* node, qreal startAngle, qreal spanAngle,
    QskAspect::Subcontrol subControl ) const
{
    const auto rect = qskSubControlRect( this, skinnable, subControl );

    return updateArcNode( skinnable, node,
        rect, startAngle, spanAngle, subControl );
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

    const qreal borderWidth = skinnable->metric( subControl | QskAspect::Border );

    QColor borderColor;
    if ( borderWidth > 0.0 )
        borderColor = skinnable->color( subControl | QskAspect::Border );

    const auto r = rect.marginsRemoved( skinnable->marginHint( subControl ) );
    return updateArcNode( skinnable, node, r,
        borderWidth, borderColor, fillGradient, arcMetrics );
}

QSGNode* QskSkinlet::updateLineNode( const QskSkinnable* skinnable,
     QSGNode* node, const QLineF& line, QskAspect::Subcontrol subControl )
{
    auto lineStipple = skinnable->stippleMetricsHint( subControl );
    if ( !lineStipple.isValid() )
        lineStipple = Qt::SolidLine;

    const auto lineWidth = skinnable->metric( subControl | QskAspect::Size );
    const auto lineColor = skinnable->color( subControl );

    return qskUpdateLineNode( skinnable, node,
        lineColor, lineWidth, lineStipple, line );
}

QSGNode* QskSkinlet::updateLinesNode( const QskSkinnable* skinnable,
    QSGNode* node, const QVector< QLineF >& lines, QskAspect::Subcontrol subControl )
{
    auto lineStipple = skinnable->stippleMetricsHint( subControl );
    if ( !lineStipple.isValid() )
        lineStipple = Qt::SolidLine;

    const auto lineWidth = skinnable->metric( subControl | QskAspect::Size );
    const auto lineColor = skinnable->color( subControl );

    return qskUpdateLinesNode( skinnable, node,
        lineColor, lineWidth, lineStipple, lines );
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
    auto clipNode = QskSGNode::ensureNode< QskBoxClipNode >( node );

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

QSGNode* QskSkinlet::updateTextNode( const QskSkinnable* skinnable,
    QSGNode* node, const QRectF& rect, Qt::Alignment alignment,
    const QString& text, const QFont& font, const QskTextOptions& textOptions,
    const QskTextColors& textColors, Qsk::TextStyle textStyle )
{
    return qskUpdateTextNode( skinnable, node, rect, alignment,
        text, font, textOptions, textColors, textStyle );
}

QSGNode* QskSkinlet::updateTextNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QRectF& rect, Qt::Alignment alignment,
    const QString& text, QskAspect::Subcontrol subControl )
{
    if ( text.isEmpty() || rect.isEmpty() )
        return nullptr;

    const auto textColors = qskTextColors( skinnable, subControl );
    const auto textOptions = skinnable->textOptionsHint( subControl );

    auto textStyle = Qsk::Normal;
    if ( textColors.styleColor.alpha() == 0 )
    {
        textStyle = skinnable->flagHint< Qsk::TextStyle >(
            subControl | QskAspect::Style, Qsk::Normal );
    }

    const auto font = skinnable->effectiveFont( subControl );

    return qskUpdateTextNode( skinnable, node, rect, alignment,
        text, font, textOptions, textColors, textStyle );
}

QSGNode* QskSkinlet::updateTextNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QString& text, QskAspect::Subcontrol subControl ) const
{
    const auto rect = qskSubControlRect( this, skinnable, subControl );
    const auto alignment = skinnable->alignmentHint( subControl, Qt::AlignLeft );

    return updateTextNode( skinnable, node,
        rect, alignment, text, subControl );
}

QSGNode* QskSkinlet::updateSymbolNode(
    const QskSkinnable* skinnable, QSGNode* node,
    QskAspect::Subcontrol subControl ) const
{
    return updateGraphicNode( skinnable, node,
        skinnable->symbolHint( subControl ), subControl );
}

QSGNode* QskSkinlet::updateGraphicNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QskGraphic& graphic, QskAspect::Subcontrol subControl,
    Qt::Orientations mirrored ) const
{
    const auto rect = qskSubControlRect( this, skinnable, subControl );
    const auto alignment = skinnable->alignmentHint( subControl, Qt::AlignCenter );
    const auto colorFilter = skinnable->effectiveGraphicFilter( subControl );

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

    const auto r = qskAlignedRectF( rect, size, alignment );
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

int QskSkinlet::sampleIndexAt( const QskSkinnable* skinnable,
    const QRectF& rect, QskAspect::Subcontrol subControl, const QPointF& pos ) const
{
    // slow default implementation to be overloaded when having many cells

    const auto count = sampleCount( skinnable, subControl );

    for ( int i = 0; i < count; i++ )
    {
        const auto r = sampleRect( skinnable, rect, subControl, i );
        if ( r.contains( pos ) )
            return i;
    }

    return -1;
}

QSGNode* QskSkinlet::updateSeriesNode( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, QSGNode* rootNode ) const
{
    auto node = rootNode ? rootNode->firstChild() : nullptr;
    QSGNode* lastNode = nullptr;

    const auto count = sampleCount( skinnable, subControl );

    for( int i = 0; i < count; i++ )
    {
        QSGNode* newNode = nullptr;

        {
            const auto newStates = sampleStates( skinnable, subControl, i );

            QskSkinStateChanger stateChanger( skinnable );
            stateChanger.setStates( newStates, i );

            newNode = updateSampleNode( skinnable, subControl, i, node );
        }

        if ( newNode )
        {
            if ( newNode == node )
            {
                node = node->nextSibling();
            }
            else
            {
                if ( rootNode == nullptr )
                    rootNode = new QSGNode();

                if ( node )
                    rootNode->insertChildNodeBefore( newNode, node );
                else
                    rootNode->appendChildNode( newNode );
            }

            lastNode = newNode;
        }
    }

    QskSGNode::removeAllChildNodesAfter( rootNode, lastNode );

    return rootNode;
}

QSGNode* QskSkinlet::updateSampleNode( const QskSkinnable*,
    QskAspect::Subcontrol, int index, QSGNode* ) const
{
    Q_UNUSED( index )
    return nullptr;
}

QskAspect::States QskSkinlet::sampleStates(
    const QskSkinnable* skinnable, QskAspect::Subcontrol, int index ) const
{
    Q_UNUSED( index )
    return skinnable->skinStates();
}

QVariant QskSkinlet::sampleAt( const QskSkinnable*,
    QskAspect::Subcontrol, int index ) const
{
    Q_UNUSED( index )
    return QVariant();
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
