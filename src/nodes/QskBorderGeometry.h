/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BORDER_GEOMETRY_H
#define QSK_BORDER_GEOMETRY_H

#include <QskGlobal.h>

#include <QSGGeometry>
#include <QColor>
#include <QRectF>
#include <QMarginsF>

class QSGTexture;

class QSK_EXPORT QskBorderGeometry : public QSGGeometry
{
public:
    QskBorderGeometry();
    ~QskBorderGeometry();

    void setEdgeBackground( Qt::Edge, QRgb backgroundIn, QRgb backgroundOut );
    void setEdgeForeground( Qt::Edge, QRgb foreground );

    void setBorder( const QRectF& rect, const QMarginsF& border,
        const QRectF& textureCoordinates, const QSizeF& textureSize );

private:
    void update();
    void initIndex();

    QRectF m_rect;
    QMarginsF m_border;

    QRectF m_textureCoordinates;

    float m_scaleX;
    float m_scaleY;
};

#endif
