#pragma once

#include <QSGGeometryNode>
#include "BlurredBoxTextureProvider.h"

class QskBoxShapeMetrics;
class BlurredBoxNodePrivate;

class BlurredBoxNode final : public QSGGeometryNode
{
  public:
    BlurredBoxNode();

    void preprocess() override;

    void setBlurData( const QRectF&, const QskBoxShapeMetrics&, const QRectF& rectOnScreen, float opacity, float blurDirections, float blurQuality,
        float blurSize , BlurredBoxTextureProvider* textureProvider);

  private:
    Q_DECLARE_PRIVATE( BlurredBoxNode )
};
