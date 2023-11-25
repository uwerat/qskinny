/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TICKMARKS_NODE_H
#define QSK_TICKMARKS_NODE_H

#include "QskBasicLinesNode.h"

class QskIntervalF;
class QskScaleTickmarks;
class QskGraduationMetrics;
class QRectF;

class QSK_EXPORT QskTickmarksNode : public QskBasicLinesNode
{
  public:
    QskTickmarksNode();
    ~QskTickmarksNode() override;

    void update(const QColor&, const QRectF&, const QskIntervalF&,
        const QskScaleTickmarks&, int tickLineWidth, Qt::Orientation,
        Qt::Alignment, const QskGraduationMetrics& );

  private:
    QskHashValue m_hash = 0;
};

#endif
