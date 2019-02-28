/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MyToggleButtonSkinlet.h"
#include "MyToggleButton.h"

#include <QskTextOptions.h>
#include <QskGraphic.h>

static inline QRectF sectionRect( const QRectF& rect, int section )
{
    auto r = rect;

    if ( section == 0 )
        r.setWidth( 0.5 * rect.width() );
    else
        r.setLeft( rect.right() - 0.5 * rect.width() );

    return r;
}

MyToggleButtonSkinlet::MyToggleButtonSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    // sorted in stacking order

    setNodeRoles(
        { PanelRole, CursorRole, UncheckedPanelRole, UncheckedLabelRole,
          UncheckedIconRole, CheckedPanelRole, CheckedLabelRole, CheckedIconRole
        }
    );
}

QRectF MyToggleButtonSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    using Q = MyToggleButton;

    const auto button = static_cast< const MyToggleButton* >( skinnable );

    if( subControl == Q::Panel )
    {
        return button->contentsRect();
    }
    else if( subControl == Q::UncheckedPanel )
    {
        const auto rect = innerRect( skinnable, Q::Panel );
        return sectionRect( rect, button->isInverted() ? 0 : 1 );
    }
    else if( subControl == Q::CheckedPanel )
    {
        const auto rect = innerRect( skinnable, Q::Panel );
        return sectionRect( rect, button->isInverted() ? 1 : 0 );
    }
    else if( subControl == Q::CheckedLabel || subControl == Q::CheckedIcon )
    {
        return innerRect( skinnable, Q::CheckedPanel );
    }
    else if( subControl == Q::UncheckedLabel || subControl == Q::UncheckedIcon )
    {
        return innerRect( skinnable, Q::UncheckedPanel );
    }
    else if( subControl == Q::Cursor )
    {
        auto position = skinnable->metric( Q::Cursor | QskAspect::Position );
        if ( button->isInverted() )
            position = 1.0 - position;

        auto rect = innerRect( skinnable, Q::Panel );

        rect.setWidth( 0.5 * rect.width() );
        rect.moveLeft( rect.left() + position * rect.width() );

        return rect;
    }

    return Inherited::subControlRect( skinnable, subControl );
}

QRectF MyToggleButtonSkinlet::innerRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    return skinnable->innerBox( subControl, subControlRect( skinnable, subControl ) );
}

QSGNode* MyToggleButtonSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    using Q = MyToggleButton;

    const auto button = static_cast< const Q* >( skinnable );

    switch( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( button, node, Q::Panel );
        }

        case CheckedLabelRole:
        {
            return updateTextNode(
                button, node, button->textAt( 1 ),
                button->textOptions(), Q::CheckedLabel );
        }

        case UncheckedLabelRole:
        {
            return updateTextNode(
                button, node, button->textAt( 0 ),
                button->textOptions(), Q::UncheckedLabel );
        }

        case CheckedIconRole:
        {
            return updateGraphicNode(
                button, node, button->graphicAt( 1 ), Q::CheckedIcon );
        }

        case UncheckedIconRole:
        {
            return updateGraphicNode(
                button, node, button->graphicAt( 0 ), Q::UncheckedIcon );
        }

        case CheckedPanelRole:
        case UncheckedPanelRole:
        {
            // not implemented
            return nullptr;
        }

        case CursorRole:
        {
            return updateBoxNode( button, node, Q::Cursor );
        }

        default:
        {
            return nullptr;
        }
    }
}

#include "moc_MyToggleButtonSkinlet.cpp"
