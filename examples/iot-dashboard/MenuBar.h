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

#ifndef MENUBAR_H
#define MENUBAR_H

#include <QskGraphicLabel.h>
#include <QskLinearBox.h>
#include <QskTextLabel.h>

class MenuBarTopLabel : public QskGraphicLabel
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Graphic )

        MenuBarTopLabel( const QskGraphic& graphic, QQuickItem* parent = nullptr )
            : QskGraphicLabel( graphic, parent )
        {
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskGraphicLabel::Graphic )
            {
                return Graphic;
            }

            return subControl;
        }
};

class MenuBarGraphicLabel : public QskGraphicLabel
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Graphic )

        MenuBarGraphicLabel( const QskGraphic& graphic, QQuickItem* parent = nullptr )
            : QskGraphicLabel( graphic, parent )
        {
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskGraphicLabel::Graphic )
            {
                return Graphic;
            }

            return subControl;
        }
};

class MenuBarLabel : public QskTextLabel
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Text )

        MenuBarLabel( const QString& text, QQuickItem* parent = nullptr )
            : QskTextLabel( text, parent )
        {
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskTextLabel::Text )
            {
                return Text;
            }

            return subControl;
        }
};

class MenuItem : public QskLinearBox
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel )
        QSK_STATES( Active )

        MenuItem( const QString& name, QQuickItem* parent );

        QskAspect::Subcontrol effectiveSubcontrol( QskAspect::Subcontrol subControl ) const override final;

    private:
        QString m_name;
        bool m_isActive = false;
};

class MenuBar : public QskLinearBox
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel )

        MenuBar( QQuickItem* parent );

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final;

    private:
        QList< QString > m_entryStrings;
        QList< MenuItem* > m_entries;
        uint m_activeEntry = 0;
};

#endif // MENUBAR_H
