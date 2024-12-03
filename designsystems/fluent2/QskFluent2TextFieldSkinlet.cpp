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
    if ( subControl == Q::Panel )
    {
        auto rect = contentsRect;
        rect.setY( rect.bottom() - skinnable->strutSizeHint( subControl ).height() );

        return rect;
    }
    else if ( subControl == Q::LabelText )
    {
        auto rect = contentsRect;
        rect.setHeight( skinnable->strutSizeHint( subControl ).height() );

        return rect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSizeF QskFluent2TextFieldSkinlet::adjustSizeHint(
    const QskSkinnable* skinnable, Qt::SizeHint which, const QSizeF& oldHint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    qreal h = skinnable->strutSizeHint( Q::Panel ).height();

    const auto textField = static_cast< const Q* >( skinnable );
    if ( !textField->labelText().isEmpty() )
        h += textField->strutSizeHint( Q::LabelText ).height();

    return QSizeF( oldHint.width(), h );
}

#include "moc_QskFluent2TextFieldSkinlet.cpp"
