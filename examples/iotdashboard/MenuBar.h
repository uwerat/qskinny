/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskGraphicLabel.h>
#include <QskLinearBox.h>
#include <QskTextLabel.h>

class MenuBarTopLabel final : public QskGraphicLabel
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Graphic )

    MenuBarTopLabel( const QskGraphic& graphic, QQuickItem* parent = nullptr )
        : QskGraphicLabel( graphic, parent )
    {
        setSubcontrolProxy( QskGraphicLabel::Graphic, Graphic );
    }
};

class MenuBarGraphicLabel final : public QskGraphicLabel
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Graphic )

    MenuBarGraphicLabel( const QskGraphic& graphic, QQuickItem* parent = nullptr )
        : QskGraphicLabel( graphic, parent )
    {
        setSubcontrolProxy( QskGraphicLabel::Graphic, Graphic );
    }
};

class MenuBarLabel final : public QskTextLabel
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Text )

    MenuBarLabel( const QString& text, QQuickItem* parent = nullptr )
        : QskTextLabel( text, parent )
    {
        setSubcontrolProxy( QskTextLabel::Text, Text );
    }
};

class MenuItem final : public QskLinearBox
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )
    QSK_STATES( Active )

    MenuItem( const QString& name, QQuickItem* parent );
};

class MenuBar final : public QskLinearBox
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )

    MenuBar( QQuickItem* parent );

  private:
    QList< QString > m_entryStrings;
    QList< MenuItem* > m_entries;
    uint m_activeEntry = 0;
};
