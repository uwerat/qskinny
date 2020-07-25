/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkinlet.h"

#include "QskAspect.h"
#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxClipNode.h"
#include "QskBoxNode.h"
#include "QskBoxShapeMetrics.h"
#include "QskControl.h"
#include "QskFunctions.h"
#include "QskGradient.h"
#include "QskGraphicNode.h"
#include "QskGraphicTextureFactory.h"
#include "QskTextColors.h"
#include "QskTextNode.h"
#include "QskTextOptions.h"

#include <qguiapplication.h>
#include <qquickwindow.h>
#include <qsgsimplerectnode.h>

static const int qskBackgroundRole = 254;
static const int qskDebugRole = 253;

static inline QSGNode::Flags qskNodeFlags( quint8 nodeRole )
{
    return static_cast< QSGNode::Flags >( ( nodeRole + 1 ) << 8 );
}

static inline quint8 qskRole( const QSGNode* node )
{
    return static_cast< quint8 >( ( ( node->flags() & 0x0ffff ) >> 8 ) - 1 );
}

static inline void qskSetRole( quint8 nodeRole, QSGNode* node )
{
    const QSGNode::Flags flags = qskNodeFlags( nodeRole );
    node->setFlags( node->flags() | flags );
}

static inline QSGNode* qskFindNodeByFlag( QSGNode* parent, int nodeRole )
{
    auto node = parent->firstChild();
    while ( node )
    {
        if ( qskRole( node ) == nodeRole )
            return node;

        node = node->nextSibling();
    }

    return nullptr;
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

static inline QSGNode* qskUpdateGraphicNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QskGraphic& graphic, const QskColorFilter& colorFilter,
    const QRectF& rect )
{
    if ( rect.isEmpty() )
        return nullptr;

    auto mode = QskTextureRenderer::OpenGL;

    auto graphicNode = static_cast< QskGraphicNode* >( node );
    if ( graphicNode == nullptr )
        graphicNode = new QskGraphicNode();

    QRectF r = rect;

    if ( const auto control = skinnable->owningControl() )
    {
        if ( control->testControlFlag( QskControl::PreferRasterForTextures ) )
            mode = QskTextureRenderer::Raster;

        /*
           Aligning the rect according to scene coordinates, so that
           we don't run into rounding issues downstream, where values
           will be floored/ceiled ending up with a slightly different
           aspect ratio.
         */
        const QRectF sceneRect(
            control->mapToScene( r.topLeft() ),
            r.size() * QskTextureRenderer::devicePixelRatio() );

        r = qskInnerRect( sceneRect );
        r.moveTopLeft( control->mapFromScene( r.topLeft() ) );
    }

    graphicNode->setGraphic( graphic, colorFilter, mode, r );

    return graphicNode;
}

static inline bool qskIsBoxVisible( const QskBoxBorderMetrics& borderMetrics,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient )
{
    if ( gradient.isVisible() )
        return true;

    return !borderMetrics.isNull() && borderColors.isVisible();
}

static inline QskTextColors qskTextColors(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl )
{
    using namespace QskAspect;

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

    c.styleColor = skinnable->color( subControl | StyleColor );
    c.linkColor = skinnable->color( subControl | LinkColor );

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
    QSGNode* oldNode;
    QSGNode* newNode;

    if ( const auto control = skinnable->controlCast() )
    {
        // background

        oldNode = qskFindNodeByFlag( parentNode, qskBackgroundRole );

        newNode = nullptr;
        if ( control->autoFillBackground() )
            newNode = updateBackgroundNode( control, oldNode );

        insertRemoveNodes( parentNode, oldNode, newNode, qskBackgroundRole );

        // debug

        oldNode = qskFindNodeByFlag( parentNode, qskDebugRole );

        newNode = nullptr;
        if ( control->testControlFlag( QskControl::DebugForceBackground ) )
            newNode = updateDebugNode( control, oldNode );

        insertRemoveNodes( parentNode, oldNode, newNode, qskDebugRole );
    }

    for ( int i = 0; i < m_data->nodeRoles.size(); i++ )
    {
        const auto nodeRole = m_data->nodeRoles[ i ];

        Q_ASSERT( nodeRole <= 245 ); // reserving 10 roles

        oldNode = qskFindNodeByFlag( parentNode, nodeRole );
        newNode = updateSubNode( skinnable, nodeRole, oldNode );

        insertRemoveNodes( parentNode, oldNode, newNode, nodeRole );
    }
}

