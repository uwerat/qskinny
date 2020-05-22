#ifndef MENUBAR_H
#define MENUBAR_H

#include <QskLinearBox.h>

class MenuBar : public QskLinearBox
{
    Q_OBJECT

public:
    MenuBar( QQuickItem* parent );

private:
    QList< QString > m_entries;
};

#endif // MENUBAR_H
