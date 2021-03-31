#pragma once

#include <QskLinearBox.h>
#include <QGradient>

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
