/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMaterial3TextFieldSkinlet.h"
#include "QskTextField.h"

QskMaterial3TextFieldSkinlet::QskMaterial3TextFieldSkinlet( QskSkin* skin )
    : Inherited( skin )
{
}

QskMaterial3TextFieldSkinlet::~QskMaterial3TextFieldSkinlet()
{
}

QRectF QskMaterial3TextFieldSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskMaterial3TextFieldSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskMaterial3TextFieldSkinlet.cpp"
