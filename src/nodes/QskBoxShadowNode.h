/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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
