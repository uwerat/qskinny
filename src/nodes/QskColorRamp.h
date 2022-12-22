/**********************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_COLOR_RAMP_H
#define QSK_COLOR_RAMP_H

#include "QskGlobal.h"
#include "QskGradient.h"

class QSGTexture;

namespace QskColorRamp
{
    QSGTexture* texture( const void* rhi,
        const QskGradientStops&, QskGradient::SpreadMode );
}

#endif
