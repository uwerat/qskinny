#include "OtherSkin.h"
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
#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>

#include <QDebug>

class Palette
{
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

    static inline QFont qskFont( qreal pointSize )
    {
        QFont font( "Roboto" );
        font.setPointSizeF( pointSize / qskDpiScaled( 1.0 ) );
        return font;
    }

}

OtherSkin::OtherSkin( const QString& name, QObject* parent ):
        QskSkin( parent ),
    m_name( name ),
    m_palette( new Palette )
{
    setObjectName( "OtherSkin" );
    declareSkinlet< Speedometer, SpeedometerSkinlet >();
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

    // -- sound control
    setGradient( SoundControl::CrossHair, QColor( "OliveDrab" ).lighter( 120 ) );
    setGradient( SoundControl::Marker, QColor( "OliveDrab" ) );

    setBoxBorderMetrics( SoundControl::Marker, 2 );
    setBoxShape( SoundControl::Marker, 100, Qt::RelativeSize );

    setSkinHint( ButtonBar::Indicator | QskAspect::GraphicRole, SkinFactory::Indicator );
    setSkinHint( SoundControl::Vehicle | QskAspect::GraphicRole, SkinFactory::Vehicle );
    setSkinHint( SoundControl::MarkerControl | QskAspect::GraphicRole, SkinFactory::Vehicle );

    setBoxBorderMetrics( QskSeparator::Panel, 0 );
    setGradient( QskSeparator::Panel, m_palette->color3 );
    setMetric( QskSeparator::Panel | QskAspect::Size, 1 );

    // -- push buttons

    setMargins( QskPushButton::Panel | QskAspect::Padding, 8 );
    setBoxBorderMetrics( QskPushButton::Panel, 0 );
    setBoxShape( QskPushButton::Panel, 4 );

    setGradient( QskPushButton::Panel, m_palette->color1 );
    setGradient( QskPushButton::Panel | QskPushButton::Pressed, m_palette->color2 );

    setColor( QskPushButton::Text, m_palette->color3 );
    setAnimation( QskPushButton::Panel | QskAspect::Color, duration );

    // -- overlay

    QColor overlayColor( "#011627" );
    overlayColor.setAlpha( 200 );
    setGradient( SoundControl::Overlay, overlayColor );

    // -- slider control buttons
    setBoxBorderMetrics( SoundControl::SliderControl, 0 ); 
    setBoxShape( SoundControl::SliderControl, 4 ); 

    setGradient( SoundControl::SliderControl, m_palette->color1 );
    setGradient( SoundControl::SliderControl | QskPushButton::Pressed, m_palette->color2 );

    setAnimation( SoundControl::SliderControl | QskAspect::Color, duration );

    setMetric( QskPushButton::Text | QskAspect::Size, 20 );
    setSkinHint( QskPushButton::Text | QskAspect::FontRole, int( QskSkin::LargeFont ) );
    setSkinHint( QskPushButton::Text | QskAspect::Alignment, Qt::AlignCenter );

    // -- a more advanced setup of the hints for the slider

    setMetric( QskSlider::Panel | QskAspect::Size, 30 );
    setGradient( QskSlider::Panel, QskGradient() );

    setMetric( QskSlider::Groove | QskAspect::Size, 5 );
    setGradient( QskSlider::Groove, m_palette->color4.darker( 200 ) );

    setMetric( QskSlider::Fill | QskAspect::Size, 5 );
    setGradient( QskSlider::Fill, m_palette->color3.lighter( 150 ) );

    setMetric( QskSlider::Handle | QskAspect::Size, 22 );
    setBoxBorderMetrics( QskSlider::Handle, 0 );
    setBoxShape( QskSlider::Handle, 6 );
    setGradient( QskSlider::Handle, m_palette->color3 );
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
