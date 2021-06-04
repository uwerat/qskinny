/*
 *  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QSGGeometryNode>
#include <QColor>
#include <QVector2D>
#include <QVector4D>

#include "shadowedrectanglematerial.h"

struct QSGMaterialType;
class ShadowedBorderRectangleMaterial;

/**
 * Scene graph node for a shadowed rectangle.
 *
 * This node will set up the geometry and materials for a shadowed rectangle,
 * optionally with rounded corners.
 *
 * \note You must call updateGeometry() after setting properties of this node,
 * otherwise the node's state will not correctly reflect all the properties.
 *
 * \sa ShadowedRectangle
 */
class ShadowedRectangleNode : public QSGGeometryNode
{
public:
    ShadowedRectangleNode();

    /**
     * Set whether to draw a border.
     *
     * Note that this will switch between a material with or without border.
     * This means this needs to be called before any other setters.
     */
    void setBorderEnabled(bool enabled);

    void setRect(const QRectF &rect);
    void setSize(qreal size);
    void setRadius(const QVector4D &radius);
    void setColor(const QColor &color);
    void setShadowColor(const QColor &color);
    void setOffset(const QVector2D &offset);
    void setBorderWidth(qreal width);
    void setBorderColor(const QColor &color);
    void setShaderType(ShadowedRectangleMaterial::ShaderType type);

    /**
     * Update the geometry for this node.
     *
     * This is done as an explicit step to avoid the geometry being recreated
     * multiple times while updating properties.
     */
    void updateGeometry();

protected:
    virtual ShadowedRectangleMaterial *createBorderlessMaterial();
    virtual ShadowedBorderRectangleMaterial *createBorderMaterial();
    virtual QSGMaterialType* borderMaterialType();
    virtual QSGMaterialType* borderlessMaterialType();

    QSGGeometry *m_geometry;
    ShadowedRectangleMaterial *m_material = nullptr;
    ShadowedRectangleMaterial::ShaderType m_shaderType = ShadowedRectangleMaterial::ShaderType::Standard;

private:
    QRectF m_rect;
    qreal m_size = 0.0;
    QVector4D m_radius = QVector4D{0.0, 0.0, 0.0, 0.0};
    QVector2D m_offset = QVector2D{0.0, 0.0};
    QVector2D m_aspect = QVector2D{1.0, 1.0};
    qreal m_borderWidth = 0.0;
    QColor m_borderColor;
};
