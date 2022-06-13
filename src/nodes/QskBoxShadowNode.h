/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_SHADOW_NODE_H
#define QSK_BOX_SHADOW_NODE_H

#include "QskGlobal.h"
#include <QSGGeometryNode>

class QColor;
class QskBoxShapeMetrics;

class QskBoxShadowNodePrivate;

class QSK_EXPORT QskBoxShadowNode : public QSGGeometryNode
{
  public:
    QskBoxShadowNode();
    ~QskBoxShadowNode() override;

    void setRect( const QRectF& );
    void setShape( const QskBoxShapeMetrics& );
    void setColor( const QColor& );
    void setBlurRadius( qreal );

    void setClipShape( const QskBoxShapeMetrics& );

    void updateGeometry();

  private:
    Q_DECLARE_PRIVATE( QskBoxShadowNode )
};

#endif
