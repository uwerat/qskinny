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
    else if ( subControl == Q::PlaceholderText )
    {
        if( input->hasSkinState( Q::TextPopulated ) )
        {
            return {};
        }
        else
        {
            return input->subControlRect( Q::Text );
        }
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSizeF QskFluent2TextFieldSkinlet::adjustSizeHint(
    const QskSkinnable* skinnable, Qt::SizeHint which, const QSizeF& oldHint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    qreal h = skinnable->strutSizeHint( Q::Panel ).height();

    const auto input = static_cast< const Q* >( skinnable );
    if ( !input->labelText().isEmpty() )
        h += input->strutSizeHint( Q::LabelText ).height();

    return QSizeF( oldHint.width(), h );
}

#include "moc_QskFluent2TextFieldSkinlet.cpp"
