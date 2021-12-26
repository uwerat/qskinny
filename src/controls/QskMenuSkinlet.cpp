#include "QskMenuSkinlet.h"
#include "QskMenu.h"

#include <QskBoxNode.h>
#include <QskGraphic.h>
#include <QskColorFilter.h>
#include <QskGraphicNode.h>
#include <QskTextNode.h>
#include <QskTextOptions.h>
#include <QskSGNode.h>
#include <QskFunctions.h>
#include <QskSkinStateChanger.h>
#include <QskMargins.h>

#include <qfontmetrics.h>

namespace
{
    class StateChanger : public QskSkinStateChanger
    {
      public:
        inline StateChanger( const QskSkinnable* skinnable, bool isSelected )
            : QskSkinStateChanger( skinnable, isSelected
                    ? QskMenu::Selected : QskAspect::NoState )
        {
        }
    };
}

static qreal qskMaxTextWidth( const QskMenu* menu )
{
    const QFontMetricsF fm( menu->effectiveFont( QskMenu::Text ) );

    auto maxWidth = 0.0;

    for ( int i = 0; i < menu->count(); i++ )
    {
        const auto entry = menu->entryAt( i );
        if( !entry.text.isEmpty() )
        {
            const auto w = qskHorizontalAdvance( fm, entry.text );
            if( w > maxWidth )
                maxWidth = w;
        }
    }

    return maxWidth;
}

static qreal qskGraphicWidth( const QskMenu* menu )
{
    const auto hint = menu->strutSizeHint( QskMenu::Graphic );
    const qreal textHeight = menu->effectiveFontHeight( QskMenu::Text );

    const auto h = qMax( hint.height(), textHeight );

    qreal maxW = 0.0;
    for ( int i = 0; i < menu->count(); i++ )
    {
        const auto w = menu->graphicAt( i ).widthForHeight( h );
        if( w > maxW )
            maxW = w;
    }

    return qMax( hint.width(), maxW );
}

static qreal qskCellWidth( const QskMenu* menu )
{
    using Q = QskMenu;

    const auto spacing = menu->spacingHint( Q::Cell );
    const auto padding = menu->paddingHint( Q::Cell );

    auto w = qskGraphicWidth( menu )
        + spacing + qskMaxTextWidth( menu );

    w += padding.left() + padding.right();

    const auto minWidth = menu->strutSizeHint( Q::Cell ).width();
    return qMax( w, minWidth );
}

static qreal qskCellHeight( const QskMenu* menu )
{
    using Q = QskMenu;

    const auto graphicHeight = menu->strutSizeHint( Q::Graphic ).height();
    const auto textHeight = menu->effectiveFontHeight( Q::Text );
    const auto padding = menu->paddingHint( Q::Cell );

    qreal h = qMax( graphicHeight, textHeight );
    h += padding.top() + padding.bottom();

    const auto minHeight = menu->strutSizeHint( Q::Cell ).height();
    h = qMax( h, minHeight );

    return h;
}

static QSGNode* qskUpdateGraphicNode( const QskMenu* menu,
    const QRectF& rect, const QskGraphic& graphic, QSGNode* node )
{
    const auto alignment = menu->alignmentHint( QskMenu::Graphic, Qt::AlignCenter );
    const auto colorFilter = menu->effectiveGraphicFilter( QskMenu::Graphic );

    return QskSkinlet::updateGraphicNode(
        menu, node, graphic, colorFilter, rect, alignment );
}

static QSGNode* qskUpdateTextNode( const QskMenu* menu,
    const QRectF& rect, const QString& text, QSGNode* node )
{
    const auto alignment = menu->alignmentHint(
        QskMenu::Text, Qt::AlignVCenter | Qt::AlignLeft );

    return QskSkinlet::updateTextNode( menu, node, rect, alignment,
        text, menu->textOptions(), QskMenu::Text );
}

