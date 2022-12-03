/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "ShapeItem.h"

class GeometricShape : public ShapeItem
{
    Q_OBJECT

    Q_PROPERTY( Figure figure READ figure WRITE setFigure )
    Q_PROPERTY( QColor border READ border WRITE setBorder )

  public:
    enum Figure
    {
        Rectangle,
        Diamond,
        TriangleDown,
        TriangleUp,
        TriangleLeft,
        TriangleRight,
        Ellipse,
        Ring,
        Star,
        Hexagon,
        Arc
    };
    Q_ENUM( Figure );

    GeometricShape( QQuickItem* parent = nullptr );
    GeometricShape( Figure figure, QQuickItem* parent = nullptr );

    void setFigure( Figure );
    Figure figure() const;

    void setBorder( const QColor& );
    QColor border() const;

  private:
    Figure m_figure = Rectangle;
};
