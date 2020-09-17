#include "Humidity.h"
#include "DaytimeSkin.h"
#include "RoundedIcon.h"

#include <QskBoxShapeMetrics.h>
#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskPushButton.h>
#include <QskTextLabel.h>

#include <QImage>

Humidity::Humidity( QQuickItem* parent )
    : Box( "", parent )
{
    setMarginsHint( Panel | QskAspect::Padding, 10 );

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

    auto* buttons = new QskLinearBox( Qt::Vertical, layout );
    buttons->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
    buttons->setSpacing( 0 );

    QImage upImage( ":/images/up.png" );
    auto upGraphic = QskGraphic::fromImage( upImage );
    upGraphic.setDefaultSize( {10, 5.71} );
    auto* upButton = new QskPushButton( buttons );
    upButton->setBoxShapeHint( QskPushButton::Panel, {30, 30, 0, 0} );
    upButton->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Expanding );
    upButton->setFixedWidth( 32 );
    upButton->setGradientHint( QskPushButton::Panel, {"#f7f7f7"} );
    upButton->setGraphic( upGraphic );
    upButton->setGraphicSourceSize( {10, 5.71} );

    QImage downImage( ":/images/down.png" );
    auto downGraphic = QskGraphic::fromImage( downImage );
    downGraphic.setDefaultSize( {10, 5.71} );
    auto* downButton = new QskPushButton( buttons );
    downButton->setBoxShapeHint( QskPushButton::Panel, {0, 0, 30, 30} );
    downButton->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Expanding );
    downButton->setFixedWidth( 32 );
    downButton->setGradientHint( QskPushButton::Panel, {"#f7f7f7"} );
    downButton->setGraphic( downGraphic );
    downButton->setGraphicSourceSize( {10, 5.71} );
}
