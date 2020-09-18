#ifndef CARD_H
#define CARD_H

#include "src/shadowedrectangle.h"

#include <QskLinearBox.h>

class QskTextLabel;

class ShadowBox : public QskControl
{
        Q_OBJECT
    public:
        ShadowBox( QskControl* control, QQuickItem* parent );

    protected:
        void updateLayout() override;

    private:
        QskControl* m_control;
        ShadowedRectangle* m_rectangle;
};

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
