#include "UpAndDownButton.h"

#include <QskBoxShapeMetrics.h>
#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskPushButton.h>

#include <QImage>

QSK_SUBCONTROL( RoundButton, Panel )

RoundButton::RoundButton( Qt::Edge edge, QQuickItem* parent )
    : QskPushButton( parent )
{
    setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Expanding );
    setFixedWidth( 42 );
    setFixedHeight( 46.31 );

    QskGraphic graphic;

    if( edge == Qt::TopEdge )
    {
        setBoxShapeHint( RoundButton::Panel, {30, 30, 0, 0} );
        QImage upImage( ":/images/up.png" );
        graphic = QskGraphic::fromImage( upImage );
    }
    else
    {
        setBoxShapeHint( RoundButton::Panel, {0, 0, 30, 30} );
        QImage downImage( ":/images/down.png" );
        graphic = QskGraphic::fromImage( downImage );
    }

    graphic.setDefaultSize( {10, 5.71} );
    setGraphic( graphic );
    setGraphicSourceSize( {10, 5.71} );
}

QskAspect::Subcontrol RoundButton::effectiveSubcontrol( QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskPushButton::Panel )
    {
        return RoundButton::Panel;
    }

    return subControl;
}

UpAndDownButton::UpAndDownButton( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
    setSpacing( 0 );

    auto* upButton = new RoundButton( Qt::TopEdge, this );
    auto* downButton = new RoundButton( Qt::BottomEdge, this );
}
