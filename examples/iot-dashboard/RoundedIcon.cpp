/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "RoundedIcon.h"

#include <QskBoxShapeMetrics.h>
#include <QskGraphic.h>
#include <QskGraphicLabel.h>

#include <QImage>
#include <QFile>

QSK_SUBCONTROL( RoundedIcon, Panel )
QSK_SUBCONTROL( RoundedIcon, Icon )
QSK_STATE( RoundedIcon, Bright, ( QskAspect::FirstUserState << 1 ) )
QSK_STATE( RoundedIcon, Small, ( QskAspect::FirstUserState << 2 ) )

RoundedIcon::RoundedIcon( const QString& iconName, bool isBright, bool isSmall, QQuickItem* parent )
    : QskBox( parent )
    , m_iconName( iconName )
{
    setPanel( true );
    setPolishOnResize( true );

    if( isSmall )
    {
        setSkinState( skinState() | Small );
    }

    setSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Constrained );
    const qreal size = metric( RoundedIcon::Panel | QskAspect::Size );
    setFixedWidth( size );

    if( isBright )
    {
        setSkinState( Bright );
    }

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
        const qreal size = metric( Icon | QskAspect::Size );
        m_graphicLabel->setSize( {size, size} );
        m_graphicLabel->setPosition( { ( width() - m_graphicLabel->width() ) / 2, ( height() - m_graphicLabel->height() ) / 2 } );
    }
}

QSizeF RoundedIcon::contentsSizeHint( Qt::SizeHint /*which*/, const QSizeF& size ) const
{
    QSizeF ret = size;
    ret.setHeight( size.width() );

    return ret;
}

#include "moc_RoundedIcon.cpp"
