/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSegmentedBarSkinlet.h"
#include "QskSegmentedBar.h"

#include "QskGraphic.h"
#include "QskColorFilter.h"
#include "QskTextOptions.h"
#include "QskSGNode.h"
#include "QskFunctions.h"

#include <qfontmetrics.h>
#include <qmath.h>

QskSegmentedBarSkinlet::QskSegmentedBarSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, SegmentRole, CursorRole, TextRole, GraphicRole  } );
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

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QRectF QskSegmentedBarSkinlet::cursorRect(
    const QskSegmentedBar* bar, const QRectF& contentsRect ) const
{
    using Q = QskSegmentedBar;

    if( bar->selectedIndex() < 0 || bar->count() <= 0 )
        return QRectF();

    auto rect = subControlRect( bar, contentsRect, Q::Panel );
    rect = bar->innerBox( Q::Panel, rect );

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

QRectF QskSegmentedBarSkinlet::segmentRect(
    const QskSegmentedBar* bar, const QRectF& contentsRect, int index ) const
{
    using Q = QskSegmentedBar;

    const auto spacing = bar->spacingHint( Q::Panel );
    const auto count = bar->count();

    auto rect = subControlRect( bar, contentsRect, Q::Panel );
    rect = bar->innerBox( Q::Panel, rect );

    if( bar->orientation() == Qt::Horizontal )
    {
        const qreal w = ( rect.width() - ( count - 1 ) * spacing ) / count;

        rect.setLeft( index * ( w + spacing ) );
        rect.setWidth( w );
    }
    else
    {
        const qreal h = ( rect.height() - ( count - 1 ) * spacing ) / count;

        rect.setTop( index * ( h + spacing ) );
        rect.setHeight( h );
    }

    return rect;
}

QSGNode* QskSegmentedBarSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    using Q = QskSegmentedBar;

    switch( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( skinnable, node, Q::Panel );

        case CursorRole:
            return updateBoxNode( skinnable, node, Q::Cursor );

        case SegmentRole:
            return updateSeriesNode( skinnable, Q::Segment, node );

        case TextRole:
            return updateSeriesNode( skinnable, Q::Text, node );

        case GraphicRole:
            return updateSeriesNode( skinnable, Q::Graphic, node );
    }

    return nullptr;
}

QSizeF QskSegmentedBarSkinlet::segmentSizeHint( const QskSegmentedBar* bar ) const
{
    qreal widthMax = 0;
    qreal graphicRatioMax = 0;

    const QFontMetricsF fm( bar->effectiveFont( QskSegmentedBar::Text ) );

    for ( int i = 0; i < bar->count(); i++ )
    {
        const auto value = bar->optionAt( i );

        if ( value.canConvert< QskGraphic >() )
        {
            const auto graphic = value.value< QskGraphic >();

            if ( !graphic.isNull() )
            {
                const auto sz = graphic.defaultSize();

                if( sz.isValid() )
                {
                    const qreal ratio = sz.width() / sz.height();

                    if( graphicRatioMax < ratio )
                        graphicRatioMax = ratio;
                }
            }
        }
        else if ( value.canConvert< QString >() )
        {
            const auto text = value.value< QString >();
            if ( !text.isEmpty() )
            {
                const auto sz = fm.size( Qt::TextShowMnemonic, text );

                if( sz.width() > widthMax )
                    widthMax = sz.width();
            }
        }
    }

    if( graphicRatioMax > 0 )
    {
        const qreal w = fm.height() * graphicRatioMax;

        if( w > widthMax )
            widthMax = w;
    }

    return bar->outerBoxSize( QskSegmentedBar::Segment, QSizeF( widthMax, fm.height() ) );
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
        const auto segmentSize = segmentSizeHint( bar );

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

    if ( subControl == Q::Segment )
    {
        const auto bar = static_cast< const QskSegmentedBar* >( skinnable );
        return segmentRect( bar, contentsRect, index );
    }

    if ( subControl == Q::Text || subControl == Q::Graphic )
    {
        const auto rect = sampleRect( skinnable, contentsRect, Q::Segment, index );
        return skinnable->innerBox( Q::Segment, rect );
    }

    return Inherited::sampleRect( skinnable, contentsRect, subControl, index );
}

QskAspect::States QskSegmentedBarSkinlet::sampleStates(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl, int index ) const
{
    using Q = QskSegmentedBar;

    auto states = Inherited::sampleStates( skinnable, subControl, index );

    if ( subControl == Q::Segment || subControl == Q::Graphic || subControl == Q::Text )
    {
        const auto bar = static_cast< const QskSegmentedBar* >( skinnable );

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

    if ( subControl == Q::Segment )
    {
        return updateBoxNode( skinnable, node, rect, subControl );
    }

    const auto alignment = bar->alignmentHint( subControl, Qt::AlignCenter );

    if ( subControl == Q::Text )
    {
        const auto value = bar->optionAt( index );
        if ( value.canConvert< QString >() )
        {
            const auto text = value.value< QString >();

            return QskSkinlet::updateTextNode( bar, node,
                rect, alignment, text, bar->textOptions(), Q::Text );
        }

        return nullptr;
    }

    if ( subControl == Q::Graphic )
    {
        const auto value = bar->optionAt( index );
        if ( value.canConvert< QskGraphic >() )
        {
            const auto graphic = value.value< QskGraphic >();
            const auto filter = bar->effectiveGraphicFilter( subControl );

            return QskSkinlet::updateGraphicNode(
                bar, node, graphic, filter, rect, alignment );
        }

        return nullptr;
    }

    return Inherited::updateSampleNode( skinnable, subControl, index, node );
}

#include "moc_QskSegmentedBarSkinlet.cpp"
