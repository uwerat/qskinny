/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskPushButton.h>

class RoundedIcon : public QskPushButton
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel, PalePanel, Graphic )
    QSK_STATES( Bright ) // to differentiate between orange and purple

    enum {
        NormalRole,
        CheckedRole,
    } GraphicRole;

    RoundedIcon( bool isBright, QQuickItem* parent = nullptr );

    void setPale( bool );
};
