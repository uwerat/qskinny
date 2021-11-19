/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskArcMetrics.h>
#include <QskIntervalF.h>
#include <QskScaleTickmarks.h>

#include <QSGGeometryNode>

class RadialTickmarksNodePrivate;

class RadialTickmarksNode : public QSGGeometryNode
{
  public:
    RadialTickmarksNode();
    ~RadialTickmarksNode() override;

    void update( const QColor&, const QRectF&, const QskArcMetrics&,
        const QskIntervalF&, const QskScaleTickmarks&, int, Qt::Orientation );

  private:
    Q_DECLARE_PRIVATE( RadialTickmarksNode )
};
