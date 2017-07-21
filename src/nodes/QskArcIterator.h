/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ARC_ITERATOR_H
#define QSK_ARC_ITERATOR_H

#include "QskGlobal.h"
#include <QtMath>

class QSK_EXPORT QskArcIterator
{
public:
    QskArcIterator( double angle, int stepCount, bool inverted = false );

    inline double cos() const
    {
        return m_cos;
    }

    inline double sin() const
    {
        return m_inverted ? -m_sin : m_sin;
    }

    inline int step() const
    {
        return m_stepIndex;
    }

    inline bool isDone() const
    {
        return m_stepIndex > m_stepCount;
    }

    inline void operator++()
    {
        const double cos0 = m_cos;

        m_cos = m_cos * m_cosStep + m_sin * m_sinStep;
        m_sin = m_sin * m_cosStep - cos0 * m_sinStep;

        ++m_stepIndex;
    }

    static int segmentHint( double radius );

private:
    double m_cos;
    double m_sin;

    int m_stepIndex;
    double m_cosStep;
    double m_sinStep;

    const int m_stepCount;
    const bool m_inverted;
};

#endif
