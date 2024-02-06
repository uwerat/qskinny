/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskStandardSymbol.h"
#include "QskGraphic.h"

#include <qpainter.h>
#include <qpainterpath.h>

static void qskOkGraphic( QPainter* painter )
{
    QPainterPath path;

    path.moveTo( 14, 43 );
    path.cubicTo( 13.5, 43, 12.25, 42.5, 11.75, 41.5 );

    path.lineTo( 0, 20.5 );

    path.cubicTo( 0, 19, 0, 17.25, 1.5, 16.5 );
    path.cubicTo( 3, 15.75, 4.75, 16.25, 5.5, 17.75 );

    path.lineTo( 14.75, 34 );

    path.cubicTo( 17.25, 28.25, 20, 23.25, 22.75, 18.5 );
    path.cubicTo( 26.75, 12.25, 31.25, 6.5, 38.25, 0.75 );

    path.cubicTo( 39.5, 0, 41.25, 0, 42.25, 1 );
    path.cubicTo( 43.25, 2.25, 43.25, 4, 42, 5.25 );
    path.cubicTo( 35.50, 10.75, 31.5, 15.75, 27.75, 21.5 );
    path.cubicTo( 24.25, 27.25, 21.25, 33.75, 16.75, 41.5 );
    path.cubicTo( 16.25, 42.5, 15.25, 43, 14.25, 43 );

    painter->setPen( Qt::NoPen );

    painter->setBrush( QColor( 76, 175, 80 ) );
    painter->drawPath( path );
}

static void qskCancelGraphic( QPainter* painter )
{
    QPainterPath path;

    path.moveTo( 5, 0 );
    path.lineTo( 20, 15 );
    path.lineTo( 35, 0 );
    path.lineTo( 40, 5 );
    path.lineTo( 25, 20 );
    path.lineTo( 40, 35 );
    path.lineTo( 35, 40 );
    path.lineTo( 20, 25 );
    path.lineTo( 5, 40 );
    path.lineTo( 0, 35 );
    path.lineTo( 15, 20 );
    path.lineTo( 0, 5 );

    painter->setPen( Qt::NoPen );

    painter->setBrush( QColor( Qt::red ) );
    painter->drawPath( path );
}

static void qskCheckMarkGraphic( QPainter* painter )
{
    QPainterPath path;

    path.moveTo( 0.0, 0.5 );
    path.lineTo( 0.33, 1.0 );
    path.lineTo( 1.0, 0.0 );

    painter->setPen( QPen( Qt::black, 0.2 ) );
    painter->drawPath( path );
}

static void qskCrossMarkGraphic( QPainter* painter )
{
    painter->setPen( QPen( Qt::black, 0.2 ) );
    painter->drawLine( 0.0, 0.0, 1.0, 1.0 );
    painter->drawLine( 0.0, 1.0, 1.0, 0.0 );
}

static void qskBulletGraphic( QPainter* painter )
{
    painter->setPen( QPen( Qt::black, 1.0 ) );
    painter->drawEllipse( QRectF( 0.0, 0.0, 1.0, 1.0 ) );
}

static void qskTriangleGraphic( QPainter* painter, 
    qreal width, qreal height, int type )
{
    const QRectF rect( 0.0, 0.0, width, height );

    QPainterPath path;

    QPolygonF triangle;

    switch( type )
    {
        case QskStandardSymbol::TriangleUp:
        {
            triangle += rect.bottomLeft();
            triangle += QPointF( rect.center().x(), rect.top() );
            triangle += rect.bottomRight();

            break;
        }
        case QskStandardSymbol::TriangleDown:
        {
            triangle += rect.topLeft();
            triangle += QPointF( rect.center().x(), rect.bottom() );
            triangle += rect.topRight();

            break;
        }
        case QskStandardSymbol::TriangleRight:
        {
            triangle += rect.bottomLeft();
            triangle += QPointF( rect.right(), rect.center().y() );
            triangle += rect.topLeft();

            break;
        }
        case QskStandardSymbol::TriangleLeft:
        {
            triangle += rect.bottomRight();
            triangle += QPointF( rect.left(), rect.center().y() );
            triangle += rect.topRight();

            break;
        }
    }

    path.addPolygon( triangle );

    painter->setPen( Qt::NoPen );
    painter->setBrush( QColor( Qt::black ) );
    painter->drawPath( path );
}

QskGraphic QskStandardSymbol::graphic( Type symbolType )
{
    static QskGraphic graphics[ SymbolTypeCount ];

    if ( symbolType < 0 || symbolType >= SymbolTypeCount )
        return QskGraphic();

    if ( graphics[ symbolType ].isNull() )
    {
        QPainter painter( &graphics[ symbolType ] );
        painter.setRenderHint( QPainter::Antialiasing, true );

        switch ( symbolType )
        {
            case QskStandardSymbol::Ok:
            {
                qskOkGraphic( &painter );
                break;
            }
            case QskStandardSymbol::Cancel:
            {
                qskCancelGraphic( &painter );
                break;
            }
            case QskStandardSymbol::CheckMark:
            {
                qskCheckMarkGraphic( &painter );
                break;
            }
            case QskStandardSymbol::CrossMark:
            {
                qskCrossMarkGraphic( &painter );
                break;
            }
            case QskStandardSymbol::SegmentedBarCheckMark:
            {
                qskCheckMarkGraphic( &painter );
                break;
            }
            case QskStandardSymbol::TriangleUp:
            case QskStandardSymbol::TriangleDown:
            {
                qskTriangleGraphic( &painter, 2.0, 1.0, symbolType );
                break;
            }
            case QskStandardSymbol::TriangleLeft:
            case QskStandardSymbol::TriangleRight:
            {
                qskTriangleGraphic( &painter, 1.0, 2.0, symbolType );
                break;
            }
            case QskStandardSymbol::Bullet:
            {
                qskBulletGraphic( &painter );
                break;
            }
            case QskStandardSymbol::NoSymbol:
            case QskStandardSymbol::SymbolTypeCount:
            {
                break;
            }
        }
    }

    return graphics[ symbolType ];
}

#include "moc_QskStandardSymbol.cpp"
