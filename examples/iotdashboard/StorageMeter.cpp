#include "StorageMeter.h"
#include "CircularProgressBar.h"
#include <QskTextLabel.h>

QSK_SUBCONTROL( StorageMeter, Fill )

StorageMeter::StorageMeter(QQuickItem *parent) noexcept
    : EnergyMeter(QColor{}, QskGradient{}, 0, parent)
{
  const auto gradient = gradientHint(StorageMeter::Fill);
  setGradientHint(StorageMeter::Fill, QskGradient(QskGradientStops{
    {0.0,Qt::green},
    {0.5,"darkorange"},
    {1.0,Qt::red}
  }));
}

qreal StorageMeter::progress() const noexcept
{
  if( auto* const bar = findChild<CircularProgressBar*>() )
  {
    return bar->value() / 100.0;
  }
  return 0.0;
}

void StorageMeter::setProgress(qreal progress) noexcept
{
  const auto value = qBound(0.0, progress, 1.0);

  const auto gradient = gradientHint(StorageMeter::Fill);

  const auto color = gradient.extracted( value, value ).startColor();

  if( auto* const bar = findChild<CircularProgressBar*>() )
  {
    bar->setGradientHint( CircularProgressBar::Bar, {color, color.darker(100)});
    bar->setValue(value * 100.0);
  }

  if ( auto* const label = findChild<QskTextLabel*>() )
  {
    label->setTextColor( color );
    label->setText(  locale().toString( static_cast<int>(value * 100.0) ) + " " + locale().percent()  );
  }
}
