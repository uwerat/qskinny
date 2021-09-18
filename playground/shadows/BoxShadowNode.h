/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QSGGeometryNode>

class QColor;
class QskBoxShapeMetrics;

class BoxShadowNodePrivate;

class BoxShadowNode : public QSGGeometryNode
{
  public:
    BoxShadowNode();
    ~BoxShadowNode() override;

    void setRect( const QRectF& );
    void setShape( const QskBoxShapeMetrics& );
    void setColor( const QColor& );
    void setBlurRadius( qreal );

    void setClipRect( const QRectF& );

    void updateGeometry();

  private:
    Q_DECLARE_PRIVATE( BoxShadowNode )
};
