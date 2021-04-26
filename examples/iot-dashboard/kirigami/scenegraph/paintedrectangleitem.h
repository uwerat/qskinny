/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PAINTEDRECTANGLEITEM_H
#define PAINTEDRECTANGLEITEM_H

#include <QQuickPaintedItem>

/**
 * A rectangle with a border and rounded corners, rendered through QPainter.
 *
 * This is a helper used by ShadowedRectangle as fallback for when software
 * rendering is used, which means our shaders cannot be used.
 *
 * Since we cannot actually use QSGPaintedNode, we need to do some trickery
 * using QQuickPaintedItem as a child of ShadowedRectangle.
 *
 * \warning This item is **not** intended as a general purpose item.
 */
class PaintedRectangleItem : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit PaintedRectangleItem(QQuickItem *parent = nullptr);

    void setColor(const QColor &color);
    void setRadius(qreal radius);
    void setBorderColor(const QColor &color);
    void setBorderWidth(qreal width);

    void paint(QPainter *painter) override;

private:
    QColor m_color;
    qreal m_radius = 0.0;
    QColor m_borderColor;
    qreal m_borderWidth = 0.0;
};

#endif // PAINTEDRECTANGLEITEM_H