QSGNode* QskSkinlet::updateBackgroundNode(
    const QskControl* control, QSGNode* node ) const
{
    const QRectF rect = control->rect();
    if ( rect.isEmpty() )
        return nullptr;

    const QskGradient gradient = control->background();
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
    {
        return nullptr;
    }

    QSGSimpleRectNode* rectNode = static_cast< QSGSimpleRectNode* >( node );
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

    const QRectF r = control->rect();
    if ( rectNode->rect() != r )
        rectNode->setRect( r );

    return rectNode;
}

void QskSkinlet::insertRemoveNodes( QSGNode* parentNode,
    QSGNode* oldNode, QSGNode* newNode, quint8 nodeRole ) const
{
    if ( newNode && newNode->parent() != parentNode )
    {
        qskSetRole( nodeRole, newNode );

        switch ( nodeRole )
        {
            case qskBackgroundRole:
            {
                parentNode->prependChildNode( newNode );
                break;
            }
            case qskDebugRole:
            {
                QSGNode* firstNode = parentNode->firstChild();

                if ( firstNode && ( qskRole( firstNode ) == qskBackgroundRole ) )
                    parentNode->insertChildNodeAfter( newNode, firstNode );
                else
                    parentNode->prependChildNode( newNode );

                break;
            }
            default:
            {
                insertNodeSorted( newNode, parentNode );
            }
        }
    }

    if ( oldNode && oldNode != newNode )
    {
        parentNode->removeChildNode( oldNode );
        if ( oldNode->flags() & QSGNode::OwnedByParent )
            delete oldNode;
    }
}

void QskSkinlet::insertNodeSorted( QSGNode* node, QSGNode* parentNode ) const
{
    QSGNode* sibling = nullptr;

    if ( parentNode->childCount() > 0 )
    {
        const int nodePos = m_data->nodeRoles.indexOf( qskRole( node ) );

        // in most cases we are appending, so let's start at the end

        for ( QSGNode* childNode = parentNode->lastChild();
            childNode != nullptr; childNode = childNode->previousSibling() )
        {
            const auto childNodeRole = qskRole( childNode );
            if ( childNodeRole == qskBackgroundRole )
            {
                sibling = childNode;
            }
            else
            {
                /*
                   Imperformant implementation, but as the number of roles is
                   usually < 5 we don't introduce some sort of support for faster lookups
                 */

                const int index = m_data->nodeRoles.indexOf( qskRole( childNode ) );
                if ( index >= 0 && index < nodePos )
                    sibling = childNode;
            }

            if ( sibling != nullptr )
                break;
        }
    }

    if ( sibling )
        parentNode->insertChildNodeAfter( node, sibling );
    else
        parentNode->prependChildNode( node );
}

void QskSkinlet::removeTraillingNodes( QSGNode* node, QSGNode* child )
{
    if ( node && child )
    {
        while ( auto sibling = child->nextSibling() )
        {
            node->removeChildNode( sibling );
            delete sibling;
        }   
    }   
}

void QskSkinlet::setNodeRole( QSGNode* node, quint8 nodeRole )
{
    qskSetRole( nodeRole, node );
}

quint8 QskSkinlet::nodeRole( const QSGNode* node )
{
    return node ? qskRole( node ) : 0;
}

