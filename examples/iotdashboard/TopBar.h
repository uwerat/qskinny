/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskLinearBox.h>
#include <QskTextLabel.h>

class TimeTitleLabel : public QskTextLabel
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Text )

    TimeTitleLabel( const QString& text, QQuickItem* parent = nullptr )
        : QskTextLabel( text, parent )
    {
        setSubcontrolProxy( QskTextLabel::Text, Text );
    }
};

class TimeLabel : public QskTextLabel
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Text )

    TimeLabel( const QString& text, QQuickItem* parent = nullptr )
        : QskTextLabel( text, parent )
    {
        setSubcontrolProxy( QskTextLabel::Text, Text );
    }
};

class TopBarItem : public QskLinearBox
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Item1, Item2, Item3, Item4 )

    TopBarItem( int index, const QString& name, const QskGradient& gradient,
        int progress, int value, QQuickItem* parent );

  private:
    QString m_name;
};

class TopBar : public QskLinearBox
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )

    TopBar( QQuickItem* parent = nullptr );

  private:
    QList< TopBarItem* > m_entries;
};
