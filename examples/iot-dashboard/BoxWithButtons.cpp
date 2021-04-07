#include "BoxWithButtons.h"

#include "RoundedIcon.h"
#include "Skin.h"
#include "UpAndDownButton.h"

#include <QskTextLabel.h>

QSK_SUBCONTROL( BoxWithButtons, Panel )

QSK_SUBCONTROL( IndoorTemperature, Panel )
QSK_SUBCONTROL( Humidity, Panel )

BoxWithButtons::BoxWithButtons( const QString& title, bool isBright, QQuickItem* parent )
    : Box( "", parent )
{
    setPanel( true );
    setSizePolicy( Qt::Vertical, QskSizePolicy::Maximum );

    auto* layout = new QskLinearBox( Qt::Horizontal, this );
    layout->setSpacing( 20 );

    QString iconFile = title.toLower();
    iconFile = iconFile.replace( ' ', '-' );
    new RoundedIcon( iconFile, isBright, false, layout );

    auto* titleAndValue = new QskLinearBox( Qt::Vertical, layout );
    titleAndValue->setMargins( {0, 10, 0, 0} );

    auto* titleLabel = new QskTextLabel( title, titleAndValue );
    titleLabel->setFontRole( Skin::TitleFont );

    auto* value = new QskTextLabel( "+24", titleAndValue );
    value->setFontRole( QskSkin::HugeFont );
    value->setTextColor( "#929CB2" );

    new UpAndDownButton( layout );
}
