#pragma once

#include <QskLinearBox.h>

class TopBarItem : public QskLinearBox
{
    Q_OBJECT

public:
    TopBarItem( const QString& name, QQuickItem* parent );

private:
    QString m_name;
};

class TopBar : public QskLinearBox
{
    Q_OBJECT

public:
    TopBar(QQuickItem* parent);

private:
    QList< TopBarItem* > m_entries;
};
