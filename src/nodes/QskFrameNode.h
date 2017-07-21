/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_FRAME_NODE_H
#define QSK_FRAME_NODE_H

#include "QskGlobal.h"

#include <QColor>
#include <QSGNode>
#include <QSGVertexColorMaterial>

class QSK_EXPORT QskFrameNode : public QSGGeometryNode
{
public:
    QskFrameNode();
    virtual ~QskFrameNode();

    void setColors( const QColor& darkColor,
        const QColor& fillColor, const QColor& lightColor );

    void setBorderWidth( double width );

    void setRadius( double radius );
    void setRect( const QRectF& rect );

    // -1.0: sunken, 0.0: flat, 1.0: raised
    void setShadeFactor( double );

    void update();

private:
    void updateGeometry();

    QColor m_darkColor;
    QColor m_lightColor;
    QColor m_fillColor;

    double m_borderWidth;

    QRectF m_rect;
    double m_radius;

    double m_shadeFactor;
    bool m_isDirty;

    QSGVertexColorMaterial m_material;
    QSGGeometry m_geometry;
};

#endif
