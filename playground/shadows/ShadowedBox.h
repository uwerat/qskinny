#ifndef SHADOWED_BOX_H
#define SHADOWED_BOX_H

#include <QskControl.h>
#include <QskBoxShapeMetrics.h>

class QskGradient;

class ShadowedBox : public QskControl
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )

    class Shadow
    {
      public:
        qreal extent = 0.0;
        qreal xOffset = 0.0;
        qreal yOffset = 0.0;
    };

    ShadowedBox(QQuickItem* parent = nullptr);
    ~ShadowedBox() override;

    void setShadow( const Shadow& );
    const Shadow& shadow() const;

    void setGradient( const QskGradient& );
    const QskGradient& gradient() const;

    void setShadowColor( const QColor& );
    QColor shadowColor() const;

    void setShape( const QskBoxShapeMetrics& );
    const QskBoxShapeMetrics& shape() const;

  private:
    Shadow m_shadow;
    QColor m_shadowColor = Qt::black;
    QskGradient m_gradient;
    QskBoxShapeMetrics m_shape;
};

#endif
