/*
 *  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include "shadowedrectangle.h"

/**
 * A rectangle with a shadow, using a QQuickItem as texture.
 *
 * This item will render a rectangle, with a shadow below it. The rendering is done
 * using distance fields, which provide greatly improved performance. The shadow is
 * rendered outside of the item's bounds, so the item's width and height are the
 * rectangle's width and height.
 *
 * @since 5.69 / 2.12
 */
class ShadowedTexture : public ShadowedRectangle
{
    Q_OBJECT

    Q_PROPERTY(QQuickItem *source READ source WRITE setSource NOTIFY sourceChanged)

public:
    ShadowedTexture(QQuickItem *parent = nullptr);
    ~ShadowedTexture() override;

    QQuickItem *source() const;
    void setSource(QQuickItem *newSource);
    Q_SIGNAL void sourceChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *data) override;

private:
    QQuickItem *m_source = nullptr;
    bool m_sourceChanged = false;
};
