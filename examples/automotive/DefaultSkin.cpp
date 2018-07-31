#include "DefaultSkin.h"

#include "SkinFactory.h"
#include "SoundControl.h"
#include "ButtonBar.h"

#include "Speedometer.h"
#include "SpeedometerSkinlet.h"

#include <QskBox.h>
#include <QskFunctions.h>
#include <QskPushButton.h>
#include <QskSkinTransition.h>
#include <QskSlider.h>
#include <QskTextLabel.h>
#include <QskSeparator.h>
#include <QskColorFilter.h>
#include <QskMargins.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxBorderColors.h>

#include <QDebug>

class Transition final : public QskSkinTransition
{
protected:
    void updateSkin( QskSkin*, QskSkin* newSkin ) override
    {
        DefaultSkin* skin = static_cast< DefaultSkin* >( newSkin );
        skin->resetHints();
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

    static inline QFont qskFont( qreal pointSize )
    {
        QFont font( "Roboto" );
        font.setPointSizeF( pointSize / qskDpiScaled( 1.0 ) );
        return font;
    }

} // namespace

DefaultSkin::DefaultSkin( const QString& name, QObject* parent ):
    QskSkin( parent ),
    m_name( name ),
    m_palette( new Palette ),
    m_scheme( Daylight )
{
    setObjectName( "DefaultSkin" );
    declareSkinlet< Speedometer, SpeedometerSkinlet >();
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

    // -- sound control

    setGradient( SoundControl::Overlay, 0 );
    setGradient( SoundControl::CrossHair, m_palette->color3 );
    setGradient( SoundControl::Marker, m_palette->color5 );

    setBoxBorderMetrics( SoundControl::Marker, 2 );
    setBoxShape( SoundControl::Marker, 100, Qt::RelativeSize );

    setGradient( QskSeparator::Panel, m_palette->color3 );
    setMetric( QskSeparator::Panel | QskAspect::Size, 2 );

    // -- push buttons

    setMargins( QskPushButton::Panel | QskAspect::Padding, 10 );
    setGradient( QskPushButton::Panel, m_palette->color1 );
    setColor( QskPushButton::Text, m_palette->color3 );
    setGradient( QskPushButton::Panel | QskPushButton::Pressed, m_palette->color2 );
    setAnimation( QskPushButton::Panel | QskAspect::Color, duration );

    setGradient( SoundControl::SliderControl, m_palette->color1 );
    setGradient( SoundControl::SliderControl | QskPushButton::Pressed, m_palette->color2 );
    setAnimation( SoundControl::SliderControl | QskAspect::Color, duration );

    setMetric( QskPushButton::Text | QskAspect::Size, 20 );
    setSkinHint( QskPushButton::Text | QskAspect::FontRole, int( QskSkin::LargeFont ) );
    setSkinHint( QskPushButton::Text | QskAspect::Alignment, Qt::AlignCenter );

    // -- a more advanced setup of the hints for the slider

    const qreal dim = 30;

    setMetric( QskSlider::Panel | QskAspect::Size, dim );
    setGradient( QskSlider::Panel, QskGradient() );

    setMetric( QskSlider::Groove | QskAspect::Size, 2 );
    setGradient( QskSlider::Groove, m_palette->color4 );

    setMetric( QskSlider::Fill | QskAspect::Size, 2 );
    setGradient( QskSlider::Fill, m_palette->color4.darker( 200 ) );

    setMetric( QskSlider::Handle | QskAspect::Size, 24 );
    setBoxBorderMetrics( QskSlider::Handle, 0 );
    setBoxShape( QskSlider::Handle, 100.0, Qt::RelativeSize );
    setGradient( QskSlider::Handle, m_palette->color5 );

    // -- speedometers

    setBoxBorderMetrics( Speedometer::Panel, 2 );
    setGradient( Speedometer::Panel, m_palette->color1 );
    setBoxBorderColors( Speedometer::Panel, m_palette->color3 );

    setBoxBorderMetrics( Speedometer::NeedleHead, 2 );
    setMetric( Speedometer::NeedleHead | QskAspect::Size, 15 );
    setGradient( Speedometer::NeedleHead, QskGradient( QskGradient::Diagonal,
                                                       m_palette->color2, m_palette->color1 ) );
//    setBoxBorderColors( Speedometer::NeedleHead, m_palette->color4 );

    setMetric( Speedometer::Needle | QskAspect::MinimumWidth, 2 );
    setMetric( Speedometer::Needle | QskAspect::Margin, 10 );
    setColor( Speedometer::Needle, m_palette->color2 );

    // margins between numbers and ticks:
    setMargins( Speedometer::Labels, QskMargins( 4, 4, 4, 4 ) );
    setMetric( Speedometer::Labels | QskAspect::MinimumWidth, 2 );
    setMetric( Speedometer::Labels | QskAspect::Size, 15 ); // ticks size
    setColor( Speedometer::Labels, m_palette->color4 );
    setFontRole( Speedometer::Labels, QskSkin::SmallFont );


    // handle expanding, when being pressed
    for ( auto state : { QskAspect::NoState, QskSlider::Pressed } )
    {
        using namespace QskAspect;

        const Aspect aspect = QskSlider::Handle | state;

        // fullsize, only when being pressed
        const qreal sz = ( state == NoState ) ? 0.7 * dim : dim;

        setMetric( aspect | Size, sz );

        // move the handle smoothly, when using keys
        setAnimation( aspect | Metric | Position,
            ( state == NoState ) ? 2 * duration : 0 );
    }

    setAnimation( QskSlider::Handle | QskAspect::Metric, duration );

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

void DefaultSkin::resetHints()
{
    delete m_palette;
    m_palette = new Palette( m_scheme );
    initHints();
}

