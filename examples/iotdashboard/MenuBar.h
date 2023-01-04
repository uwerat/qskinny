/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskGraphicLabel.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskTextLabel.h>

#include "MainItem.h"

class MenuBarTopLabel final : public QskGraphicLabel
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Graphic )

    MenuBarTopLabel( const QString& icon, QQuickItem* parent = nullptr )
        : QskGraphicLabel( icon, parent )
    {
        setSubcontrolProxy( QskGraphicLabel::Graphic, Graphic );
    }
};

class MenuButton final : public QskPushButton
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel, Text, Graphic )

    MenuButton( const QString& name, QQuickItem* parent );
};

class MenuBar final : public QskLinearBox
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )

    MenuBar( QQuickItem* parent );

  Q_SIGNALS:
    void pageChangeRequested( const int index );

  public Q_SLOTS:
    void setActivePage( const int index );

  private:
    MenuButton* m_buttons[ Cube::NumPositions ];
    uint m_currentIndex;
};
