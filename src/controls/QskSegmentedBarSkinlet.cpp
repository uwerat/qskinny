/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSegmentedBarSkinlet.h"
#include "QskSegmentedBar.h"

#include "QskLabelData.h"
#include "QskGraphic.h"
#include "QskColorFilter.h"
#include "QskFunctions.h"
#include "QskSGNode.h"
#include "QskSkin.h"
#include "QskSkinStateChanger.h"
#include "QskSubcontrolLayoutEngine.h"
#include "QskBoxHints.h"

#include <qfontmetrics.h>
#include <qmath.h>

namespace
{
    QskBoxHints effectiveBoxHints( QskAspect::Subcontrol subControl,
        const QskSegmentedBar* bar, int index )
    {
        using Q = QskSegmentedBar;

        auto boxHints = bar->boxHints( subControl );

        const bool leading = ( index == 0 );
        const bool trailing = ( index == bar->count() - 1 );

        if ( !( leading || trailing ) )
            return boxHints;

        // something more expressive than just a boolean. TODO ...
        if ( !bar->flagHint< bool >( Q::Panel | QskAspect::Option, false ) )
            return boxHints;

        const auto panelShape = bar->boxShapeHint( Q::Panel );
        auto& shape = boxHints.shape;

        // when there is only 1 segment we have to fit both ends

        if ( leading )
        {
            Qt::Corner corners[2];

            corners[0] = Qt::TopLeftCorner;

            if ( bar->orientation() == Qt::Vertical )
                corners[1] = Qt::TopRightCorner;
            else
                corners[1] = Qt::BottomLeftCorner;

            shape.setSizeMode( panelShape.sizeMode() );
            shape.setRadius( corners[0], panelShape.radius( corners[0] ) );
            shape.setRadius( corners[1], panelShape.radius( corners[1] ) );
        }

        if ( trailing )
        {
            Qt::Corner corners[2];

            corners[0] = Qt::BottomRightCorner;

            if ( bar->orientation() == Qt::Vertical )
                corners[1] = Qt::BottomLeftCorner;
            else
                corners[1] = Qt::TopRightCorner;

            shape.setSizeMode( panelShape.sizeMode() );
            shape.setRadius( corners[0], panelShape.radius( corners[0] ) );
            shape.setRadius( corners[1], panelShape.radius( corners[1] ) );
        }

        boxHints.borderMetrics = bar->boxBorderMetricsHint( Q::Panel );
        boxHints.borderColors = QColor();

        return boxHints;
    }


    QskGraphic iconAt( const QskSegmentedBar* bar, const int index )
    {
        using Q = QskSegmentedBar;

        if ( bar->selectedIndex() == index )
        {
            /*
                Material 3 replaces the icon of the selected element by a checkmark,
                when icon and text are set. So this code is actually not correct
                as it also replaces the icon when there is no text
             */

            const auto icon = bar->symbolHint( Q::Icon | Q::Selected );
            if ( !icon.isNull() )
                return icon;
        }

        return bar->optionAt( index ).icon().graphic();
    }

    class LayoutEngine : public QskSubcontrolLayoutEngine
    {
      public:
        LayoutEngine( const QskSegmentedBar* bar, int index )
            : QskSubcontrolLayoutEngine( bar->orientation() )
        {
            using Q = QskSegmentedBar;

            setSpacing( bar->spacingHint( Q::Panel ) );

            const auto option = bar->optionAt( index );

            setGraphicTextElements( bar, Q::Text, option.text(),
                Q::Icon, iconAt( bar, index ).defaultSize() );

            if( bar->orientation() == Qt::Horizontal )
            {
                const auto alignment = bar->alignmentHint( Q::Panel, Qt::AlignCenter );
                setFixedContent( Q::Text, Qt::Horizontal, alignment );
            }
        }
    };
}

QskSegmentedBarSkinlet::QskSegmentedBarSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, SegmentRole, SeparatorRole,
        CursorRole, SplashRole, TextRole, IconRole  } );
}

