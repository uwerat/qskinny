/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_GRADUATION_H
#define QSK_GRADUATION_H

#include <QskGlobal.h>
#include <qmetatype.h>

class QskTickmarks;

namespace QskGraduation
{
    Q_NAMESPACE_EXPORT( QSK_EXPORT )

    QskTickmarks divideInterval( qreal x1, qreal x2,
        int maxMajorSteps, int maxMinorSteps, qreal stepSize = 0.0 );

    qreal stepSize( double length, int numSteps );
}

#endif
