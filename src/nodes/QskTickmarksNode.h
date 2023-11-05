/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TICKMARKS_NODE_H
#define QSK_TICKMARKS_NODE_H

#include "QskGlobal.h"

#include <qsgnode.h>
#include <qnamespace.h>

class QColor;
class QRectF;
class QskIntervalF;
class QskScaleTickmarks;

class QskTickmarksNodePrivate;
class QskTickmarksMetrics;

class QSK_EXPORT QskTickmarksNode : public QSGGeometryNode
{
  public:
    QskTickmarksNode();
    ~QskTickmarksNode() override;

    void update(const QColor&, const QRectF&, const QskIntervalF&,
        const QskScaleTickmarks&, int tickLineWidth, Qt::Orientation,
        Qt::Alignment, const QskTickmarksMetrics& metrics );

  private:
    Q_DECLARE_PRIVATE( QskTickmarksNode )
};

#endif