QSGNode* QskSkinlet::findNodeByRole( QSGNode* parent, quint8 nodeRole )
{
    return qskFindNodeByFlag( parent, nodeRole );
}

QSGNode* QskSkinlet::updateBoxNode( const QskSkinnable* skinnable,
    QSGNode* node, QskAspect::Subcontrol subControl ) const
{
    const QRectF rect = qskSubControlRect( this, skinnable, subControl );
    return updateBoxNode( skinnable, node, rect, subControl );
}

QSGNode* QskSkinlet::updateBoxNode( const QskSkinnable* skinnable,
    QSGNode* node, const QRectF& rect, QskAspect::Subcontrol subControl )
{
    using namespace QskAspect;

    const QMarginsF margins = skinnable->marginsHint( subControl | Margin );

    const QRectF boxRect = rect.marginsRemoved( margins );
    if ( boxRect.isEmpty() )
        return nullptr;

    auto borderMetrics = skinnable->boxBorderMetricsHint( subControl );
    borderMetrics = borderMetrics.toAbsolute( boxRect.size() );

    const auto borderColors = skinnable->boxBorderColorsHint( subControl );
    const auto fillGradient = skinnable->gradientHint( subControl );

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

QSGNode* QskSkinlet::updateBoxClipNode( const QskSkinnable* skinnable,
    QSGNode* node, QskAspect::Subcontrol subControl ) const
{
    const QRectF rect = qskSubControlRect( this, skinnable, subControl );
    return updateBoxClipNode( skinnable, node, rect, subControl );
}

QSGNode* QskSkinlet::updateBoxClipNode( const QskSkinnable* skinnable,
    QSGNode* node, const QRectF& rect, QskAspect::Subcontrol subControl )
{
    using namespace QskAspect;

    auto clipNode = static_cast< QskBoxClipNode* >( node );
    if ( clipNode == nullptr )
        clipNode = new QskBoxClipNode();

    const QMarginsF margins = skinnable->marginsHint( subControl | Margin );

    const QRectF clipRect = rect.marginsRemoved( margins );
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

    auto colors = qskTextColors( skinnable, subControl );

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
    const QRectF rect = qskSubControlRect( this, skinnable, subControl );
    const auto alignment = skinnable->flagHint< Qt::Alignment >(
        QskAspect::Alignment | subControl, Qt::AlignLeft );

    return updateTextNode( skinnable, node,
        rect, alignment, text, textOptions, subControl );
}

QSGNode* QskSkinlet::updateGraphicNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QskGraphic& graphic, QskAspect::Subcontrol subcontrol ) const
{
    const QRectF rect = qskSubControlRect( this, skinnable, subcontrol );

    const Qt::Alignment alignment = skinnable->flagHint< Qt::Alignment >(
        subcontrol | QskAspect::Alignment, Qt::AlignCenter );

    const auto colorFilter = skinnable->effectiveGraphicFilter( subcontrol );

    return updateGraphicNode( skinnable, node,
        graphic, colorFilter, rect, alignment );
}

QSGNode* QskSkinlet::updateGraphicNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QskGraphic& graphic, const QskColorFilter& colorFilter,
    const QRectF& rect, Qt::Alignment alignment )
{
    if ( graphic.isNull() )
        return nullptr;

    const QSizeF size = graphic.defaultSize().scaled(
        rect.size(), Qt::KeepAspectRatio );

    const QRectF r = qskAlignedRectF( rect, size.width(), size.height(), alignment );
    return qskUpdateGraphicNode( skinnable, node, graphic, colorFilter, r );
}

QSGNode* QskSkinlet::updateGraphicNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QskGraphic& graphic, const QskColorFilter& colorFilter,
    const QRectF& rect )
{
    if ( graphic.isNull() )
        return nullptr;

    return qskUpdateGraphicNode( skinnable, node, graphic, colorFilter, rect );
}

#include "moc_QskSkinlet.cpp"
