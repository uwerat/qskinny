#ifndef CARD_H
#define CARD_H

#include <QskLinearBox.h>

class QskTextLabel;

class Card : public QskLinearBox
{
public:
    Card( const QString& title, QskControl* content, QQuickItem* parent );

private:
    QString m_title;
    QskTextLabel* m_label;
    QskControl* m_content;
};

#endif // CARD_H
