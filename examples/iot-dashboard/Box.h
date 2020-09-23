#ifndef CARD_H
#define CARD_H

#include "src/shadowedrectangle.h"

#include <QskLinearBox.h>

class QskTextLabel;

class ShadowBox : public QskControl
{
        Q_OBJECT
    public:
        ShadowBox( QQuickItem* parent = nullptr );

    protected:
        void updateLayout() override;

    private:
//        QskControl* m_control;
        ShadowedRectangle* m_rectangle;
};

class Box : public QskLinearBox
{
        Q_OBJECT
    public:
        QSK_SUBCONTROLS( Panel )

        Box( const QString& title, QQuickItem* parent );

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final;

    private:
        QString m_title;
        QskTextLabel* m_label;
        QskControl* m_content;
};

#endif // CARD_H
