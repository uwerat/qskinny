/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "Box.h"
#include "RoundedIcon.h"

#include <QskBoxShapeMetrics.h>
#include <QskTextLabel.h>

class ButtonValueLabel : public QskTextLabel
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Text )

    ButtonValueLabel( const QString& text, QQuickItem* parent )
        : QskTextLabel( text, parent )
    {
        setSubcontrolProxy( QskTextLabel::Text, Text );
    }
};

class TitleAndValueBox : public QskLinearBox
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )

    TitleAndValueBox( Qt::Orientation orientation, QQuickItem* parent )
        : QskLinearBox( orientation, parent )
    {
        setPanel( true );
        setSubcontrolProxy( QskLinearBox::Panel, Panel );
    }
};

class BoxWithButtons : public Box
{
  public:
    QSK_SUBCONTROLS( Panel )

    BoxWithButtons( const QString& title, const QString& value,
        bool isBright, QQuickItem* parent = nullptr );
};

class IndoorTemperature : public BoxWithButtons
{
  public:
    QSK_SUBCONTROLS( Panel )

    IndoorTemperature( QQuickItem* parent )
        : BoxWithButtons( "Indoor Temperature", "+24", true, parent )
    {
    }
};

class Humidity : public BoxWithButtons
{
  public:
    QSK_SUBCONTROLS( Panel )

    Humidity( QQuickItem* parent )
        : BoxWithButtons( "Humidity", "30%", false, parent )
    {
    }
};
