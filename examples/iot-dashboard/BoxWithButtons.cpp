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

#include "BoxWithButtons.h"

#include "RoundedIcon.h"
#include "Skin.h"
#include "UpAndDownButton.h"

#include <QskTextLabel.h>

QSK_SUBCONTROL( ButtonValueLabel, Text )

QSK_SUBCONTROL( TitleAndValueBox, Panel )

QSK_SUBCONTROL( BoxWithButtons, Panel )

QSK_SUBCONTROL( IndoorTemperature, Panel )
QSK_SUBCONTROL( Humidity, Panel )

BoxWithButtons::BoxWithButtons( const QString& title, const QString& value, bool isBright, QQuickItem* parent )
    : Box( "", parent )
{
    setPanel( true );
    setSizePolicy( Qt::Vertical, QskSizePolicy::Maximum );

    auto* layout = new QskLinearBox( Qt::Horizontal, this );
    layout->setSpacing( 20 );

    QString iconFile = title.toLower();
    iconFile = iconFile.replace( ' ', '-' );
    new RoundedIcon( iconFile, isBright, false, layout );

    auto* titleAndValue = new TitleAndValueBox( Qt::Vertical, layout );

    auto* titleLabel = new QskTextLabel( title, titleAndValue );
    titleLabel->setFontRole( Skin::TitleFont );

    new ButtonValueLabel( value, titleAndValue );

    new UpAndDownButton( layout );
}

#include "moc_BoxWithButtons.cpp"
