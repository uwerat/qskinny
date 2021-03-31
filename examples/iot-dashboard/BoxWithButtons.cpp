#include "BoxWithButtons.h"

#include "RoundedIcon.h"
#include "Skin.h"
#include "UpAndDownButton.h"

#include <QskTextLabel.h>

QSK_SUBCONTROL( BigRoundedIcon, Panel )

QSK_SUBCONTROL( BoxWithButtons, Panel )

BoxWithButtons::BoxWithButtons( const QString& title, const QskGradient& gradient, QQuickItem* parent )
    : Box( "", parent )
{
    setPanel( true );
    setSizePolicy( Qt::Vertical, QskSizePolicy::Maximum );

    auto* layout = new QskLinearBox( Qt::Horizontal, this );
    layout->setSpacing( 20 );

    QString iconFile = title.toLower();
    iconFile = iconFile.replace( ' ', '-' );
    auto* icon = new BigRoundedIcon( iconFile, gradient, layout );
    icon->setFixedSize( 68, 68 ); // ### fix properly

    auto* titleAndValue = new QskLinearBox( Qt::Vertical, layout );
    titleAndValue->setMargins( {0, 10, 0, 0} );

    auto* titleLabel = new QskTextLabel( title, titleAndValue );
    titleLabel->setFontRole( Skin::TitleFont );

    auto* value = new QskTextLabel( "+24", titleAndValue );
    value->setFontRole( QskSkin::HugeFont );
    value->setTextColor( "#929CB2" );

    new UpAndDownButton( layout );
}
