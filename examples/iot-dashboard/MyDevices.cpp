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

#include "MyDevices.h"
#include "Skin.h"
#include "RoundedIcon.h"

#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskGridBox.h>
#include <QskTextLabel.h>

#include <QImage>

namespace
{
    class Device : public QskLinearBox
    {
        public:
            Device( const QString& name, bool isBright, QQuickItem* parent )
                : QskLinearBox( Qt::Vertical, parent )
                , m_name( name )
            {
                setDefaultAlignment( Qt::AlignCenter );
                setAutoAddChildren( false );

                m_icon = new RoundedIcon( QString(), isBright, true, this );
                m_icon->setSkinState( m_icon->skinState() | RoundedIcon::Small );
                m_icon->setOpacity( 0.15 );
                addItem( m_icon );

                auto* textLabel = new QskTextLabel( name, this );
                textLabel->setFontRole( QskSkin::TinyFont );
                textLabel->setAlignment( Qt::AlignHCenter );
                addItem( textLabel );

                auto fileName = name.toLower();
                fileName.replace( ' ', '-' );
                fileName = ":/images/" + fileName + ".png";
                QImage image( fileName );
                auto graphic = QskGraphic::fromImage( image );
                m_graphicLabel = new QskGraphicLabel( graphic, this );
            }

        protected:
            void updateLayout() override
            {
                QskLinearBox::updateLayout();

                // We cannot use the icon from RoundedIcon here because
                // it would inherit the transparency
                const qreal size = metric( RoundedIcon::Icon | QskAspect::Size );
                m_graphicLabel->setSize( {size, size} );
                m_graphicLabel->setPosition( { m_icon->position().x() + ( m_icon->width() - m_graphicLabel->width() ) / 2,
                                               ( m_icon->position().y() + m_icon->height() - m_graphicLabel->height() ) / 2 } );
            }

        private:
            QString m_name;
            RoundedIcon* m_icon;
            QskGraphicLabel* m_graphicLabel;
    };
}

MyDevices::MyDevices( QQuickItem* parent )
    : Box( "My devices", parent )
{
    auto* gridBox = new QskGridBox( this );
    gridBox->setSpacing( 15 );

    auto* lamps = new Device( "Lamps", true, gridBox );
    gridBox->addItem( lamps, 0, 0 );

    auto* musicSystem = new Device( "Music System", false, gridBox );
    gridBox->addItem( musicSystem, 0, 1 );

    auto* ac = new Device( "AC", false, gridBox );
    gridBox->addItem( ac, 1, 0 );

    auto* router = new Device( "Router", true, gridBox );
    gridBox->addItem( router, 1, 1 );
}
