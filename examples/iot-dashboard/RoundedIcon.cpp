/****************************************************************************
**
** Copyright 2021 Edelhirsch Software GmbH. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of the copyright holder nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

#include "RoundedIcon.h"

#include <QskBoxShapeMetrics.h>
#include <QskGraphic.h>
#include <QskGraphicLabel.h>

#include <QImage>

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
