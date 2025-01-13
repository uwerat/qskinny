/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFluent2TextAreaSkinlet.h"
#include "QskTextArea.h"

using Q = QskTextArea;

QskFluent2TextAreaSkinlet::QskFluent2TextAreaSkinlet( QskSkin* skin )
    : Inherited( skin )
{
}

QskFluent2TextAreaSkinlet::~QskFluent2TextAreaSkinlet()
{
}

QRectF QskFluent2TextAreaSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSizeF QskFluent2TextAreaSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    return Inherited::sizeHint( skinnable, which, constraint );
}

#include "moc_QskFluent2TextAreaSkinlet.cpp"
