/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkinlet.h"

#include "QskAspect.h"
#include "QskSetup.h"
#include "QskSkin.h"
#include "QskControl.h"
#include "QskBoxShapeMetrics.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskGradient.h"
#include "QskSkinRenderer.h"
#include "QskBoxNode.h"
#include "QskBoxClipNode.h"
#include "QskTextNode.h"
#include "QskGraphicNode.h"
#include "QskGraphicTextureFactory.h"
#include "QskFunctions.h"

#include <QSGSimpleRectNode>

#include <unordered_map>

static const int qskBackgroundRole = 254;
static const int qskDebugRole = 253;

static inline QSGNode::Flags qskNodeFlags( int nodeRole )
{
    return static_cast< QSGNode::Flags >( ( nodeRole + 1 ) << 8 );
}

static inline int qskRole( const QSGNode* node )
{
    return ( ( node->flags() & 0x0ffff ) >> 8 ) - 1;
}

static inline void qskSetRole( int nodeRole, QSGNode* node )
{
    const QSGNode::Flags flags = qskNodeFlags( nodeRole );
    node->setFlags( node->flags() | flags );
}

static inline QSGNode* qskFindNodeByFlag( QSGNode* parent, int nodeRole )
{
    auto node = parent->firstChild();
    while ( node )
    {
        if ( qskRole( node ) == nodeRole)
            return node;

        node = node->nextSibling();
    }

    return nullptr;
}

static inline QSGNode* qskUpdateGraphicNode( 
    const QskSkinnable* skinnable, QSGNode* node,
    const QskGraphic& graphic, const QskColorFilter& colorFilter,
    const QRect& rect )
{
    if ( rect.isEmpty() )
        return nullptr;

    auto mode = QskGraphicTextureFactory::OpenGL;

    const QskControl* control = skinnable->owningControl();
    if ( control && control->testControlFlag( QskControl::PreferRasterForTextures ) )
        mode = QskGraphicTextureFactory::Raster;

    auto graphicNode = static_cast< QskGraphicNode* >( node );
    if ( graphicNode == nullptr )
        graphicNode = new QskGraphicNode();

    graphicNode->setGraphic( graphic, colorFilter, mode, rect );
    return graphicNode;
}

static inline bool qskIsBoxVisible( const QskBoxBorderMetrics& borderMetrics,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient )
{
    if ( gradient.isVisible() )
        return true;

    return !borderMetrics.isNull() && borderColors.isVisible();
}

class QskSkinlet::PrivateData
{
public:
    PrivateData( QskSkin* skin ):
        skin( skin ),
        ownedBySkinnable( false )
    {
    }

    QskSkin* skin;
    QVector< quint8 > nodeRoles;

    bool ownedBySkinnable : 1;
};

QskSkinlet::QskSkinlet( QskSkin* skin ):
    m_data( new PrivateData( skin ) )
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
        const int nodeRole = m_data->nodeRoles[i];

        Q_ASSERT( nodeRole <= 245 ); // reserving 10 roles

        oldNode = qskFindNodeByFlag( parentNode, nodeRole );
        newNode = updateSubNode( skinnable, nodeRole, oldNode );

        insertRemoveNodes( parentNode, oldNode, newNode, nodeRole );
    }
}

QSGNode* QskSkinlet::updateBackgroundNode(
    const QskControl* control, QSGNode* node ) const
{
    const QRectF rect = control->boundingRect();
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

    const QRectF r = control->boundingRect();
    if ( rectNode->rect() != r )
        rectNode->setRect( r );

    return rectNode;
}

void QskSkinlet::insertRemoveNodes( QSGNode* parentNode,
    QSGNode* oldNode, QSGNode* newNode, int nodeRole ) const
{
    if ( newNode && newNode->parent() != parentNode )
    {
        qskSetRole( nodeRole, newNode );

        switch( nodeRole )
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
            const quint8 childNodeRole = qskRole( childNode );
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
    const QRectF rect = subControlRect( skinnable, subControl );
    return updateBoxNode( skinnable, node, rect, subControl );
}

QSGNode* QskSkinlet::updateBoxNode( const QskSkinnable* skinnable,
    QSGNode* node, const QRectF& rect, QskAspect::Subcontrol subControl ) const
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
    const QRectF rect = subControlRect( skinnable, subControl );
    return updateBoxClipNode( skinnable, node, rect, subControl );
}

QSGNode* QskSkinlet::updateBoxClipNode( const QskSkinnable* skinnable,
    QSGNode* node, const QRectF& rect, QskAspect::Subcontrol subControl ) const
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
    QskAspect::Subcontrol subControl ) const
{
    if ( text.isEmpty() || rect.isEmpty() )
        return nullptr;

    auto textNode = static_cast< QskTextNode* >( node );
    if ( textNode == nullptr )
        textNode = new QskTextNode();

    QskSkinRenderer::updateText( skinnable, rect, alignment,
        text, textOptions, textNode, subControl );

    return textNode;
}

QSGNode* QskSkinlet::updateTextNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QString& text, const QskTextOptions& textOptions,
    QskAspect::Subcontrol subControl ) const
{
    const QRectF rect = subControlRect( skinnable, subControl );
    const auto alignment = skinnable->flagHint< Qt::Alignment >(
        QskAspect::Alignment | subControl, Qt::AlignLeft );

    return updateTextNode( skinnable, node, rect, alignment,
        text, textOptions, subControl );
}

QSGNode* QskSkinlet::updateGraphicNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QskGraphic& graphic, QskAspect::Subcontrol subcontrol ) const
{
    const QRectF rect = subControlRect( skinnable, subcontrol );

    const Qt::Alignment alignment = skinnable->flagHint< Qt::Alignment >(
        subcontrol | QskAspect::Alignment, Qt::AlignCenter );
    
    const auto colorFilter = skinnable->effectiveGraphicFilter( subcontrol );
    
    return updateGraphicNode( skinnable, node,
        graphic, colorFilter, rect, alignment );
}

QSGNode* QskSkinlet::updateGraphicNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QskGraphic& graphic, const QskColorFilter& colorFilter,
    const QRectF& rect, Qt::Alignment alignment ) const
{
    if ( graphic.isNull() )
        return nullptr;

    const QSizeF scaledSize = graphic.defaultSize().scaled(
        rect.size(), Qt::KeepAspectRatio );

    const QRect r = qskAlignedRect( qskInnerRect( rect ),
        int( scaledSize.width() ), int( scaledSize.height() ), alignment );

    return qskUpdateGraphicNode( skinnable, node, graphic, colorFilter, r );
}

QSGNode* QskSkinlet::updateGraphicNode(
    const QskSkinnable* skinnable, QSGNode* node,
    const QskGraphic& graphic, const QskColorFilter& colorFilter,
    const QRectF& rect ) const
{   
    if ( graphic.isNull() )
        return nullptr;

    return qskUpdateGraphicNode( skinnable, node,
        graphic, colorFilter, rect.toAlignedRect() );
}   

#include "moc_QskSkinlet.cpp"
