/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMenuSkinlet.h"
#include "QskMenu.h"

#include "QskBoxNode.h"
#include "QskGraphic.h"
#include "QskColorFilter.h"
#include "QskTextOptions.h"
#include "QskSGNode.h"
#include "QskFunctions.h"
#include "QskMargins.h"
#include "QskFunctions.h"

#include <qfontmetrics.h>
#include <qmath.h>

template< class T >
static inline QVariant qskSampleAt( const QskMenu* menu, int index )
{
    const auto list = menu->optionAt( index );
    for ( const auto& value : list )
    {
        if ( value.canConvert< T >() )
            return value;
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
        CacheGuard( PrivateData* data )
            : m_data( data )
        {
            m_data->enableCache( true );
        }

        ~CacheGuard()
        {
            m_data->enableCache( false );
        }

      private:
        PrivateData* m_data;
    };

    void enableCache( bool on )
    {
        m_isCaching = on;
        m_segmentHeight = m_segmentWidth = m_graphicWidth = m_textWidth = -1.0;
    }

    inline int separatorsBefore( const QskMenu* menu, int index ) const
    {
        int i = 0;
        for ( ; i < menu->separatorCount(); i++ )
        {
            if ( menu->separatorPosition( i ) > index )
                break;
        }

        return i;
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

    inline qreal segmentWidth( const QskMenu* menu ) const
    {
        if ( m_isCaching )
        {
            if ( m_segmentWidth < 0.0 )
                m_segmentWidth = segmentWidthInternal( menu );

            return m_segmentWidth;
        }

        return segmentWidthInternal( menu );
    }

    inline qreal segmentHeight( const QskMenu* menu ) const
    {
        if ( m_isCaching )
        {
            if ( m_segmentHeight < 0.0 )
                m_segmentHeight = segmentHeightInternal( menu );

            return m_segmentHeight;
        }

        return segmentHeightInternal( menu );
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
                const auto text = sample.toString();
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

    qreal segmentWidthInternal( const QskMenu* menu ) const
    {
        using Q = QskMenu;

        const auto spacing = menu->spacingHint( Q::Segment );
        const auto padding = menu->paddingHint( Q::Segment );

        auto w = graphicWidth( menu ) + spacing + textWidth( menu );

        w += padding.left() + padding.right();

        const auto minWidth = menu->strutSizeHint( Q::Segment ).width();
        return qMax( w, minWidth );
    }

    qreal segmentHeightInternal( const QskMenu* menu ) const
    {
        using Q = QskMenu;

        const auto graphicHeight = menu->strutSizeHint( Q::Graphic ).height();
        const auto textHeight = menu->effectiveFontHeight( Q::Text );
        const auto padding = menu->paddingHint( Q::Segment );

        qreal h = qMax( graphicHeight, textHeight );
        h += padding.top() + padding.bottom();

        const auto minHeight = menu->strutSizeHint( Q::Segment ).height();
        h = qMax( h, minHeight );

        return h;
    }

    bool m_isCaching;

    mutable qreal m_graphicWidth = -1.0;
    mutable qreal m_textWidth = -1.0;
    mutable qreal m_segmentHeight = -1.0;
    mutable qreal m_segmentWidth = -1.0;
};

QskMenuSkinlet::QskMenuSkinlet( QskSkin* skin )
    : Inherited( skin )
    , m_data( new PrivateData() )
{
    appendNodeRoles( { PanelRole } );
}

QskMenuSkinlet::~QskMenuSkinlet() = default;

QRectF QskMenuSkinlet::cursorRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect, int index ) const
{
    const auto count = sampleCount( skinnable, QskMenu::Segment );

    auto rect = sampleRect( skinnable, contentsRect,
        QskMenu::Segment, qBound( 0, index, count ) );

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

    if ( subControl == Q::Segment )
    {
        const auto r = menu->subControlContentsRect( Q::Panel );

        auto h = m_data->segmentHeight( menu );

        if ( int n = m_data->separatorsBefore( menu, index ) )
        {
            // spacing ???

            const qreal separatorH = menu->metric( Q::Separator | QskAspect::Size );
            h += n * separatorH;
        }

        return QRectF( r.x(), r.y() + index * h, r.width(), h );
    }

    if ( subControl == QskMenu::Graphic || subControl == QskMenu::Text )
    {
        const auto r = sampleRect( menu, contentsRect, Q::Segment, index );
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
                const auto spacing = skinnable->spacingHint( Q::Segment );
                textRect.setX( r.x() + graphicWidth + spacing );
            }

            return textRect;
        }
    }

    if ( subControl == QskMenu::Separator )
    {
        const int pos = menu->separatorPosition( index );
        if ( pos < 0 )
            return QRectF();

        QRectF r = menu->subControlContentsRect( Q::Panel );

        if ( pos < menu->count() )
        {
            const auto segmentRect = sampleRect( skinnable, contentsRect, Q::Segment, pos );
            r.setBottom( segmentRect.top() ); // spacing ???
        }

        const qreal h = menu->metric( Q::Separator | QskAspect::Size );
        r.setTop( r.bottom() - h );

        return r;
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

    if ( subControl == Q::Segment || subControl == Q::Graphic || subControl == Q::Text )
    {
        const auto menu = static_cast< const QskMenu* >( skinnable );
        return menu->count();
    }

    if ( subControl == Q::Separator )
    {
        const auto menu = static_cast< const QskMenu* >( skinnable );
        return menu->separatorCount();
    }

    return Inherited::sampleCount( skinnable, subControl );
}

QskAspect::States QskMenuSkinlet::sampleStates(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl, int index ) const
{
    using Q = QskMenu;

    auto states = Inherited::sampleStates( skinnable, subControl, index );

    if ( subControl == Q::Segment || subControl == Q::Graphic || subControl == Q::Text )
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
    enum { Panel, Segment, Cursor, Graphic, Text, Separator };
    static QVector< quint8 > roles = { Panel, Separator, Segment, Cursor, Graphic, Text };

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
            case Segment:
            {
                newNode = updateSeriesNode( skinnable, QskMenu::Segment, oldNode );
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
            case Separator:
            {
                newNode = updateSeriesNode( skinnable, QskMenu::Separator, oldNode );
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

    if ( subControl == Q::Segment )
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
            text, menu->textOptions(), Q::Text );
    }

    if ( subControl == Q::Separator )
    {
        return updateBoxNode( menu, node, rect, subControl );
    }

    return nullptr;
}

QSizeF QskMenuSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    using Q = QskMenu;
    const auto menu = static_cast< const QskMenu* >( skinnable );

    const PrivateData::CacheGuard guard( m_data.get() );

    qreal w = 0.0;
    qreal h = 0.0;

    if ( const auto count = sampleCount( skinnable, Q::Segment ) )
    {
        w = m_data->segmentWidth( menu );
        h = count * m_data->segmentHeight( menu );
    }

    if ( const auto count = sampleCount( skinnable, Q::Separator ) )
    {
        h += count * menu->metric( Q::Separator | QskAspect::Size );
    }

    auto hint = skinnable->outerBoxSize( QskMenu::Panel, QSizeF( w, h ) );
    hint = hint.expandedTo( skinnable->strutSizeHint( QskMenu::Panel ) );

    return hint;
}

#include "moc_QskMenuSkinlet.cpp"
