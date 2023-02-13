/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SkinnyShapeFactory.h"

QPainterPath SkinnyShapeFactory::shapePath( Shape shape, const QSizeF& size )
{
    QRectF rect;
    rect.setSize( size );

    QPainterPath path;

    switch( shape )
    {
        case Rectangle:
        {
            path.addRect( rect );
            break;
        }
        case Diamond:
        {
            QPolygonF diamond;
            diamond += QPointF( rect.center().x(), rect.top() );
            diamond += QPointF( rect.left(), rect.center().y() );
            diamond += QPointF( rect.center().x(), rect.bottom() );
            diamond += QPointF( rect.right(), rect.center().y() );

            path.addPolygon( diamond );
            break;
        }
        case TriangleDown:
        {
            QPolygonF triangle;
            triangle += rect.topLeft();
            triangle += QPointF( rect.center().x(), rect.bottom() );
            triangle += rect.topRight();

            path.addPolygon( triangle );
            break;
        }
        case TriangleUp:
        {
            QPolygonF triangle;
            triangle += rect.bottomLeft();
            triangle += QPointF( rect.center().x(), rect.top() );
            triangle += rect.bottomRight();

            path.addPolygon( triangle );
            break;
        }
        case TriangleRight:
        {
            QPolygonF triangle;
            triangle += rect.bottomLeft();
            triangle += QPointF( rect.right(), rect.center().y() );
            triangle += rect.topLeft();

            path.addPolygon( triangle );
            break;
        }
        case TriangleLeft:
        {
            QPolygonF triangle;
            triangle += rect.bottomRight();
            triangle += QPointF( rect.left(), rect.center().y() );
            triangle += rect.topRight();

            path.addPolygon( triangle );
            break;
        }
        case Ellipse:
        {
            path.addEllipse( rect );
            break;
        }
        case Ring:
        {
            path.addEllipse( rect );

            const double w = 0.25 * rect.width();
            path.addEllipse( rect.adjusted( w, w, -w, -w ) );
            break;
        }
        case Star:
        {
            const double cos30 = 0.866025;

            const double dy = 0.25 * size.height();
            const double dx = 0.5 * size.width() * cos30 / 3.0;

            double x1 = rect.center().x() - 3 * dx;
            double y1 = rect.center().y() - 2 * dy;

            const double x2 = x1 + 1 * dx;
            const double x3 = x1 + 2 * dx;
            const double x4 = x1 + 3 * dx;
            const double x5 = x1 + 4 * dx;
            const double x6 = x1 + 5 * dx;
            const double x7 = x1 + 6 * dx;

            const double y2 = y1 + 1 * dy;
            const double y3 = y1 + 2 * dy;
            const double y4 = y1 + 3 * dy;
            const double y5 = y1 + 4 * dy;

            QPolygonF star;
            star += QPointF( x4, y1 );
            star += QPointF( x5, y2 );
            star += QPointF( x7, y2 );
            star += QPointF( x6, y3 );
            star += QPointF( x7, y4 );
            star += QPointF( x5, y4 );
            star += QPointF( x4, y5 );
            star += QPointF( x3, y4 );
            star += QPointF( x1, y4 );
            star += QPointF( x2, y3 );
            star += QPointF( x1, y2 );
            star += QPointF( x3, y2 );

            path.addPolygon( star );
            break;
        }
        case Hexagon:
        {
            const double cos30 = 0.866025;

            const double dx = 0.5 * size.width() - cos30;
            const double dy = 0.25 * size.height();

            double x1 = rect.center().x() - dx;
            double y1 = rect.center().y() - 2 * dy;

            const double x2 = x1 + 1 * dx;
            const double x3 = x1 + 2 * dx;

            const double y2 = y1 + 1 * dy;
            const double y3 = y1 + 3 * dy;
            const double y4 = y1 + 4 * dy;

            QPolygonF hexagon;
            hexagon += QPointF( x2, y1 );
            hexagon += QPointF( x3, y2 );
            hexagon += QPointF( x3, y3 );
            hexagon += QPointF( x2, y4 );
            hexagon += QPointF( x1, y3 );
            hexagon += QPointF( x1, y2 );

            path.addPolygon( hexagon );
            break;
        }
        case Arc:
        {
            path.arcMoveTo( rect, -60 );
            path.arcTo( rect, -60, 300 );
            
            const double w = 0.25 * rect.width();
            const auto r = rect.adjusted( w, w, -w, -w );

            QPainterPath innerPath;

            innerPath.arcMoveTo( r, 240 );
            innerPath.arcTo( r, 240, -300 );

            path.connectPath( innerPath );
            
            break;
        }

        default:
            return QPainterPath();
    };

    path.closeSubpath();
    return path;
}

#include "moc_SkinnyShapeFactory.cpp"
