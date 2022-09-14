#pragma once

#include <QskBox.h>

class QskGradient;
class QSGTexture;

class BlurredBox final : public QskBox
{
    Q_OBJECT
    Q_PROPERTY( float blurDirections READ blurDirections WRITE setBlurDirections NOTIFY
            blurDirectionsChanged )
    Q_PROPERTY( float blurQuality READ blurQuality WRITE setBlurQuality NOTIFY blurQualityChanged )
    Q_PROPERTY( float blurSize READ blurSize WRITE setBlurSize NOTIFY blurSizeChanged )
    using Inherited = QskBox;

  public:
    QSK_SUBCONTROLS( Panel )

    BlurredBox( QQuickItem* parent = nullptr );
    ~BlurredBox() override;

    void geometryChangeEvent( QskGeometryChangeEvent* ) override;

    float blurDirections() const;
    void setBlurDirections( float newBlurDirections );

    float blurQuality() const;
    void setBlurQuality( float newBlurQuality );

    float blurSize() const;
    void setBlurSize( float newBlurSize );

  Q_SIGNALS:
    void blurDirectionsChanged( float );
    void blurQualityChanged( float );
    void blurSizeChanged( float );

  private:
    float m_blurDirections = 32.0;
    float m_blurQuality = 8.0;
    float m_blurSize = 8.0;
};
