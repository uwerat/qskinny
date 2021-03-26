#ifndef MENUBAR_H
#define MENUBAR_H

#include <QskLinearBox.h>
#include <QskTextLabel.h>

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

        MenuItem( const QString& name, QQuickItem* parent );

        QskAspect::Subcontrol effectiveSubcontrol( QskAspect::Subcontrol subControl ) const override final;

        void setActive( bool active );

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
