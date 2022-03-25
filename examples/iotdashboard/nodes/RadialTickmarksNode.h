/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskIntervalF.h>

#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

class QskArcMetrics;
class QskScaleTickmarks;

class RadialTickmarksNode : public QSGGeometryNode
{
  public:
    RadialTickmarksNode();
    ~RadialTickmarksNode() override;

    void update( const QColor&, const QRectF&,
        const QskArcMetrics&, const QskScaleTickmarks&, int lineWidth );

  private:
    QSGGeometry m_geometry;
    QSGFlatColorMaterial m_material;

    QRectF m_rect;
    int m_lineWidth = 0;

    QskHashValue m_hash = 0;
};
