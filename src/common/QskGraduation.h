/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_GRADUATION_H
#define QSK_GRADUATION_H

#include <QskGlobal.h>
#include <qmetatype.h>

class QskScaleTickmarks;

namespace QskGraduation
{
    Q_NAMESPACE_EXPORT( QSK_EXPORT )

    enum Attribute
    {
        Inverted = 1 << 0,
        Floating = 1 << 1
    };

    Q_ENUM_NS( Attribute )

    Q_DECLARE_FLAGS( Attributes, Attribute )
    Q_DECLARE_OPERATORS_FOR_FLAGS( Attributes )

    QskScaleTickmarks divideInterval( qreal x1, qreal x2,
        int maxMajorSteps, int maxMinorSteps, qreal stepSize = 0.0 );

    void calculate( Attributes, int maxNumSteps,
        qreal& x1, qreal& x2, qreal& stepSize );

    qreal alignedStepSize( double intervalSize, int numSteps );
}

#endif
