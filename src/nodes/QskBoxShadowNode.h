/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_BOX_SHADOW_NODE_H
#define QSK_BOX_SHADOW_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QColor;
class QskBoxShapeMetrics;

class QskBoxShadowNodePrivate;

class QSK_EXPORT QskBoxShadowNode : public QSGGeometryNode
{
  public:
    QskBoxShadowNode();
    ~QskBoxShadowNode() override;

    void setShadowData( const QRectF&, const QskBoxShapeMetrics&,
        qreal blurRadius, const QColor& );

  private:
    Q_DECLARE_PRIVATE( QskBoxShadowNode )
};

#endif
