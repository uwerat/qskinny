#include "DefaultSkin.h"

#include "SoundControl.h"

#include <QskBox.h>
#include <QskFunctions.h>
#include <QskPushButton.h>
#include <QskSkinTransition.h>
#include <QskSlider.h>
#include <QskTextLabel.h>

#include <QDebug>

class DefaultSkin::Transition : public QskSkinTransition
{
protected:
    virtual void updateSkin( QskSkin*, QskSkin* newSkin ) override final
    {
        DefaultSkin* skin = static_cast< DefaultSkin* >( newSkin );
        skin->resetColors();
    }
};

class Palette {
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

DefaultSkin::DefaultSkin( const QString& name, QObject* parent ) :
        QskSkin( parent ),
    m_name( name ),
    m_palette( new Palette ),
    m_scheme( Daylight )
{
    setObjectName( "DefaultSkin" );
    initHints();
}

void DefaultSkin::initHints()
{
    setFont( QskSkin::DefaultFont, qskFont( 13 ) );
    setFont( QskSkin::LargeFont, qskFont( 20 ) );

    setColor( QskTextLabel::Text, m_palette->color4 );

    setColor( FilledRectangle::Panel, m_palette->color3 );

    setColor( QskPushButton::Panel, m_palette->color1 );
    setColor( QskPushButton::Text, m_palette->color3 );
    setColor( QskPushButton::Panel | QskPushButton::Pressed, m_palette->color2 );
    setMetric( QskPushButton::Text | QskAspect::Size, 20 );
    setSkinHint( QskPushButton::Text | QskAspect::FontRole, int( QskSkin::LargeFont ) );
    setSkinHint( QskPushButton::Text | QskAspect::Alignment, Qt::AlignCenter );

    setMetric( QskSlider::Panel | QskAspect::Size, 5 );
    setColor( QskSlider::Groove, m_palette->color4 );
    setColor( QskSlider::Fill, m_palette->color4.darker( 200 ) );
    setMetric( QskSlider::Handle | QskAspect::Size, 18 );
    setMetric( QskSlider::Handle | QskAspect::Radius, 9 );
    setColor( QskSlider::Handle, m_palette->color5 );

    setColor( BalanceFadeBox::Panel, m_palette->color5 );
    setMetric( BalanceFadeBox::Panel | QskAspect::Radius, 15 );

    // animator for daylight/night scheme transitions
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

DefaultSkin::~DefaultSkin()
{
    delete m_palette;
}
