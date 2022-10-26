#pragma once

#include <QskSlider.h>

class LinearGradientSlider : public QskSlider
{
    Q_OBJECT
    Q_PROPERTY(
        QColor selectedColor READ selectedColor WRITE setSelectedColor NOTIFY selectedColorChanged )
    using Inherited = QskSlider;

  public:
    explicit LinearGradientSlider( QQuickItem* parent = nullptr );
    explicit LinearGradientSlider( Qt::Orientation orientation, QQuickItem* parent = nullptr );
    const QColor& selectedColor() const;

  public Q_SLOTS:
    void setSelectedColor( const QColor& newSelectedColor );

  Q_SIGNALS:
    void selectedColorChanged();

  private:
    QColor m_selectedColor;
};
