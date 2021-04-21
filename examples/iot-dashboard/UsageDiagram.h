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

#ifndef USAGEDIAGRAM_H
#define USAGEDIAGRAM_H

#include "Box.h"

#include <QskTextLabel.h>

#include <QQuickPaintedItem>

class Diagram;
class DiagramContent;
class QskGridBox;

class WeekdayLabel : public QskTextLabel
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel, Text )

        WeekdayLabel( const QString& text, QQuickItem* parent ) : QskTextLabel( text, parent )
        {
            setPanel( true );
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskTextLabel::Panel )
            {
                return Panel;
            }

            if( subControl == QskTextLabel::Text )
            {
                return Text;
            }

            return subControl;
        }
};

class WeekdayBox : public QskBox
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel )

        WeekdayBox( QQuickItem* parent ) : QskBox( true, parent )
        {
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskBox::Panel )
            {
                return WeekdayBox::Panel;
            }

            return subControl;
        }
};

class CaptionColorBox : public QskBox
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel )

        CaptionColorBox( QQuickItem* parent ) : QskBox( true, parent )
        {
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskBox::Panel )
            {
                return Panel;
            }

            return subControl;
        }
};

class CaptionItem : public QskLinearBox
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel )
        QSK_STATES( Water, Electricity, Gas )

        CaptionItem( QskAspect::State state, QQuickItem* parent );
};

class CaptionBox : public QskLinearBox
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel )

        CaptionBox( QQuickItem* parent ) : QskLinearBox( Qt::Horizontal, parent )
        {
            setPanel( true );
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskBox::Panel )
            {
                return Panel;
            }

            return subControl;
        }
};

class UsageDiagram : public Box
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel )

        UsageDiagram( QQuickItem* parent );

        void updateLayout() override;

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskBox::Panel )
            {
                return Panel;
            }

            return subControl;
        }

    private:
        Diagram* m_diagram;
        QskLinearBox* m_captionBox;
        QskGridBox* m_weekdays;
};

#endif // USAGEDIAGRAM_H
