/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskStateCombination.h"

static void qskRegisterStateCombination()
{
    qRegisterMetaType< QskStateCombination >();
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterStateCombination )

