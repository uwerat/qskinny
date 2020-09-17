#ifndef CARD_H
#define CARD_H

#include <QskLinearBox.h>

class QskTextLabel;

class Box : public QskLinearBox
{
    public:
        Box( const QString& title, QQuickItem* parent );

    private:
        QString m_title;
        QskTextLabel* m_label;
        QskControl* m_content;
};

#endif // CARD_H
