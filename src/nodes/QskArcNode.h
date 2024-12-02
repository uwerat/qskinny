/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ARC_NODE_H
#define QSK_ARC_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QskArcMetrics;
class QskGradient;

class QSK_EXPORT QskArcNode : public QSGNode
{
  public:
    QskArcNode();
    ~QskArcNode() override;

    void setArcData( const QRectF&, const QskArcMetrics&, const QskGradient& );

    void setArcData( const QRectF&, const QskArcMetrics&,
        qreal borderWidth, const QColor& borderColor, const QskGradient& );
};

#endif
