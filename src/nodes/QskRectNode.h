/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_RECT_NODE_H
#define QSK_RECT_NODE_H

#include "QskGlobal.h"
#include "QskGradient.h"

#include <QColor>
#include <QSGNode>
#include <QSGVertexColorMaterial>
#include <QSGFlatColorMaterial>

class QSK_EXPORT QskRectNode : public QSGGeometryNode
{
public:
    QskRectNode();
    virtual ~QskRectNode();

    void setRect( const QRectF& rect );
    void setRadius( double radius );

    void setBorderWidth( double width );
    void setBorderColor( const QColor& color  );

    void setFillGradient( const QskGradient& );
    void setFillColor( const QColor& color  );

    void update();

private:
    void updateGeometry();

    QRectF m_rect;
    double m_radius;

    double m_borderWidth;

    QskGradient m_fillGradient;
    QColor m_fillColor;

    QColor m_borderColor;

    bool m_isDirty;

    QSGVertexColorMaterial m_materialVertex;
    QSGGeometry m_geometryVertex;

    QSGFlatColorMaterial m_materialFlat;
    QSGGeometry m_geometryFlat;
};

#endif
