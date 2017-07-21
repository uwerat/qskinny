/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskArcIterator.h"
#include <QtMath>

QskArcIterator::QskArcIterator( double angle, int stepCount, bool inverted ):
    m_cos( inverted ? 1.0 : 0.0 ),
    m_sin( inverted ? 0.0 : 1.0 ),
    m_stepIndex( 0 ),
    m_stepCount( stepCount ),
    m_inverted( inverted )
{
    const double angleStep = angle / stepCount;
    m_cosStep = qFastCos( angleStep );
    m_sinStep = qFastSin( angleStep );
}

int QskArcIterator::segmentHint( double radius )
{
    const double arcLength = radius * M_PI_2;
    return qBound( 3, qCeil( arcLength / 3.0 ), 18 ); // every 3 pixels
}