static QSGNode* qskUpdateBackgroundNode( const QskMenu* menu, QSGNode* rootNode )
{
    auto node = rootNode ? rootNode->firstChild() : nullptr;
    QSGNode* lastNode = nullptr;

    for( int i = 0; i < menu->count(); i++ )
    {
        QSGNode* newNode = nullptr;

        {
            const StateChanger stateChanger( menu, menu->currentIndex() == i );

            newNode = QskSkinlet::updateBoxNode(
                menu, node, menu->cellRect( i ), QskMenu::Cell );
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

static void qskUpdateItemNode(
    const QskMenu* menu, const QRectF& graphicRect, const QskGraphic& graphic,
    const QRectF& textRect, const QString& text, QSGNode* itemNode )
{
    enum { GraphicRole, TextRole };
    static QVector< quint8 > roles = { GraphicRole, TextRole };

    for ( const auto role : roles )
    {
        auto oldNode = QskSGNode::findChildNode( itemNode, role );
        QSGNode* newNode = nullptr;

        if( role == GraphicRole )
            newNode = qskUpdateGraphicNode( menu, graphicRect, graphic, oldNode );
        else
            newNode = qskUpdateTextNode( menu, textRect, text, oldNode );

        QskSGNode::replaceChildNode( roles, role, itemNode, oldNode, newNode );
    }
}

static QSGNode* qskUpdateItemsNode( const QskMenu* menu, QSGNode* rootNode )
{
    const auto spacing = menu->spacingHint( QskMenu::Cell );
    const auto graphicWidth = qskGraphicWidth( menu );

    if ( rootNode == nullptr )
        rootNode = new QSGNode();

    QSGNode* node = nullptr;

    for( int i = 0; i < menu->count(); i++ )
    {
        if ( node == nullptr )
            node = rootNode->firstChild();
        else
            node = node->nextSibling();

        if ( node == nullptr )
        {
            node = new QSGNode();
            rootNode->appendChildNode( node );
        }

        {
            const StateChanger stateChanger( menu, menu->currentIndex() == i );

            const auto cellRect = menu->cellRect( i );

            auto graphicRect = cellRect;
            graphicRect.setWidth( graphicWidth );

            auto textRect = cellRect;
            textRect.setX( graphicRect.right() + spacing );

            qskUpdateItemNode( menu, graphicRect, menu->graphicAt( i ),
                textRect, menu->entryAt( i ).text, node );
        }
    }

    QskSGNode::removeAllChildNodesAfter( rootNode, node );

    return rootNode;
}

QskMenuSkinlet::QskMenuSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    appendNodeRoles( { PanelRole } );
}

QRectF QskMenuSkinlet::subControlRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl ) const
{
    const auto menu = static_cast< const QskMenu* >( skinnable );

    if( subControl == QskMenu::Panel )
    {
        return contentsRect;
    }

    if( subControl == QskMenu::Cursor )
    {
        return menu->cellRect( menu->currentIndex() );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QRectF QskMenuSkinlet::itemRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl, int index ) const
{
    if ( subControl == QskMenu::Cell )
    {
        const auto menu = static_cast< const QskMenu* >( skinnable );

        if ( index < 0 || index >= menu->count() )
            return QRectF();

        const auto r = menu->subControlContentsRect( QskMenu::Panel );
        const auto h = qskCellHeight( menu );

        return QRectF( r.x(), r.y() + index * h, r.width(), h );
    }

    return Inherited::itemRect(
        skinnable, contentsRect, subControl, index );
}

int QskMenuSkinlet::itemIndexAt( const QskSkinnable* skinnable,
    const QRectF& rect, QskAspect::Subcontrol subControl, const QPointF& pos ) const
{
    if ( subControl == QskMenu::Cell )
    {
        const auto menu = static_cast< const QskMenu* >( skinnable );

        const auto panelRect = menu->subControlContentsRect( QskMenu::Panel );
        if ( !panelRect.contains( pos ) )
            return -1;

        /*
            A menu never has many items and we can simply iterate
            without being concerned about performance issues
         */

        const auto h = qskCellHeight( menu );

        auto r = panelRect;
        r.setHeight( h );
        
        for ( int i = 0; i < menu->count(); i++ )
        {
            if ( r.contains( pos ) )
                return i;

            r.moveTop( r.bottom() );
        }

        return -1;
    }

    return Inherited::itemIndexAt( skinnable, rect, subControl, pos );
}

QSGNode* QskMenuSkinlet::updateContentsNode(
    const QskPopup* popup, QSGNode* contentsNode ) const
{
    enum { Panel, Backgrounds, Cursor, Items };
    static QVector< quint8 > roles = { Panel, Backgrounds, Cursor, Items };

    if ( contentsNode == nullptr )
        contentsNode = new QSGNode();

    const auto menu = static_cast< const QskMenu* >( popup );

    for ( const auto role : roles )
    {
        auto oldNode = QskSGNode::findChildNode( contentsNode, role );

        QSGNode* newNode = nullptr;

        switch( role )
        {
            case Panel:
            {
                newNode = updateBoxNode( menu, oldNode, QskMenu::Panel );
                break;
            }
            case Backgrounds:
            {
                newNode = qskUpdateBackgroundNode( menu, oldNode );
                break;
            }
            case Cursor:
            {
                newNode = updateBoxNode( menu, oldNode, QskMenu::Cursor );
                break;
            }
            case Items:
            {
                newNode = qskUpdateItemsNode( menu, oldNode );
                break;
            }
        }

        QskSGNode::replaceChildNode( roles, role, contentsNode, oldNode, newNode );
    }

    return contentsNode;
}

QSizeF QskMenuSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto menu = static_cast< const QskMenu* >( skinnable );

    const qreal w = qskCellWidth( menu );
    const auto h = menu->count() * qskCellHeight( menu );

    return menu->outerBoxSize( QskMenu::Panel, QSizeF( w, h ) );
}

#include "moc_QskMenuSkinlet.cpp"
