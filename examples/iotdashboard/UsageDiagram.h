/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "Box.h"
#include "Diagram.h"

class UsageDiagramLegend : public QskLinearBox
{
  public:
    QSK_SUBCONTROLS( Panel, Symbol )
    QSK_STATES( Water, Electricity, Gas )

    UsageDiagramLegend( QQuickItem* parent = nullptr );
};

class UsageDiagram : public Diagram
{
  public:
    UsageDiagram( QQuickItem* parent = nullptr );

  private:
    void addCurve( const qreal values[], const size_t count );
};

class UsageDiagramBox : public Box
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel, DaysBox, DayText )

    UsageDiagramBox( const QString& title, Diagram* diagram, QQuickItem* parent = nullptr );
};
