/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTreeView.h"

QSK_SUBCONTROL( QskTreeView, Cell )
QSK_SUBCONTROL( QskTreeView, Text )

QskTreeView::QskTreeView( QQuickItem* const parent )
    : Inherited( parent )
{
    setSubcontrolProxy( Inherited::Cell, Cell );
    setSubcontrolProxy( Inherited::Text, Text );
}