#ifndef BOX_SHADOW_NODE_H
#define BOX_SHADOW_NODE_H

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
    void setShadow( qreal extent, qreal dx, qreal dy  );

    void updateGeometry();

  private:
    Q_DECLARE_PRIVATE( BoxShadowNode )
};

#endif
