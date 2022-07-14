/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskGridBox.h>
#include <QskLinearBox.h>

class RoomsPage : public QskLinearBox
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )

    RoomsPage( QQuickItem* parent );

  private:
    QList< QskLinearBox* > m_columns;
};
