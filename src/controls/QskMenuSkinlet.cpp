/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMenuSkinlet.h"
#include "QskMenu.h"

#include "QskGraphic.h"
#include "QskColorFilter.h"
#include "QskTextOptions.h"
#include "QskFunctions.h"
#include "QskMargins.h"
#include "QskFunctions.h"
#include "QskLabelData.h"

#include "QskSGNode.h"
#include "QskSlideInNode.h"

#include <qfontmetrics.h>
#include <qmath.h>

static inline int qskActionIndex( const QskMenu* menu, int optionIndex )
{
    if ( optionIndex < 0 )
        return -1;

    const auto& actions = menu->actions();

    auto it = std::lower_bound(
        actions.constBegin(), actions.constEnd(), optionIndex );

    return it - actions.constBegin();
}

static inline qreal qskPaddedSeparatorHeight( const QskMenu* menu )
{
    using Q = QskMenu;

    const auto margins = menu->marginHint( Q::Separator );

    return menu->metric( Q::Separator | QskAspect::Size )
        + margins.top() + margins.bottom();
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
        const auto hint = menu->strutSizeHint( QskMenu::Icon );
        const qreal textHeight = menu->effectiveFontHeight( QskMenu::Text );

        const auto h = qMax( hint.height(), textHeight );

        qreal maxW = 0.0;

        const auto options = menu->options();
        for ( auto& option : options )
        {
            const auto graphic = option.icon().graphic();
            if ( !graphic.isNull() )
            {
                const auto w = graphic.widthForHeight( h );
                if( w > maxW )
                    maxW = w;
            }
        }

        return qMax( hint.width(), maxW );
    }

    qreal textWidthInternal( const QskMenu* menu ) const
    {
        const QFontMetricsF fm( menu->effectiveFont( QskMenu::Text ) );

        auto maxWidth = 0.0;

        const auto options = menu->options();
        for ( auto& option : options )
        {
            if( !option.text().isEmpty() )
            {
                const auto w = qskHorizontalAdvance( fm, option.text() );
                if( w > maxWidth )
                    maxWidth = w;
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

        const auto graphicHeight = menu->strutSizeHint( Q::Icon ).height();
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
    appendNodeRoles( { ContentsRole, PanelRole } );
}

QskMenuSkinlet::~QskMenuSkinlet() = default;

QSGNode* QskMenuSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    switch ( nodeRole )
    {
        case ContentsRole:
        {
            /*
                QskSlideInNode works for controls made of nodes - not for
                containers of other quick items. TODO ...
             */

            const auto popup = static_cast< const QskPopup* >( skinnable );

            auto rect = popup->contentsRect();
            if ( rect.isEmpty() )
                return nullptr;

            auto slideInNode = QskSGNode::ensureNode< QskSlideInNode >( node );

            slideInNode->updateTranslation(
                rect, Qt::TopEdge, popup->transitioningFactor() );

            auto contentsNode = updateContentsNode( popup, slideInNode->contentsNode() );
            slideInNode->setContentsNode( contentsNode );

            return slideInNode;
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskMenuSkinlet::cursorRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect, int index ) const
{
    using Q = QskMenu;

    const auto menu = static_cast< const QskMenu* >( skinnable );
    const auto actions = menu->actions();

    index = qskActionIndex( menu, index );

    QRectF rect;

    if ( index < 0 )
    {
        rect = sampleRect( skinnable, contentsRect, Q::Segment, 0 );
        rect.setBottom( rect.top() );
    }
    else if ( index >= actions.count() )
    {
        rect = sampleRect( skinnable, contentsRect, Q::Segment, actions.count() - 1 );
        rect.setTop( rect.bottom() );
    }
    else
    {
        rect = sampleRect( skinnable, contentsRect, Q::Segment, index );
    }

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
        const auto h = m_data->segmentHeight( menu );

        auto dy = index * h;

        if ( const auto n = menu->actions()[ index ] - index )
            dy += n * qskPaddedSeparatorHeight( menu );

        const auto r = menu->subControlContentsRect( Q::Panel );
        return QRectF( r.x(), r.y() + dy, r.width(), h );
    }

    if ( subControl == QskMenu::Icon || subControl == QskMenu::Text )
    {
        const auto r = sampleRect( menu, contentsRect, Q::Segment, index );
        const auto graphicWidth = m_data->graphicWidth( menu );

        if ( subControl == QskMenu::Icon )
        {
            auto graphicRect = r;
            graphicRect.setWidth( graphicWidth );
            const auto padding = menu->paddingHint( QskMenu::Icon );
            graphicRect = graphicRect.marginsRemoved( padding );

            return graphicRect;
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
        const auto separators = menu->separators();
        if ( index >= separators.count() )
            return QRectF();

        const auto h = qskPaddedSeparatorHeight( menu );

        auto y = index * h;

        if ( const auto n = qskActionIndex( menu, separators[ index ] ) )
            y += n * m_data->segmentHeight( menu );

        const auto r = menu->subControlContentsRect( Q::Panel );
        return QRectF( r.left(), y, r.width(), h );
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

    if ( subControl == Q::Segment || subControl == Q::Icon || subControl == Q::Text )
    {
        const auto menu = static_cast< const QskMenu* >( skinnable );
        return menu->actions().count();
    }

    if ( subControl == Q::Separator )
    {
        const auto menu = static_cast< const QskMenu* >( skinnable );
        return menu->separators().count();
    }

    return Inherited::sampleCount( skinnable, subControl );
}

QskAspect::States QskMenuSkinlet::sampleStates(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl, int index ) const
{
    using Q = QskMenu;
    using A = QskAspect;

    auto states = Inherited::sampleStates( skinnable, subControl, index );

    if ( subControl == Q::Segment || subControl == Q::Icon || subControl == Q::Text )
    {
        const auto menu = static_cast< const QskMenu* >( skinnable );

        if ( menu->currentIndex() == menu->actions()[ index ] )
        {
            states |= Q::Selected;

            if( menu->isPressed() )
            {
                states |= Q::Pressed;
            }
            else
            {
                states &= ~Q::Pressed;
            }
        }

        const auto cursorPos = menu->effectiveSkinHint(
            Q::Segment | Q::Hovered | A::Metric | A::Position ).toPointF();

        if( !cursorPos.isNull() && menu->indexAtPosition( cursorPos ) == index )
        {
            states |= Q::Hovered;
        }
        else
        {
            states &= ~Q::Hovered;
        }
    }

    return states;
}

QVariant QskMenuSkinlet::sampleAt( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index ) const
{
    using Q = QskMenu;

    if ( subControl == Q::Icon || subControl == Q::Text )
    {
        const auto menu = static_cast< const QskMenu* >( skinnable );

        const auto option = menu->optionAt( index );

        if ( subControl == Q::Icon )
            return QVariant::fromValue( option.icon().graphic() );
        else
            return QVariant::fromValue( option.text() );
    }

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
    enum { Panel, Segment, Cursor, Icon, Text, Separator };
    static QVector< quint8 > roles = { Panel, Separator, Segment, Cursor, Icon, Text };

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
            case Icon:
            {
                newNode = updateSeriesNode( skinnable, QskMenu::Icon, oldNode );
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

    if ( subControl == Q::Icon )
    {
        index = menu->actions()[ index ];

        const auto graphic = menu->optionAt( index ).icon().graphic();
        if ( graphic.isNull() )
            return nullptr;

        const auto alignment = menu->alignmentHint( subControl, Qt::AlignCenter );
        const auto filter = menu->effectiveGraphicFilter( subControl );

        return QskSkinlet::updateGraphicNode(
            menu, node, graphic, filter, rect, alignment );
    }

    if ( subControl == Q::Text )
    {
        index = menu->actions()[ index ];

        const auto text = menu->optionAt( index ).text();
        if ( text.isEmpty() )
            return nullptr;

        const auto alignment = menu->alignmentHint(
            subControl, Qt::AlignVCenter | Qt::AlignLeft );

        return QskSkinlet::updateTextNode( menu, node, rect,
            alignment, text, Q::Text );
    }

    if ( subControl == Q::Separator )
    {
        auto gradient = menu->gradientHint( subControl );
        if ( ( gradient.type() == QskGradient::Stops ) && !gradient.isMonochrome() )
            gradient.setLinearDirection( Qt::Vertical );

        return updateBoxNode( menu, node, rect, gradient, subControl );
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
        h += count * qskPaddedSeparatorHeight( menu );
    }

    auto hint = skinnable->outerBoxSize( QskMenu::Panel, QSizeF( w, h ) );
    hint = hint.expandedTo( skinnable->strutSizeHint( QskMenu::Panel ) );

    return hint;
}

#include "moc_QskMenuSkinlet.cpp"