QskSegmentedBarSkinlet::~QskSegmentedBarSkinlet() = default;

QRectF QskSegmentedBarSkinlet::subControlRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl ) const
{
    using Q = QskSegmentedBar;

    const auto bar = static_cast< const QskSegmentedBar* >( skinnable );

    if( subControl == Q::Panel )
        return contentsRect;

    if( subControl == Q::Cursor )
        return cursorRect( bar, contentsRect );

    if( subControl == Q::Splash )
        return splashRect( bar, contentsRect );

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QRectF QskSegmentedBarSkinlet::cursorRect(
    const QskSegmentedBar* bar, const QRectF& contentsRect ) const
{
    using Q = QskSegmentedBar;

    if( bar->selectedIndex() < 0 || bar->count() <= 0 )
        return QRectF();

    auto rect = subControlRect( bar, contentsRect, Q::Panel );

    if( rect.isEmpty() )
        return QRectF();

    // position is related to the index: 2.5 means
    // the cursor is between segment[2] and segment[3]

    const qreal position = bar->positionHint( Q::Cursor );

    const int index1 = qFloor( position );
    const int index2 = qCeil( position );

    auto cursorRect = segmentRect( bar, contentsRect, index1 );
    if ( index1 != index2 )
    {
        const auto targetRect = segmentRect( bar, contentsRect, index2 );
        cursorRect = qskInterpolatedRect( cursorRect, targetRect, position - index1 );
    }

    return cursorRect;
}

QRectF QskSegmentedBarSkinlet::splashRect(
    const QskSegmentedBar* bar, const QRectF& contentsRect ) const
{
    using Q = QskSegmentedBar;

    QRectF rect;

    const auto ratio = bar->metric( Q::Splash | QskAspect::Size );

    if ( ratio > 0.0 )
    {
        const auto pos = bar->effectiveSkinHint(
            Q::Splash | QskAspect::Metric | QskAspect::Position ).toPointF();

        const int index = bar->indexAtPosition( pos );

        if( index >= 0 && index < bar->count() )
        {
            const auto sr = segmentRect( bar, contentsRect, index );
            rect = sr;
            qreal w, h;

            if( bar->orientation() == Qt::Horizontal )
            {
                w = 2.0 * rect.width() * ratio;
                h = 2.0 * rect.height();
            }
            else
            {
                w = 2.0 * rect.width();
                h = 2.0 * rect.height() * ratio;
            }

            rect.setSize( { w, h } );
            rect.moveCenter( pos );
            rect = rect.intersected( sr );
        }
    }

    return rect;
}

QRectF QskSegmentedBarSkinlet::segmentRect(
    const QskSegmentedBar* bar, const QRectF& contentsRect, int index ) const
{
    using Q = QskSegmentedBar;

    const auto count = bar->count();

    auto rect = subControlRect( bar, contentsRect, Q::Panel );

    if( bar->orientation() == Qt::Horizontal )
    {
        const qreal w = rect.width() / count;

        rect.setLeft( index * w );
        rect.setWidth( w );
    }
    else
    {
        const qreal h = rect.height() / count;

        rect.setTop( index * h );
        rect.setHeight( h );
    }

    return rect;
}

QRectF QskSegmentedBarSkinlet::separatorRect(
    const QskSegmentedBar* bar, const QRectF& contentsRect, int index ) const
{
    using Q = QskSegmentedBar;

    if( index == bar->count() - 1 )
        return QRectF(); // no separator behind the last segment

    auto rect = segmentRect( bar, contentsRect, index );

    auto sh = bar->sizeHint();

    const QSizeF strutSize = bar->strutSizeHint( Q::Separator );

    if( bar->orientation() == Qt::Horizontal )
    {
        rect.setLeft( rect.right() );
        rect.setSize( { strutSize.width(), sh.height() } );
    }
    else
    {
        rect.setTop( rect.bottom() );
        rect.setSize( { sh.width(), strutSize.height() } );
    }

    const auto padding = bar->paddingHint( Q::Separator );
    rect = rect.marginsRemoved( padding );

    return rect;
}

QSGNode* QskSegmentedBarSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    using Q = QskSegmentedBar;

    const auto bar = static_cast< const QskSegmentedBar* >( skinnable );

    switch( nodeRole )
    {
        case CursorRole:
            return updateBoxNode( skinnable, node, Q::Cursor );

        case SplashRole:
            return updateSplashNode( bar, node );

        case PanelRole:
            return updateBoxNode( skinnable, node, Q::Panel );

        case SegmentRole:
            return updateSeriesNode( skinnable, Q::Segment, node );

        case SeparatorRole:
            return updateSeriesNode( skinnable, Q::Separator, node );

        case TextRole:
            return updateSeriesNode( skinnable, Q::Text, node );

        case IconRole:
            return updateSeriesNode( skinnable, Q::Icon, node );
    }

    return nullptr;
}

