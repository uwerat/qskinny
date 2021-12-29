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
#include <QskFunctions.h>

#include <qfontmetrics.h>
#include <qmath.h>

template< class T >
static inline QVariant qskSampleAt( const QskMenu* menu, int index )
{
    const auto item = menu->itemAt( index );

    if ( item.canConvert< T >() )
        return item;

    if ( item.canConvert< QVariantList >() )
    {
        const auto list = item.value< QVariantList >();
        for ( const auto& value : list )
        {
            if ( value.canConvert< T >() )
                return value;
        }
    }

    return QVariant();
}

template< class T >
static inline T qskValueAt( const QskMenu* menu, int index )
{
    const auto sample = qskSampleAt< T >( menu, index );
    return sample.template value< T >();
}

class QskMenuSkinlet::PrivateData
{
  public:
    class CacheGuard
    {
      public:
        CacheGuard( QskMenuSkinlet::PrivateData* data )
            : m_data( data )
        {
            m_data->enableCache( true );
        }

        ~CacheGuard()
        {
            m_data->enableCache( false );
        }
       private:
        QskMenuSkinlet::PrivateData* m_data;
    };

    void enableCache( bool on )
    {
        m_isCaching = on;
        m_cellHeight = m_cellWidth = m_graphicWidth = m_textWidth = -1.0;
    }

    inline qreal graphicWidth( const QskMenu* menu ) const
    {
        if ( m_isCaching )
        {
            if ( m_graphicWidth < 0.0 )
                m_graphicWidth = graphicWidthInternal( menu );

            return m_graphicWidth;
        }

        return graphicWidthInternal( menu );
    }

    inline qreal textWidth( const QskMenu* menu ) const
    {
        if ( m_isCaching )
        {
            if ( m_textWidth < 0.0 )
                m_textWidth = textWidthInternal( menu );

            return m_textWidth;
        }

        return textWidthInternal( menu );
    }

    inline qreal cellWidth( const QskMenu* menu ) const
    {
        if ( m_isCaching )
        {
            if ( m_cellWidth < 0.0 )
                m_cellWidth = cellWidthInternal( menu );

            return m_cellWidth;
        }

        return cellWidthInternal( menu );
    }

    inline qreal cellHeight( const QskMenu* menu ) const
    {
        if ( m_isCaching )
        {
            if ( m_cellHeight < 0.0 )
                m_cellHeight = cellHeightInternal( menu );

            return m_cellHeight;
        }

        return cellHeightInternal( menu );
    }

  private:
    qreal graphicWidthInternal( const QskMenu* menu ) const
    {
        const auto skinlet = menu->effectiveSkinlet();

        const auto hint = menu->strutSizeHint( QskMenu::Graphic );
        const qreal textHeight = menu->effectiveFontHeight( QskMenu::Text );

        const auto h = qMax( hint.height(), textHeight );

        qreal maxW = 0.0;
        for ( int i = 0; i < menu->count(); i++ )
        {
            const auto sample = skinlet->sampleAt( menu, QskMenu::Graphic, i );
            if ( sample.canConvert< QskGraphic >() )
            {
                const auto graphic = sample.value< QskGraphic >();
                if ( !graphic.isNull() )
                {
                    const auto w = graphic.widthForHeight( h );
                    if( w > maxW )
                        maxW = w;
                }
            }
        }

        return qMax( hint.width(), maxW );
    }

    qreal textWidthInternal( const QskMenu* menu ) const
    {
        const auto skinlet = menu->effectiveSkinlet();

        const QFontMetricsF fm( menu->effectiveFont( QskMenu::Text ) );

        auto maxWidth = 0.0;

        for ( int i = 0; i < menu->count(); i++ )
        {
            const auto sample = skinlet->sampleAt( menu, QskMenu::Text, i );
            if ( sample.canConvert< QString >() )
            {
                const auto text = sample.value< QString >();
                if( !text.isEmpty() )
                {
                    const auto w = qskHorizontalAdvance( fm, text );
                    if( w > maxWidth )
                        maxWidth = w;
                }
            }
        }

        return maxWidth;
    }

