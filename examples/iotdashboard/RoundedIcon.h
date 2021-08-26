/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskGraphicLabel.h>

class QskGraphicLabel;

class RoundedIcon : public QskGraphicLabel
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel, PalePanel )
    QSK_STATES( Bright ) // to differentiate between orange and purple

    RoundedIcon( bool isBright, QQuickItem* parent = nullptr );

    void setIcon( const QString& );
    void setPale( bool );
};
