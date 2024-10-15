/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextInputSkinlet.h"
#include "QskTextInput.h"

#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxShapeMetrics.h"
#include "QskFunctions.h"

#include <QFontMetricsF>

using Q = QskTextInput;

QskTextInputSkinlet::QskTextInputSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( {
        PanelRole,
        LabelTextRole,
        HintTextRole,
    } );
}

QskTextInputSkinlet::~QskTextInputSkinlet()
{
}

QRectF QskTextInputSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto input = static_cast< const Q* >( skinnable );

    if ( subControl == Q::Panel )
    {
        return contentsRect;
    }
    else if ( subControl == Q::InputText )
    {
        const auto margins = input->marginHint( subControl );

        const auto leadingIconRect = input->subControlRect( Q::LeadingIcon );
        const auto panelRect = input->subControlRect( Q::Panel );
        auto rect = panelRect;
        rect.setLeft( leadingIconRect.right() );

        const auto trailingIconRect = input->subControlRect( Q::TrailingIcon );

        if( trailingIconRect.isValid() )
        {
            rect.setRight( trailingIconRect.left() );
        }
        else
        {
            rect.setRight( contentsRect.right() );
        }

        rect = rect.marginsRemoved( margins );

        return rect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSizeF QskTextInputSkinlet::adjustSizeHint( const QskSkinnable*, Qt::SizeHint, const QSizeF& hint ) const
{
    return hint;
}

QSGNode* QskTextInputSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto input = static_cast< const Q* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            if ( !input->hasPanel() )
                return nullptr;

            return updateBoxNode( skinnable, node, Q::Panel );
        }

        case LabelTextRole:
        {
            return updateTextNode( skinnable, node, input->labelText(), Q::LabelText );
        }

        case HintTextRole:
        {
            return updateTextNode( skinnable, node, input->hintText(), Q::HintText );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskTextInputSkinlet.cpp"
