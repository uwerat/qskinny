/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextFieldSkinlet.h"
#include "QskTextField.h"

#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxShapeMetrics.h"
#include "QskFunctions.h"

#include <QFontMetricsF>

using Q = QskTextField;

QskTextFieldSkinlet::QskTextFieldSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( {
        PanelRole,
        LabelTextRole,
        HintTextRole,
    } );
}

QskTextFieldSkinlet::~QskTextFieldSkinlet()
{
}

QRectF QskTextFieldSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Q::Panel )
    {
        return contentsRect;
    }
    else if ( subControl == Q::Text )
    {
        const auto textField = static_cast< const Q* >( skinnable );

        auto rect = textField->subControlRect( Q::Panel );

        const auto leadingIconRect = textField->subControlRect( Q::LeadingIcon );
        rect.setLeft( leadingIconRect.right() );

        const auto trailingIconRect = textField->subControlRect( Q::TrailingIcon );

        if( trailingIconRect.isValid() )
            rect.setRight( trailingIconRect.left() );
        else
            rect.setRight( contentsRect.right() );

        const auto margins = skinnable->marginHint( subControl );
        rect = rect.marginsRemoved( margins );

        return rect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSizeF QskTextFieldSkinlet::adjustSizeHint(
    const QskSkinnable*, Qt::SizeHint, const QSizeF& hint ) const
{
    return hint;
}

QSGNode* QskTextFieldSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto textField = static_cast< const Q* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            if ( !textField->hasPanel() )
                return nullptr;

            return updateBoxNode( skinnable, node, Q::Panel );
        }

        case LabelTextRole:
        {
            return updateTextNode( skinnable, node,
                textField->labelText(), Q::LabelText );
        }

        case HintTextRole:
        {
            return updateTextNode( skinnable, node,
                textField->placeholderText(), Q::PlaceholderText );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskTextFieldSkinlet.cpp"
