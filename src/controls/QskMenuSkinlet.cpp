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

static QSizeF qskIconSize( const QskMenu* menu )
{
    const auto hint = menu->strutSizeHint( QskMenu::Graphic );
    const qreal textHeight = QFontMetrics(
        menu->effectiveFont( QskMenu::Text ) ).height();

    const auto h = qMax( hint.height(), textHeight );

    qreal maxW = 0.0;
    for ( int i = 0; i < menu->count(); i++ )
    {
        const auto w = menu->graphicAt( i ).widthForHeight( h );
        if( w > maxW )
            maxW = w;
    }

    const auto w = qMax( hint.width(), maxW );

    return QSizeF( w, h );
}

static QSizeF qskItemSize( const QskMenu* menu )
{
    const auto spacing = menu->spacingHint( QskMenu::Cell );
    const QskMargins padding = menu->paddingHint( QskMenu::Cell );

    const auto sz = qskIconSize( menu );

    const qreal h = sz.height() + padding.height();
    const qreal w = sz.width() + spacing + qskMaxTextWidth( menu ) + padding.width();

    return QSizeF( w, h );
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

static QSGNode* qskUpdateBackgroundNode( const QskMenu*, QSGNode* )
{
    return nullptr; // TODO
}

static QRectF qskCellRect( const QskMenu* menu, int index )
{
    if ( index >= 0 )
    {
        auto r = menu->subControlRect( QskMenu::Panel );
        r = menu->innerBox( QskMenu::Panel, r );

        const auto sz = qskItemSize( menu );

        const auto y = r.y() + index * sz.height();
        return QRectF( r.x(), y, r.width(), sz.height() );
    }

    return QRectF();
}

static QSGNode* qskUpdateCursorNode( const QskMenu* menu, QSGNode* node )
{
    const auto r = qskCellRect( menu, menu->currentIndex() );
    if ( !r.isEmpty() )
    {
        const StateChanger stateChanger( menu, true );

        auto cursorNode = QskSGNode::ensureNode< QskBoxNode >( node );
        cursorNode->setBoxData( r, menu->gradientHint( QskMenu::Cell ) );

        return cursorNode;
    }

    return nullptr;
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
    const auto padding = menu->paddingHint( QskMenu::Cell );
    const auto spacing = menu->spacingHint( QskMenu::Cell );
    const auto iconSize = qskIconSize( menu );

    auto boundingRect = menu->subControlRect( QskMenu::Panel );
    boundingRect = menu->innerBox( QskMenu::Panel, boundingRect );

    auto itemSize = iconSize;
    itemSize.rwidth() += spacing + qskMaxTextWidth( menu );

#if QT_VERSION >= QT_VERSION_CHECK( 5, 14, 0 )
    itemSize = itemSize.grownBy( padding );
#else
    itemSize.rwidth() += padding.left() + padding.right();
    itemSize.rheight() += padding.top() + padding.bottom();
#endif

    itemSize = itemSize.expandedTo( menu->strutSizeHint( QskMenu::Graphic ) );

    if ( rootNode == nullptr )
        rootNode = new QSGNode();

    QSGNode* node = nullptr;

    qreal y = boundingRect.y();

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

            QRectF cellRect( boundingRect.x(), y,
                boundingRect.width(), itemSize.height() );

            cellRect = cellRect.marginsRemoved( padding );

            auto graphicRect = cellRect;
            graphicRect.setWidth( iconSize.width() );

            auto textRect = cellRect;
            textRect.setX( graphicRect.right() + spacing );

            qskUpdateItemNode( menu, graphicRect, menu->graphicAt( i ),
                textRect, menu->entryAt( i ).text, node );
        }

        y += itemSize.height();

    }

    // Remove trailing nodes
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
    if( subControl == QskMenu::Panel )
    {
        return contentsRect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
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

        QSGNode* newNode;

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
                newNode = qskUpdateCursorNode( menu, oldNode );
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
    using Q = QskMenu;

    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto menu = static_cast< const QskMenu* >( skinnable );

    const auto itemSize = qskItemSize( menu );
    const auto count = menu->count();

    const qreal h = count * itemSize.height();
    return menu->outerBoxSize( Q::Panel, QSizeF( itemSize.width(), h ) );
}

#include "moc_QskMenuSkinlet.cpp"
