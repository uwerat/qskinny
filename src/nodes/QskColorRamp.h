/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
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
