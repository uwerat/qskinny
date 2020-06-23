#ifndef CIRCULARPROGRESSBAR_H
#define CIRCULARPROGRESSBAR_H

#include <QColor>
#include <QQuickPaintedItem>

class CircularProgressBar : public QQuickPaintedItem
{
public:
    CircularProgressBar(const QColor& color, int progress, QQuickItem* parent = nullptr);

    virtual void paint(QPainter *painter) override;

    QColor color() const {
        return m_color;
    }

    void setColor(const QColor& color) {
        m_color = color;
    }

    double width() const {
        return m_width;
    }

    void setWidth(double width) {
        m_width = width;
    }

private:
    QColor m_color;
    double m_width = 20;
    int m_progress;
};

#endif // CIRCULARPROGRESSBAR_H
