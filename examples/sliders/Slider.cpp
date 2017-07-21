#include "Slider.h"
#include "SliderSkinlet.h"

#include <QskAspect.h>
#include <QskRgbValue.h>
#include <QskAnimationHint.h>

QSK_SUBCONTROL( Slider, Scale )
QSK_SUBCONTROL( Slider, Decoration )

Slider::Slider( QQuickItem* parentItem ):
    QskSlider( parentItem )
{
    using namespace QskAspect;

    setMetric( QskSlider::Handle, 80 );

    setColor( Fill | Background, QskRgbValue::Grey700 );
    setColor( Scale | Border, qRgb( 178, 178, 178 ) ); // for the ticks

    setColor( QskSlider::Handle, QskRgbValue::Grey800 );

    setAnimation( QskSlider::Handle | Color | NoState, 1000 );

    for ( auto state : { Pressed, Focused | Hovered, Hovered, Focused } )
    {
        // some extra animations
        const Aspect aspect = QskSlider::Handle | Color | state;

        setColor( aspect, QskRgbValue::Orange600 );
        setAnimation( aspect, 300 );
    }

    // using an individual skinlet, not known by the skin

    auto skinlet = new SliderSkinlet();
    skinlet->setOwnedBySkinnable( true );

    setSkinlet( skinlet );
}

Slider::~Slider() = default;

QSizeF Slider::contentsSizeHint() const
{
    const qreal extra = 40;
    return QskSlider::contentsSizeHint() + QSizeF( 0, extra );
}

