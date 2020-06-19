#include "CircularProgressBar.h"

#include <QPainter>

CircularProgressBar::CircularProgressBar(int progress, QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_progress(progress)
{
}

void CircularProgressBar::paint(QPainter *painter)
{
    auto size = contentsSize();
    qDebug() << Q_FUNC_INFO << size;
    QRectF outerRect({0, 0}, size);

    painter->setRenderHint(QPainter::Antialiasing, true);

    // ### chose a different color here due to the lack of inner shadow:
    painter->setBrush({"#e8e8e8"});
    painter->setPen(Qt::white);
    painter->drawEllipse(outerRect);

    int startAngle = 1440;
    int endAngle = -16 * (m_progress / 100.0) * 360;

    painter->setBrush(color());
    painter->setPen(color());
    painter->drawPie(outerRect, startAngle, endAngle);

    painter->setBrush(Qt::white);
    painter->setPen(Qt::white);
    QRectF innerRect(width() / 2, width() / 2, size.width() - width(), size.height() - width());
    painter->drawEllipse(innerRect);
}