QSizeF QskSegmentedBarSkinlet::segmentSizeHint(
    const QskSegmentedBar* bar, Qt::SizeHint which ) const
{
    using Q = QskSegmentedBar;

    const QSizeF sizeSymbol =
        bar->symbolHint( Q::Icon | Q::Selected ).defaultSize();

    QSizeF segmentSize;

    for ( int i = 0; i < bar->count(); i++ )
    {
        const auto option = bar->optionAt( i );

        auto iconSize = option.icon().graphic().defaultSize();
        iconSize = iconSize.expandedTo( sizeSymbol );

        LayoutEngine layoutEngine( bar, i );
        layoutEngine.setGraphicTextElements( bar,
            Q::Text, option.text(), Q::Icon, iconSize );

        const auto size = layoutEngine.sizeHint( which, QSizeF() );
        segmentSize = segmentSize.expandedTo( size );
    }

    segmentSize = bar->outerBoxSize( Q::Segment, segmentSize );
    segmentSize = segmentSize.expandedTo( bar->strutSizeHint( Q::Segment ) );
    segmentSize = segmentSize.grownBy( bar->marginHint( Q::Segment ) );

    return segmentSize;
}

QSizeF QskSegmentedBarSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    using Q = QskSegmentedBar;

    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto count = sampleCount( skinnable, Q::Segment );
    if( count == 0 )
        return QSizeF( 0, 0 );

    qreal w = 0;
    qreal h = 0;

    if ( count > 0 )
    {
        const qreal spacing = skinnable->spacingHint( Q::Panel );

        const auto bar = static_cast< const QskSegmentedBar* >( skinnable );
        const auto segmentSize = segmentSizeHint( bar, which );

        if( bar->orientation() == Qt::Horizontal )
        {
            w = count * segmentSize.width() + ( count - 1 ) * spacing;
            h = segmentSize.height();
        }
        else
        {
            w = segmentSize.width();
            h = count * segmentSize.height() + ( count - 1 ) * spacing;
        }
    }

    const auto hint = skinnable->outerBoxSize( Q::Panel, QSizeF( w, h ) );
    return hint.expandedTo( skinnable->strutSizeHint( Q::Panel ) );
}

int QskSegmentedBarSkinlet::sampleCount(
    const QskSkinnable* skinnable, QskAspect::Subcontrol ) const
{
    const auto bar = static_cast< const QskSegmentedBar* >( skinnable );
    return bar->count();
}

QRectF QskSegmentedBarSkinlet::sampleRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl, int index ) const
{
    using Q = QskSegmentedBar;
    const auto bar = static_cast< const QskSegmentedBar* >( skinnable );

    if ( subControl == Q::Segment )
    {
        return segmentRect( bar, contentsRect, index );
    }

    if ( subControl == Q::Separator )
    {
        return separatorRect( bar, contentsRect, index );
    }

    if ( subControl == Q::Text || subControl == Q::Icon )
    {
        const auto rect = sampleRect( skinnable, contentsRect, Q::Segment, index );

        LayoutEngine layoutEngine( bar, index );
        layoutEngine.setGeometries( rect );

        return layoutEngine.subControlRect( subControl );
    }

    return Inherited::sampleRect( skinnable, contentsRect, subControl, index );
}

