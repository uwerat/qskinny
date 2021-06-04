/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef MENUBAR_H
#define MENUBAR_H

#include <QskGraphicLabel.h>
#include <QskLinearBox.h>
#include <QskTextLabel.h>

class MenuBarTopLabel : public QskGraphicLabel
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Graphic )

        MenuBarTopLabel( const QskGraphic& graphic, QQuickItem* parent = nullptr )
            : QskGraphicLabel( graphic, parent )
        {
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskGraphicLabel::Graphic )
            {
                return Graphic;
            }

            return subControl;
        }
};

class MenuBarGraphicLabel : public QskGraphicLabel
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Graphic )

        MenuBarGraphicLabel( const QskGraphic& graphic, QQuickItem* parent = nullptr )
            : QskGraphicLabel( graphic, parent )
        {
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskGraphicLabel::Graphic )
            {
                return Graphic;
            }

            return subControl;
        }
};

class MenuBarLabel : public QskTextLabel
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Text )

        MenuBarLabel( const QString& text, QQuickItem* parent = nullptr )
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

class MenuItem : public QskLinearBox
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel )
        QSK_STATES( Active )

        MenuItem( const QString& name, QQuickItem* parent );

        QskAspect::Subcontrol effectiveSubcontrol( QskAspect::Subcontrol subControl ) const override final;

    private:
        QString m_name;
        bool m_isActive = false;
};

class MenuBar : public QskLinearBox
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel )

        MenuBar( QQuickItem* parent );

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final;

    private:
        QList< QString > m_entryStrings;
        QList< MenuItem* > m_entries;
        uint m_activeEntry = 0;
};

#endif // MENUBAR_H
