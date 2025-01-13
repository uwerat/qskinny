/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMaterial3TextAreaSkinlet.h"
#include "QskTextArea.h"

QskMaterial3TextAreaSkinlet::QskMaterial3TextAreaSkinlet( QskSkin* skin )
    : Inherited( skin )
{
}

QskMaterial3TextAreaSkinlet::~QskMaterial3TextAreaSkinlet()
{
}

QRectF QskMaterial3TextAreaSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskMaterial3TextAreaSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF QskMaterial3TextAreaSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    return Inherited::sizeHint( skinnable, which, constraint );
}

#include "moc_QskMaterial3TextAreaSkinlet.cpp"
