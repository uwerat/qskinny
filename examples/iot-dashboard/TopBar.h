/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef TOPBAR_H
#define TOPBAR_H

#include <QskLinearBox.h>
#include <QskTextLabel.h>

#include <QGradient>

class TimeTitleLabel : public QskTextLabel
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Text )

        TimeTitleLabel( const QString& text, QQuickItem* parent = nullptr )
            : QskTextLabel( text, parent )
        {
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskTextLabel::Text )
            {
                return Text;
            }

            return subControl;
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
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskTextLabel::Text )
            {
                return Text;
            }

            return subControl;
        }
};

class TopBarItem : public QskLinearBox
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Item1, Item2, Item3, Item4 )

        TopBarItem( int index, const QString& name, const QskGradient& gradient, int progress, int value, QQuickItem* parent );

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskLinearBox::Panel )
            {
                return Panel;
            }

            return subControl;
        }

    private:
        QString m_name;
};

class TopBar : public QskLinearBox
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel )

        TopBar( QQuickItem* parent );

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskLinearBox::Panel )
            {
                return Panel;
            }

            return subControl;
        }

    private:
        QList< TopBarItem* > m_entries;
};

#endif
