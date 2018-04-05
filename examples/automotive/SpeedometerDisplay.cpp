#include "SpeedometerDisplay.h"

#include "Speedometer.h"

#include <QskLinearBox.h>
#include <QskTextLabel.h>

SpeedometerDisplay::SpeedometerDisplay( QQuickItem *parent ) :
    QskControl( parent )
{
    QskLinearBox* box = new QskLinearBox( Qt::Horizontal, this );
    box->setAutoAddChildren( true );
    box->setAutoLayoutChildren( true );
    box->setMargins( QMarginsF( 40, 20, 40, 20 ) );
    box->setAlignment( 0, Qt::AlignHCenter );

    QskTextLabel* label = new QskTextLabel( QStringLiteral( "text" ), box );
    label->setFixedSize( QSizeF( 300, 300 ) );
    label->setAlignment( Qt::AlignHCenter | Qt::AlignCenter );

    Speedometer* speedometer = new Speedometer( box );
    speedometer->setFixedSize( QSizeF( 400, 400 ) );
}
