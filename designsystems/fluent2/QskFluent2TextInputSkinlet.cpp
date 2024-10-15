/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFluent2TextInputSkinlet.h"
#include "QskTextInput.h"

using Q = QskTextInput;

QskFluent2TextInputSkinlet::QskFluent2TextInputSkinlet( QskSkin* skin )
    : Inherited( skin )
{
}

QskFluent2TextInputSkinlet::~QskFluent2TextInputSkinlet()
{
}

QRectF QskFluent2TextInputSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto input = static_cast< const Q* >( skinnable );

    if ( subControl == Q::Panel )
    {
        auto rect = contentsRect;

        const auto h = input->strutSizeHint( subControl ).height();
        rect.setY( rect.bottom() - h );

        return rect;
    }
    else if ( subControl == Q::LabelText )
    {
        auto rect = contentsRect;

        const auto h = input->strutSizeHint( subControl ).height();
        rect.setHeight( h );

        return rect;
    }
    else if ( subControl == Q::HintText )
    {
        if( input->hasSkinState( Q::TextPopulated ) )
        {
            return {};
        }
        else
        {
            return input->subControlRect( Q::InputText );
        }
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSizeF QskFluent2TextInputSkinlet::adjustSizeHint( const QskSkinnable* skinnable, Qt::SizeHint which, const QSizeF& oldHint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto input = static_cast< const Q* >( skinnable );

    const auto labelHeight = input->labelText().isEmpty() ? 0 : input->strutSizeHint( Q::LabelText ).height();
    const auto panelHeight = input->strutSizeHint( Q::Panel ).height();

    const auto h = labelHeight + panelHeight;

    QSizeF hint( oldHint.width(), h );

    return hint;
}

#include "moc_QskFluent2TextInputSkinlet.cpp"
