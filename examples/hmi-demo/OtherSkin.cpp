#include "OtherSkin.h"

#include "SoundControl.h"

#include <QskFunctions.h>
#include <QskPushButton.h>
#include <QskSkinTransition.h>
#include <QskSlider.h>
#include <QskTextLabel.h>

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

OtherSkin::OtherSkin( const QString& name, QObject* parent ) :
        QskSkin( parent ),
    m_name( name ),
    m_palette( new Palette )
{
    setObjectName( "OtherSkin" );
    initHints();
}

void OtherSkin::initHints()
{
    setFont( QskSkin::DefaultFont, qskFont( 13 ) );
    setFont( QskSkin::LargeFont, qskFont( 20 ) );

    setColor( QskTextLabel::Text, m_palette->color4 );

    setColor( FilledRectangle::Panel, m_palette->color3 );

    setColor( QskPushButton::Panel, m_palette->color1 );
    setColor( QskPushButton::Text, m_palette->color3 );
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
}

OtherSkin::~OtherSkin()
{
    delete m_palette;
}
