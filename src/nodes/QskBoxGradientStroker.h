/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_GRADIENT_STROKER_H
#define QSK_BOX_GRADIENT_STROKER_H

#include "QskGradient.h"
#include "QskGradientDirection.h"

class QskBoxShapeMetrics;
class QskBoxBorderMetrics;
class QskBoxMetrics;

namespace QskVertex { class ColoredLine; }

class QskBoxGradientStroker
{
  public:
    QskBoxGradientStroker( const QskBoxMetrics&, const QskGradient& );

    int lineCount() const;
    void setLines( int lineCount, QskVertex::ColoredLine* );

  private:
    void setFillLines( int lineCount, QskVertex::ColoredLine* );

    const QskBoxMetrics& m_metrics;
    const QskGradient m_gradient;
    const QskLinearDirection m_dir;
};

#endif
