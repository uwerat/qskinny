/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFluent2TextFieldSkinlet.h"
#include "QskTextField.h"

using Q = QskTextField;

QskFluent2TextFieldSkinlet::QskFluent2TextFieldSkinlet( QskSkin* skin )
    : Inherited( skin )
{
}

QskFluent2TextFieldSkinlet::~QskFluent2TextFieldSkinlet()
{
}

QRectF QskFluent2TextFieldSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Q::TextPanel )
    {
        auto rect = subControlRect( skinnable, contentsRect, Q::Panel );
        rect.setY( rect.bottom() - skinnable->strutSizeHint( subControl ).height() );

        return rect;
    }
    else if ( subControl == Q::LabelText )
    {
        const auto rect = subControlRect( skinnable, contentsRect, Q::TextPanel );
        const auto h = skinnable->effectiveFontHeight( Q::LabelText );

        return QRectF( rect.x(), rect.y() - h, rect.width(), h );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSizeF QskFluent2TextFieldSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    auto hint = Inherited::sizeHint( skinnable, which, constraint );

    const auto textField = static_cast< const QskTextField* >( skinnable );
    if ( !textField->labelText().isEmpty() )
    {
        // spacing ???
        hint.rheight() += textField->strutSizeHint( Q::LabelText ).height();
    }

    return hint;
}

#include "moc_QskFluent2TextFieldSkinlet.cpp"