    qreal cellWidthInternal( const QskMenu* menu ) const
    {
        using Q = QskMenu;

        const auto spacing = menu->spacingHint( Q::Cell );
        const auto padding = menu->paddingHint( Q::Cell );

        auto w = graphicWidth( menu ) + spacing + textWidth( menu );

        w += padding.left() + padding.right();

        const auto minWidth = menu->strutSizeHint( Q::Cell ).width();
        return qMax( w, minWidth );
    }

    qreal cellHeightInternal( const QskMenu* menu ) const
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

    bool m_isCaching;

    mutable qreal m_graphicWidth = -1.0;
    mutable qreal m_textWidth = -1.0;
    mutable qreal m_cellHeight = -1.0;
    mutable qreal m_cellWidth = -1.0;
};

QskMenuSkinlet::QskMenuSkinlet( QskSkin* skin )
    : Inherited( skin )
    , m_data( new PrivateData() )
{
    appendNodeRoles( { PanelRole } );
}

QRectF QskMenuSkinlet::cursorRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect, int index ) const
{
    const auto count = sampleCount( skinnable, QskMenu::Cell );

    auto rect = sampleRect( skinnable, contentsRect,
        QskMenu::Cell, qBound( 0, index, count ) );

    if ( index < 0 )
        rect.setBottom( rect.top() );

    if ( index >= count )
        rect.setTop( rect.bottom() );

    return rect;
}

