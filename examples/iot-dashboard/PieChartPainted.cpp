#include "PieChartPainted.h"

#include <QskSkin.h>
#include <QskTextLabel.h>

#include <QFontMetricsF>
#include <QQuickPaintedItem>

PieChartPainted::PieChartPainted(const QColor& color, const QGradient& gradient, int progress, int value, QQuickItem *parent)
    : QskControl(parent)
    , m_color(color)
    , m_gradient(gradient)
    , m_progressBar(new CircularProgressBar(gradient, progress, this))
    , m_progressLabel(new QskTextLabel(this))
//    , m_numberLabel(new QskTextLabel(QString::number(value), this))
//    , m_unitLabel(new QskTextLabel("kwH", this))
{
    setAutoLayoutChildren(true);

    auto progressText = QString::number(progress) + " %";
    m_progressLabel->setText(progressText);
    m_progressLabel->setFontRole(QskSkin::SmallFont);
    m_progressLabel->setTextColor(color);
}

QSizeF PieChartPainted::contentsSizeHint(Qt::SizeHint sizeHint, const QSizeF& size) const
{
    qDebug() << Q_FUNC_INFO << sizeHint << size << layoutRect() << contentsRect();
//    return size;
    return {57, 57};
}

void PieChartPainted::updateLayout()
{
    m_progressBar->setContentsSize(size().toSize());
    m_progressBar->update();

    auto rect = contentsRect();
    QFontMetricsF progressMetrics(m_progressLabel->effectiveFont(QskTextLabel::Text));
    auto textWidth = progressMetrics.width(m_progressLabel->text());
    auto posX = rect.width() / 2 - textWidth / 2;
    auto posY = rect.height() / 2 - progressMetrics.height() / 2;
    m_progressLabel->setPosition({posX, posY});
    m_progressLabel->setFixedWidth(textWidth);
}
