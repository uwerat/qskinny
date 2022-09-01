#pragma once

#include <QSGGeometryNode>

class QColor;
class QskBoxShapeMetrics;

class BlurredBoxNodePrivate;

class BlurredBoxNode : public QSGGeometryNode
{
  public:
    BlurredBoxNode();

    void setBlurData( const QRectF&, const QskBoxShapeMetrics&, const QRectF& rectOfScreen,
        const QRectF& rectOnScreen, float opacity, float blurDirections, float blurQuality,
        float blurSize );

  private:
    Q_DECLARE_PRIVATE( BlurredBoxNode )
};
