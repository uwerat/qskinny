#include "OtherSkin.h"
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

#include <QDebug>

class Palette {
public:

    Palette()
    {
        // grey-blue-yellow:
        color1.setNamedColor( "#363636" ); // Jet
        color2.setNamedColor( "#242F40" ); // Yankees blue
        color3.setNamedColor( "#CCA43B" ); // Satin sheet gold
        color4.setNamedColor( "#E5E5E5" ); // Platinum
        color5.setNamedColor( "#FFFFFF" ); // white
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

OtherSkin::OtherSkin( const QString& name, QObject* parent ):
        QskSkin( parent ),
    m_name( name ),
    m_palette( new Palette )
{
    setObjectName( "OtherSkin" );
    initHints();
    initGraphicFilters();
}

OtherSkin::~OtherSkin()
{
    delete m_palette;
}

void OtherSkin::initHints()
{
    const int duration = 200; // for animators

    setFont( QskSkin::DefaultFont, qskFont( 13 ) );
    setFont( QskSkin::LargeFont, qskFont( 20 ) );

    setColor( QskTextLabel::Text, m_palette->color3 );

    setColor( SoundControl::CrossHair, QColor( "OliveDrab" ).lighter( 120 ) );
    setColor( SoundControl::Marker, "OliveDrab" );
    setMetric( SoundControl::Marker | QskAspect::Radius, 100 );
    setSkinHint( SoundControl::Marker
        | QskAspect::Radius | QskAspect::AllCorners
        | QskAspect::SizeMode, Qt::RelativeSize );

    setSkinHint( ButtonBar::Indicator | QskAspect::GraphicRole, SkinFactory::Indicator );
    setSkinHint( SoundControl::Vehicle | QskAspect::GraphicRole, SkinFactory::Vehicle );
    setSkinHint( SoundControl::MarkerControl | QskAspect::GraphicRole, SkinFactory::Vehicle );

    setColor( QskSeparator::Panel, m_palette->color3 );
    setMetric( QskSeparator::Panel, 1 );

    // -- push buttons

    setMetric( QskPushButton::Panel | QskAspect::Radius, 4 );
    setMetric( QskPushButton::Panel | QskAspect::Padding, 8 );
    setColor( QskPushButton::Panel, m_palette->color1 );
    setColor( QskPushButton::Text, m_palette->color3 );
    setColor( QskPushButton::Panel | QskPushButton::Pressed, m_palette->color2 );
    setAnimation( QskPushButton::Panel | QskAspect::Color, duration );

    QColor overlayColor( "#011627" );
    overlayColor.setAlpha( 200 );
    setColor( SoundControl::Overlay, overlayColor );
    setMetric( SoundControl::SliderControl | QskAspect::Radius, 4 );
    setColor( SoundControl::SliderControl, m_palette->color1 );
    setColor( SoundControl::SliderControl | QskPushButton::Pressed, m_palette->color2 );
    setAnimation( SoundControl::SliderControl | QskAspect::Color, duration );

    setMetric( QskPushButton::Text | QskAspect::Size, 20 );
    setSkinHint( QskPushButton::Text | QskAspect::FontRole, int( QskSkin::LargeFont ) );
    setSkinHint( QskPushButton::Text | QskAspect::Alignment, Qt::AlignCenter );

    // -- a more advanced setup of the hints for the slider

    setMetric( QskSlider::Panel | QskAspect::Size, 30 );
    setMetric( QskSlider::Groove | QskAspect::Size, 5 );
    setMetric( QskSlider::Fill | QskAspect::Size, 5 );
    setColor( QskSlider::Panel, Qt::transparent );
    setColor( QskSlider::Groove, m_palette->color4.darker( 200 ) );
    setColor( QskSlider::Fill, m_palette->color3.lighter( 150 ) );
    setMetric( QskSlider::Handle | QskAspect::Size, 22 );
    setMetric( QskSlider::Handle | QskAspect::Radius, 6 );
    setColor( QskSlider::Handle, m_palette->color3 );
}

void OtherSkin::initGraphicFilters()
{
    {
        // all SVGs on the header/footer are plain white

        QskColorFilter filter;
        filter.addColorSubstitution( Qt::white, m_palette->color3.rgb() );

        setGraphicFilter( SkinFactory::Indicator, filter );
    }

    {
        // we need to modify the SVG to have more colors for substutions !!
        QskColorFilter filter;
        filter.addColorSubstitution( Qt::white, m_palette->color3.rgb() );

        setGraphicFilter( SkinFactory::Vehicle, filter );
    }
}
