/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_GRADUATION_H
#define QSK_GRADUATION_H

#include <QskGlobal.h>

class QskTickmarks;

namespace QskGraduation
{
    QSK_EXPORT QskTickmarks divideInterval( qreal x1, qreal x2,
        int maxMajorSteps, int maxMinorSteps, qreal stepSize = 0.0 );

    QSK_EXPORT qreal stepSize( double length, int numSteps );
}

#endif
