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

#include "UpAndDownButton.h"

#include <QskBoxShapeMetrics.h>
#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskPushButton.h>

#include <QImage>

QSK_SUBCONTROL( RoundButton, Panel )

QSK_STATE( RoundButton, Top, ( QskAspect::FirstUserState << 1 ) )

RoundButton::RoundButton( QskAspect::Placement placement, QQuickItem* parent )
    : QskPushButton( parent )
{
    setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Expanding );

    QskGraphic graphic;
    QImage image;

    if( placement == QskAspect::Top )
    {
        setSkinStateFlag( Top );
        image.load( ":/images/up.svg" );
    }
    else
    {
        image.load( ":/images/down.svg" );
    }

    setGraphicSourceSize( image.size() );
    graphic = QskGraphic::fromImage( image );
    setGraphic( graphic );
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

    new RoundButton( QskAspect::Top, this );
    new RoundButton( QskAspect::Bottom, this );
}
