#ifndef ROUNDEDICON_H
#define ROUNDEDICON_H

#include <QskBox.h>
#include <QskGradient.h>

class QskGraphicLabel;

class RoundedIcon : public QskBox
{
    Q_OBJECT

public:
    RoundedIcon(const QString& iconName, const QskGradient& gradient, QQuickItem *parent = nullptr);

protected:
    void updateLayout() override;
    virtual QSizeF contentsSizeHint( Qt::SizeHint, const QSizeF& ) const override;

private:
    QString m_iconName;
    QskGradient m_gradient;
    QskGraphicLabel* m_graphicLabel = nullptr;
};

#endif // ROUNDEDICON_H
