#ifndef MENUBAR_H
#define MENUBAR_H

#include <QskLinearBox.h>

class MenuItem : public QskLinearBox
{
    Q_OBJECT

public:
    QSK_SUBCONTROLS( Panel )

    MenuItem( const QString& name, QQuickItem* parent );

    QskAspect::Subcontrol effectiveSubcontrol( QskAspect::Subcontrol subControl ) const override final;

    void setActive(bool active);

private:
    QString m_name;
    bool m_isActive = false;
};

class MenuBar : public QskLinearBox
{
    Q_OBJECT

public:
    MenuBar( QQuickItem* parent );

private:
    QList< QString > m_entryStrings;
    QList< MenuItem* > m_entries;
    uint m_activeEntry = 0;
};

#endif // MENUBAR_H
