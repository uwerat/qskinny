/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

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
