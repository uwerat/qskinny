#include "Humidity.h"
#include "Skin.h"
#include "RoundedIcon.h"
#include "UpAndDownButton.h"

#include <QskBoxShapeMetrics.h>
#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskPushButton.h>
#include <QskTextLabel.h>

#include <QImage>

Humidity::Humidity( QQuickItem* parent )
    : Box( "", parent )
{
    setPanel( true );
    setPaddingHint( Panel, 8 );
    setSizePolicy( Qt::Vertical, QskSizePolicy::Maximum );

    auto* layout = new QskLinearBox( Qt::Horizontal, this );
    layout->setSpacing( 20 );

    QskGradient gradient( QskGradient::Vertical, "#6776FF", "#6100FF" );
    auto* icon = new RoundedIcon( "humidity", gradient, layout );
    icon->setMinimumWidth( 68 );
    icon->setFixedSize( 68, 68 ); // ### fix properly

    auto* titleAndValue = new QskLinearBox( Qt::Vertical, layout );

    auto* title = new QskTextLabel( "Humidity", titleAndValue );
    title->setFontRole( DaytimeSkin::TitleFont );

    auto* value = new QskTextLabel( "30%", titleAndValue );
    value->setFontRole( QskSkin::HugeFont );
    value->setTextColor( "#929CB2" );

    /*auto* buttons =*/ new UpAndDownButton( layout );
}
