/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

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

    WeekdayLabel( const QString& text, QQuickItem* parent )
        : QskTextLabel( text, parent )
    {
        setPanel( true );

        setSubcontrolProxy( QskTextLabel::Panel, Panel );
        setSubcontrolProxy( QskTextLabel::Text, Text );
    }
};

class WeekdayBox : public QskBox
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )

    WeekdayBox( QQuickItem* parent ):
        QskBox( true, parent )
    {
        setSubcontrolProxy( QskBox::Panel, WeekdayBox::Panel );
    }
};

class CaptionColorBox : public QskBox
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )

    CaptionColorBox( QQuickItem* parent ):
        QskBox( true, parent )
    {
        setSubcontrolProxy( QskBox::Panel, Panel );
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

    CaptionBox( QQuickItem* parent ):
        QskLinearBox( Qt::Horizontal, parent )
    {
        setPanel( true );
        setSubcontrolProxy( QskBox::Panel, Panel );
    }
};

class UsageDiagram : public Box
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )

    UsageDiagram( QQuickItem* parent = nullptr );

  protected:
    void updateLayout() override;

  private:
    Diagram* m_diagram;
    QskLinearBox* m_captionBox;
    QskGridBox* m_weekdays;
};
