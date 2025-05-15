/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMaterial3TextFieldSkinlet.h"
#include "QskMaterial3Skin.h"

#include <QskSkinnable.h>

static inline bool isOutlined( const QskSkinnable* skinnable )
{
    return skinnable->effectiveVariation() == QskAspect::NoVariation;
}

QskMaterial3TextFieldSkinlet::QskMaterial3TextFieldSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setRenderHints( UseHeaderAsPlaceholder );
}

QskMaterial3TextFieldSkinlet::~QskMaterial3TextFieldSkinlet()
{
}

int QskMaterial3TextFieldSkinlet::panelMode( const QskSkinnable* skinnable ) const
{
    return isOutlined( skinnable ) ? 2 : 1;
}

#include "moc_QskMaterial3TextFieldSkinlet.cpp"
