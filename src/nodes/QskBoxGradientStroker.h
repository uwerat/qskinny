/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
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
