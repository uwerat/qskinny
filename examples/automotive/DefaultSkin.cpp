#include "DefaultSkin.h"

#include "SkinFactory.h"
#include "SoundControl.h"
#include "ButtonBar.h"

#include <QskBox.h>
#include <QskFunctions.h>
#include <QskPushButton.h>
#include <QskSkinTransition.h>
#include <QskSlider.h>
#include <QskTextLabel.h>
#include <QskSeparator.h>
#include <QskColorFilter.h>
#include <QskMargins.h>

#include <QDebug>

class Transition : public QskSkinTransition
{
protected:
    virtual void updateSkin( QskSkin*, QskSkin* newSkin ) override final
    {
        DefaultSkin* skin = static_cast< DefaultSkin* >( newSkin );
        skin->resetColors();
    }
};

class Palette
{
public:

    Palette( DefaultSkin::Scheme scheme = DefaultSkin::Daylight )
    {
        if ( scheme == DefaultSkin::Daylight )
        {
            // colorful:
            color1.setNamedColor( "#011627" ); // Maastricht blue
            color2.setNamedColor( "#FF0022" ); // ruddy
            color3.setNamedColor( "#41EAD4" ); // Turquoise
            color4.setNamedColor( "#FDFFFC" ); // baby powder
            color5.setNamedColor( "#B91372" ); // red violet
        }
        else
        {
            // greenish:
            color4.setNamedColor( "#FAF3DD" ); // Beige
            color2.setNamedColor( "#C8D5B9" ); // Pale Silver
            color3.setNamedColor( "#8FC0A9" ); // Eton Blue
            color1.setNamedColor( "#68B0AB" ); // Green Sheen
            color5.setNamedColor( "#4A7C59" ); // Amazon
        }
    }

    QColor color1;
    QColor color2;
    QColor color3;
    QColor color4;
    QColor color5;

};

namespace {

    static inline QFont qskFont( int pointSize )
    {
        QFont font( "Roboto" );
        font.setPointSize( pointSize / qskDpiScaled( 1.0 ) );
        return font;
    }

}

DefaultSkin::DefaultSkin( const QString& name, QObject* parent ):
    QskSkin( parent ),
    m_name( name ),
    m_palette( new Palette ),
    m_scheme( Daylight )
{
    setObjectName( "DefaultSkin" );
    initHints();
}

DefaultSkin::~DefaultSkin()
{
    delete m_palette;
}

void DefaultSkin::initHints()
{
    const int duration = 200; // for animators

    setFont( QskSkin::DefaultFont, qskFont( 13 ) );
    setFont( QskSkin::LargeFont, qskFont( 20 ) );

    setColor( QskTextLabel::Text, m_palette->color4 );

    setColor( SoundControl::Overlay, 0 );
    setColor( SoundControl::CrossHair, m_palette->color3 );
    setColor( SoundControl::Marker, m_palette->color5 );

    // a circle
    setMetric( SoundControl::Marker | QskAspect::Radius, 100 );
    setSkinHint( SoundControl::Marker
        | QskAspect::Radius | QskAspect::AllCorners
        | QskAspect::SizeMode, Qt::RelativeSize );

    setColor( QskSeparator::Panel, m_palette->color3 );
    setMetric( QskSeparator::Panel, 2 );

    // -- push buttons

    setMargins( QskPushButton::Panel | QskAspect::Padding, 10 );
    setColor( QskPushButton::Panel, m_palette->color1 );
    setColor( QskPushButton::Text, m_palette->color3 );
    setColor( QskPushButton::Panel | QskPushButton::Pressed, m_palette->color2 );
    setAnimation( QskPushButton::Panel | QskAspect::Color, duration );

    setColor( SoundControl::SliderControl, m_palette->color1 );
    setColor( SoundControl::SliderControl | QskPushButton::Pressed, m_palette->color2 );
    setAnimation( SoundControl::SliderControl | QskAspect::Color, duration );

    setMetric( QskPushButton::Text | QskAspect::Size, 20 );
    setSkinHint( QskPushButton::Text | QskAspect::FontRole, int( QskSkin::LargeFont ) );
    setSkinHint( QskPushButton::Text | QskAspect::Alignment, Qt::AlignCenter );

    // -- a more advanced setup of the hints for the slider

    const qreal dim = 30;

    setMetric( QskSlider::Panel | QskAspect::Size, dim );
    setMetric( QskSlider::Groove | QskAspect::Size, 2 );
    setMetric( QskSlider::Fill | QskAspect::Size, 2 );
    setColor( QskSlider::Panel, Qt::transparent );
    setColor( QskSlider::Groove, m_palette->color4 );
    setColor( QskSlider::Fill, m_palette->color4.darker( 200 ) );
    setMetric( QskSlider::Handle | QskAspect::Size, 24 );
    setMetric( QskSlider::Handle | QskAspect::Radius, 12 );
    setColor( QskSlider::Handle, m_palette->color5 );

    // handle expanding, when being pressed
    for ( auto state : { QskAspect::NoState, QskSlider::Pressed } )
    {
        using namespace QskAspect;

        const Aspect aspect = QskSlider::Handle | state;

        // fullsize, only when being pressed
        const qreal sz = ( state == NoState ) ? 0.7 * dim : dim;

        setMetric( aspect | Size, sz );
        setMetric( aspect | Radius, 0.5 * sz ); // a round handle

        setAnimation( aspect | Size | Metric, duration );
        setAnimation( aspect | Radius | Metric, 200 );

        // move the handle smoothly, when using keys
        setAnimation( aspect | Metric | Position,
            ( state == NoState ) ? 2 * duration : 0 );
    }

    // animator for color scheme transitions
    setAnimation( QskAspect::Color, QskAnimationHint( 1000, QEasingCurve::InQuad ) );
}

void DefaultSkin::toggleScheme()
{
    if ( m_scheme == Daylight )
        m_scheme = Nighttime;
    else
        m_scheme = Daylight;

    Transition transition;

    transition.setSourceSkin( this );
    transition.setTargetSkin( this );
    transition.setMask( QskSkinTransition::Color );
    transition.setAnimation( animation( QskAspect::Color ) );

    transition.process();
}

void DefaultSkin::resetColors()
{
    delete m_palette;
    m_palette = new Palette( m_scheme );
    initHints();
}

