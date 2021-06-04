/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "paintedrectangleitem.h"

#include <QPainter>

PaintedRectangleItem::PaintedRectangleItem(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
}

void PaintedRectangleItem::setColor(const QColor& color)
{
    m_color = color;
    update();
}

void PaintedRectangleItem::setRadius(qreal radius)
{
    m_radius = radius;
    update();
}

void PaintedRectangleItem::setBorderColor(const QColor& color)
{
    m_borderColor = color;
    update();
}

void PaintedRectangleItem::setBorderWidth(qreal width)
{
    m_borderWidth = width;
    update();
}

void PaintedRectangleItem::paint(QPainter* painter)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(m_color);
    if (m_borderWidth > 0.0) {
        painter->setPen(QPen(m_borderColor, m_borderWidth));
    } else {
        painter->setPen(Qt::transparent);
    }
    painter->drawRoundedRect(m_borderWidth / 2, m_borderWidth / 2, width() - m_borderWidth, height() - m_borderWidth, m_radius, m_radius);
}