QRectF QskMenuSkinlet::subControlRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl ) const
{
    using Q = QskMenu;

    const auto menu = static_cast< const QskMenu* >( skinnable );

    if( subControl == Q::Panel )
    {
        return contentsRect;
    }

    if( subControl == Q::Cursor )
    {
        if ( menu->currentIndex() < 0 )
            return QRectF();

        const qreal pos = menu->positionHint( Q::Cursor );

        const int pos1 = qFloor( pos );
        const int pos2 = qCeil( pos );

        auto rect = cursorRect( skinnable, contentsRect, pos1 );

        if ( pos1 != pos2 )
        {
            const auto r = cursorRect( skinnable, contentsRect, pos2 );

            const qreal ratio = ( pos - pos1 ) / ( pos2 - pos1 );
            rect = qskInterpolatedRect( rect, r, ratio );
        }

        return rect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QRectF QskMenuSkinlet::sampleRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl, int index ) const
{
    using Q = QskMenu;

    const auto menu = static_cast< const QskMenu* >( skinnable );

    if ( subControl == Q::Cell )
    {
        const auto r = menu->subControlContentsRect( Q::Panel );
        const auto h = m_data->cellHeight( menu );

        return QRectF( r.x(), r.y() + index * h, r.width(), h );
    }

    if ( subControl == QskMenu::Graphic || subControl == QskMenu::Text )
    {
        const auto r = sampleRect( menu, contentsRect, Q::Cell, index );
        const auto graphicWidth = m_data->graphicWidth( menu );

        if ( subControl == QskMenu::Graphic )
        {
            auto graphicRect = r;
            graphicRect.setWidth( graphicWidth );

            return QRectF( r.x(), r.y(), graphicWidth, r.height() );
        }
        else
        {
            auto textRect = r;

            if ( graphicWidth > 0.0 )
            {
                const auto spacing = skinnable->spacingHint( Q::Cell );
                textRect.setX( r.x() + graphicWidth + spacing );
            }

            return textRect;
        }
    }

    return Inherited::sampleRect(
        skinnable, contentsRect, subControl, index );
}

int QskMenuSkinlet::sampleIndexAt(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl, const QPointF& pos ) const
{
    const PrivateData::CacheGuard guard( m_data.get() );
    return Inherited::sampleIndexAt( skinnable, contentsRect, subControl, pos );
}

int QskMenuSkinlet::sampleCount(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    using Q = QskMenu;

    if ( subControl == Q::Cell || subControl == Q::Graphic || subControl == Q::Text )
    {
        const auto menu = static_cast< const QskMenu* >( skinnable );
        return menu->count();
    }

    return Inherited::sampleCount( skinnable, subControl );
}

QskAspect::States QskMenuSkinlet::sampleStates(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl, int index ) const
{
    using Q = QskMenu;

    auto states = Inherited::sampleStates( skinnable, subControl, index );

    if ( subControl == Q::Cell || subControl == Q::Graphic || subControl == Q::Text )
    {
        const auto menu = static_cast< const QskMenu* >( skinnable );
        if ( menu->currentIndex() == index )
            states |= QskMenu::Selected;
    }

    return states;
}

QVariant QskMenuSkinlet::sampleAt( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index ) const
{
    using Q = QskMenu;

    const auto menu = static_cast< const QskMenu* >( skinnable );

    if ( subControl == Q::Graphic )
        return qskSampleAt< QskGraphic >( menu, index );

    if ( subControl == Q::Text )
        return qskSampleAt< QString >( menu, index );

    return Inherited::sampleAt( skinnable, subControl, index );
}

QSGNode* QskMenuSkinlet::updateContentsNode(
    const QskPopup* popup, QSGNode* contentsNode ) const
{
    const PrivateData::CacheGuard guard( m_data.get() );
    return updateMenuNode( popup, contentsNode );
}

QSGNode* QskMenuSkinlet::updateMenuNode(
    const QskSkinnable* skinnable, QSGNode* contentsNode ) const
{
    enum { Panel, Background, Cursor, Graphic, Text };
    static QVector< quint8 > roles = { Panel, Background, Cursor, Graphic, Text };

    if ( contentsNode == nullptr )
        contentsNode = new QSGNode();

    for ( const auto role : roles )
    {
        auto oldNode = QskSGNode::findChildNode( contentsNode, role );

        QSGNode* newNode = nullptr;

        switch( role )
        {
            case Panel:
            {
                newNode = updateBoxNode( skinnable, oldNode, QskMenu::Panel );
                break;
            }
            case Background:
            {
                newNode = updateSeriesNode( skinnable, QskMenu::Cell, oldNode );
                break;
            }
            case Cursor:
            {
                newNode = updateBoxNode( skinnable, oldNode, QskMenu::Cursor );
                break;
            }
            case Graphic:
            {
                newNode = updateSeriesNode( skinnable, QskMenu::Graphic, oldNode );
                break;
            }
            case Text:
            {
                newNode = updateSeriesNode( skinnable, QskMenu::Text, oldNode );
                break;
            }
        }

        QskSGNode::replaceChildNode( roles, role, contentsNode, oldNode, newNode );
    }

    return contentsNode;
}

QSGNode* QskMenuSkinlet::updateSampleNode( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index, QSGNode* node ) const
{
    using Q = QskMenu;

    auto menu = static_cast< const QskMenu* >( skinnable );

    const auto rect = sampleRect( menu, menu->contentsRect(), subControl, index );

    if ( subControl == Q::Cell )
    {
        return updateBoxNode( menu, node, rect, subControl );
    }

    if ( subControl == Q::Graphic )
    {
        const auto graphic = qskValueAt< QskGraphic >( menu, index );
        if ( graphic.isNull() )
            return nullptr;

        const auto alignment = menu->alignmentHint( subControl, Qt::AlignCenter );
        const auto filter = menu->effectiveGraphicFilter( subControl );

        return QskSkinlet::updateGraphicNode(
            menu, node, graphic, filter, rect, alignment );
    }

    if ( subControl == Q::Text )
    {
        const auto text = qskValueAt< QString >( menu, index );
        if ( text.isEmpty() )
            return nullptr;

        const auto alignment = menu->alignmentHint(
            subControl, Qt::AlignVCenter | Qt::AlignLeft );

        return QskSkinlet::updateTextNode( menu, node, rect, alignment,
            text, menu->textOptions(), QskMenu::Text );
    }

    return nullptr;
}


QSizeF QskMenuSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const PrivateData::CacheGuard guard( m_data.get() );

    const auto count = sampleCount( skinnable, QskMenu::Cell );

    qreal w = 0.0;
    qreal h = 0.0;

    if ( count > 0 )
    {
        const auto menu = static_cast< const QskMenu* >( skinnable );

        w = m_data->cellWidth( menu );
        h = count * m_data->cellHeight( menu );
    }

    auto hint = skinnable->outerBoxSize( QskMenu::Panel, QSizeF( w, h ) );
    hint = hint.expandedTo( skinnable->strutSizeHint( QskMenu::Panel ) );

    return hint;
}

#include "moc_QskMenuSkinlet.cpp"
