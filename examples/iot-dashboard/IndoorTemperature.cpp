#include "IndoorTemperature.h"
#include "DaytimeSkin.h"
#include "RoundedIcon.h"
#include "UpAndDownButton.h"

#include <QskBoxShapeMetrics.h>
#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskPushButton.h>
#include <QskTextLabel.h>

#include <QImage>

IndoorTemperature::IndoorTemperature( QQuickItem* parent )
    : Box( "", parent )
{
    setPanel( true );
    setMarginHint( Panel | QskAspect::Padding, 8 );
    setSizePolicy( Qt::Vertical, QskSizePolicy::Maximum );

    auto* layout = new QskLinearBox( Qt::Horizontal, this );
    layout->setSpacing( 20 );

    QskGradient gradient( QskGradient::Vertical, "#ff7d34", "#ff3122" );
    auto* icon = new RoundedIcon( "indoor-temperature", gradient, layout );
    icon->setMinimumWidth( 68 );
    icon->setFixedSize( 68, 68 ); // ### fix properly

    auto* titleAndValue = new QskLinearBox( Qt::Vertical, layout );
    titleAndValue->setMargins( {0, 10, 0, 0} );

    auto* title = new QskTextLabel( "Indoor Temperature", titleAndValue );
    title->setFontRole( DaytimeSkin::TitleFont );

    auto* value = new QskTextLabel( "+24", titleAndValue );
    value->setFontRole( QskSkin::HugeFont );
    value->setTextColor( "#929CB2" );

    auto* buttons = new UpAndDownButton( layout );
    Q_UNUSED( buttons );
}