QskAspect::States QskSegmentedBarSkinlet::sampleStates(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl, int index ) const
{
    using Q = QskSegmentedBar;
    using A = QskAspect;

    auto states = Inherited::sampleStates( skinnable, subControl, index );

    const auto* bar = static_cast< const QskSegmentedBar* >( skinnable );

    if ( subControl == Q::Segment )
    {
        if ( bar->isSegmentEnabled( index ) )
        {
            if ( bar->selectedIndex() == index )
                states |= Q::Selected;
        }
        else
        {
            states |= Q::Disabled;
        }

        const auto cursorPos = bar->effectiveSkinHint(
            Q::Segment | Q::Hovered | A::Metric | A::Position ).toPointF();

        if( !cursorPos.isNull() && bar->indexAtPosition( cursorPos ) == index )
        {
            states |= Q::Hovered;
        }
        else
        {
            states &= ~Q::Hovered;
        }

        const auto focusIndex = bar->positionHint( Q::Segment | Q::Focused );

        if( focusIndex >= 0 && focusIndex < bar->count() )
        {
            if( focusIndex == index )
            {
                states |= Q::Focused;
            }
            else
            {
                states &= ~Q::Focused;
            }
        }
    }
    else if( subControl == Q::Icon || subControl == Q::Text )
    {
        if ( bar->isSegmentEnabled( index ) )
        {
            if ( bar->selectedIndex() == index )
                states |= Q::Selected;
        }
        else
        {
            states |= Q::Disabled;
        }
    }

    return states;
}

QSGNode* QskSegmentedBarSkinlet::updateSampleNode( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index, QSGNode* node ) const
{
    using Q = QskSegmentedBar;

    auto bar = static_cast< const QskSegmentedBar* >( skinnable );

    const auto rect = sampleRect( bar, bar->contentsRect(), subControl, index );

    if ( subControl == Q::Separator )
    {
        return updateBoxNode( skinnable, node, rect, subControl );
    }

    if ( subControl == Q::Segment )
    {
        const auto boxHints = effectiveBoxHints( subControl, bar, index );
        return updateBoxNode( bar, node, rect, boxHints );
    }

    const auto alignment = bar->alignmentHint( subControl, Qt::AlignCenter );

    if ( subControl == Q::Text )
    {
        const auto text = bar->optionAt( index ).text();

        if( !text.isEmpty() )
        {
            return QskSkinlet::updateTextNode( bar, node,
                rect, alignment, text, Q::Text );
        }

        return nullptr;
    }

    if ( subControl == Q::Icon )
    {
        const auto graphic = iconAt( bar, index );

        if( !graphic.isEmpty() )
        {
            const auto filter = bar->effectiveGraphicFilter( subControl );
            const auto padding = bar->paddingHint( Q::Icon );
            const auto graphicRect = rect.marginsRemoved( padding );

            return QskSkinlet::updateGraphicNode(
                bar, node, graphic, filter, graphicRect, alignment );
        }

        return nullptr;
    }

    return Inherited::updateSampleNode( skinnable, subControl, index, node );
}

QSGNode* QskSegmentedBarSkinlet::updateSplashNode(
    const QskSegmentedBar* bar, QSGNode* node ) const
{
    using Q = QskSegmentedBar;

    const auto splashRect = bar->subControlRect( Q::Splash );
    if ( splashRect.isEmpty() )
        return nullptr;

    auto clipNode = updateBoxClipNode(
        bar, node, bar->subControlRect( Q::Panel ), Q::Panel );

    if ( clipNode )
    {
        auto boxNode = updateBoxNode( bar, clipNode->firstChild(), splashRect, Q::Splash );
        if ( boxNode->parent() == nullptr )
            clipNode->appendChildNode( boxNode );
    }

    return clipNode;
}

#include "moc_QskSegmentedBarSkinlet.cpp"
