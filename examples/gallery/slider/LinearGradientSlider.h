#pragma once

#include <QskSlider.h>

class LinearGradientSlider : public QskSlider
{
    Q_OBJECT
    Q_PROPERTY(
        QColor selectedColor READ selectedColor NOTIFY selectedColorChanged )
    using Inherited = QskSlider;

  public:
    explicit LinearGradientSlider( QQuickItem* parent = nullptr );
    explicit LinearGradientSlider( Qt::Orientation orientation, QQuickItem* parent = nullptr );
    const QColor& selectedColor() const;

  Q_SIGNALS:
    void selectedColorChanged();

  private:
    QColor m_selectedColor;
};
