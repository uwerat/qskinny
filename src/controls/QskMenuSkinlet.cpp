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

template< class T >
static T qskValueAt( const QskMenu* menu, int index )
{
    const auto item = menu->itemAt( index );

    if ( item.canConvert< T >() )
        return item.value< T >();

    if ( item.canConvert< QVariantList >() )
    {
        const auto list = item.value< QVariantList >();
        for ( const auto& value : list )
        {
            if ( value.canConvert< T >() )
                return value.value< T >();
        }
    }

    return T();
}

static qreal qskMaxTextWidth( const QskMenu* menu )
{
    const QFontMetricsF fm( menu->effectiveFont( QskMenu::Text ) );

    auto maxWidth = 0.0;

    for ( int i = 0; i < menu->count(); i++ )
    {
        const auto value = menu->itemAt( i );

        const auto text = qskValueAt< QString >( menu, i );
        if( !text.isEmpty() )
        {
            const auto w = qskHorizontalAdvance( fm, text );
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
        const auto graphic = qskValueAt< QskGraphic >( menu, i );
        const auto w = graphic.widthForHeight( h );
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
    using Q = QskMenu;

    const auto skinlet = menu->effectiveSkinlet();

    auto node = rootNode ? rootNode->firstChild() : nullptr;
    QSGNode* lastNode = nullptr;

    for( int i = 0; i < menu->count(); i++ )
    {
        QSGNode* newNode = nullptr;

        {
            QskSkinStateChanger stateChanger( menu );
            stateChanger.setStates(
                skinlet->subControlCellStates( menu, Q::Cell, i ) );

            newNode = QskSkinlet::updateBoxNode(
                menu, node, menu->cellRect( i ), Q::Cell );
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
    using Q = QskMenu;

    const auto skinlet = menu->effectiveSkinlet();

    const auto spacing = menu->spacingHint( Q::Cell );
    const auto graphicWidth = qskGraphicWidth( menu );
    const auto contentsRect = menu->contentsRect();

    if ( rootNode == nullptr )
        rootNode = new QSGNode();

    QSGNode* node = nullptr;

    const auto count = skinlet->subControlCellCount( menu, Q::Cell );
    for( int i = 0; i < count; i++ )
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
            QskSkinStateChanger stateChanger( menu );
            stateChanger.setStates(
                skinlet->subControlCellStates( menu, Q::Cell, i ) );

            const auto cellRect = skinlet->subControlCell(
                menu, contentsRect, Q::Cell, i );

            auto graphicRect = cellRect;
            graphicRect.setWidth( graphicWidth );

            auto textRect = cellRect;
            textRect.setX( graphicRect.right() + spacing );

            const auto graphic = qskValueAt< QskGraphic >( menu, i );
            const auto text = qskValueAt< QString >( menu, i );

            qskUpdateItemNode( menu, graphicRect, graphic,
                textRect, text, node );
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

QRectF QskMenuSkinlet::subControlCell(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl, int index ) const
{
    // QskMenu::Text, QskMenu::Graphic ???
    if ( subControl == QskMenu::Cell )
    {
        const auto menu = static_cast< const QskMenu* >( skinnable );

        if ( index < 0 || index >= menu->count() )
            return QRectF();

        const auto r = menu->subControlContentsRect( QskMenu::Panel );
        const auto h = qskCellHeight( menu );

        return QRectF( r.x(), r.y() + index * h, r.width(), h );
    }

    return Inherited::subControlCell(
        skinnable, contentsRect, subControl, index );
}

int QskMenuSkinlet::subControlCellCount(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    // QskMenu::Text, QskMenu::Graphic ???
    if ( subControl == QskMenu::Cell )
    {
        const auto menu = static_cast< const QskMenu* >( skinnable );
        return menu->count();
    }

    return Inherited::subControlCellCount( skinnable, subControl );
}

QskAspect::States QskMenuSkinlet::subControlCellStates(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl, int index ) const
{
    auto states = Inherited::subControlCellStates( skinnable, subControl, index );

    // QskMenu::Text, QskMenu::Graphic ???
    if ( subControl == QskMenu::Cell )
    {
        const auto menu = static_cast< const QskMenu* >( skinnable );
        if ( menu->currentIndex() == index )
            states |= QskMenu::Selected;
    }

    return states;
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
