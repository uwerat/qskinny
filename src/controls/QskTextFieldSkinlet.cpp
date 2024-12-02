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
    setNodeRoles( { PanelRole, LabelTextRole, PlaceholderTextRole, } );
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
        auto rect = subControlRect( skinnable, contentsRect, Q::Panel );

        const auto r1 = subControlRect( skinnable, contentsRect, Q::LeadingIcon );
        rect.setLeft( r1.right() );

        const auto r2 = subControlRect( skinnable, contentsRect, Q::TrailingIcon );

        if( r2.isValid() )
            rect.setRight( r2.left() );
        else
            rect.setRight( contentsRect.right() );

        const auto margins = skinnable->marginHint( subControl );
        rect = rect.marginsRemoved( margins );

        return rect;
    }
    else if ( subControl == Q::PlaceholderText )
    {
        if( !skinnable->hasSkinState( Q::TextPopulated ) )
            return subControlRect( skinnable, contentsRect, Q::Text );

        return QRectF();
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

        case PlaceholderTextRole:
        {
            return updateTextNode( skinnable, node,
                textField->placeholderText(), Q::PlaceholderText );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskTextFieldSkinlet.cpp"
