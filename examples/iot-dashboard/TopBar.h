#pragma once

#include <QskLinearBox.h>
#include <QGradient>

class TopBarItem : public QskLinearBox
{
    Q_OBJECT

public:
    TopBarItem(const QString& name, const QColor& textColor, const QGradient &gradient, int progress, int value, QQuickItem* parent );

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
