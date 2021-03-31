#include "RoundedIcon.h"

#include <QskBoxShapeMetrics.h>
#include <QskGraphic.h>
#include <QskGraphicLabel.h>

#include <QImage>

QSK_SUBCONTROL( RoundedIcon, Panel )
QSK_STATE( RoundedIcon, Bright, ( QskAspect::FirstUserState << 1 ) )

RoundedIcon::RoundedIcon( const QString& iconName, bool isBright, QQuickItem* parent )
    : QskBox( parent )
    , m_iconName( iconName )
{
    setPanel( true );
    setPolishOnResize( true );

    if( isBright )
    {
        setSkinState( Bright );
    }

    setSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Constrained );

    QString fileName = ":/images/" + iconName + ".png";

    if( QFile::exists( fileName ) )
    {
        QImage image( fileName );
        auto graphic = QskGraphic::fromImage( image );
        m_graphicLabel = new QskGraphicLabel( graphic, this );
    }
}

void RoundedIcon::updateLayout()
{
    if( m_graphicLabel )
    {
        m_graphicLabel->setSize( {36, 36} );
        m_graphicLabel->setPosition( { ( width() - m_graphicLabel->width() ) / 2, ( height() - m_graphicLabel->height() ) / 2 } );
    }
}

QSizeF RoundedIcon::contentsSizeHint( Qt::SizeHint /*which*/, const QSizeF& size ) const
{
    QSizeF ret = size;
    ret.setHeight( size.width() );

    return ret;
}
