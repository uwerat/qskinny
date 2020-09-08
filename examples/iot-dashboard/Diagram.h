#ifndef DIAGRAM_H
#define DIAGRAM_H

#include <QskLinearBox.h>

#include <QQuickPaintedItem>

class DiagramContent;

class Diagram : public QskLinearBox
{
        Q_OBJECT

    public:
        Diagram( QQuickItem* parent );
        void updateLayout() override;

    private:
        QskLinearBox* m_caption;
        QskLinearBox* m_weekdays;
        DiagramContent* m_content;
};

class DiagramContent : public QQuickPaintedItem
{
        Q_OBJECT

    public:
        DiagramContent( QQuickItem* parent );

        virtual void paint( QPainter* painter ) override;

};

#endif // DIAGRAM_H
