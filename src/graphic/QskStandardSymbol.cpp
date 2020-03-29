/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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

static void qskCriticalGraphic( QPainter* painter )
{
    QPainterPath path;
    path.addEllipse( 0, 5, 40, 40 );
    path.addRect( 5, 22, 30, 5 );

    painter->setPen( Qt::NoPen );
    // painter->setBrush( QColor( Qt::black ) );
    painter->setBrush( QColor( Qt::red ) );
    painter->drawPath( path );
}

static void qskWarningGraphic( QPainter* painter )
{
    const QRectF outerRect( 0, 2, 40, 36 );

    const double off = 0.2 * qMin( outerRect.width(), outerRect.height() );

    const double offBottom = 0.5 * ( outerRect.width() / outerRect.height() ) * off;
    const QRectF innerRect = outerRect.adjusted( off, off, -off, -offBottom );

    QPainterPath path;

    path.moveTo( outerRect.center().x(), outerRect.top() );
    path.lineTo( outerRect.right(), outerRect.bottom() );
    path.lineTo( outerRect.left(), outerRect.bottom() );
    path.closeSubpath();

    path.moveTo( innerRect.left(), innerRect.bottom() );
    path.lineTo( innerRect.right(), innerRect.bottom() );
    path.lineTo( innerRect.center().x(), innerRect.top() );
    path.closeSubpath();

    painter->setPen( Qt::NoPen );
    painter->setBrush( QColor( Qt::red ) );
    painter->drawPath( path );

    const double d = 0.25 * off;

    const QRectF r1(
        innerRect.center().x() - d,
        innerRect.top() + 0.25 * innerRect.height(),
        2 * d,
        0.5 * innerRect.height() );

    painter->fillRect( r1, Qt::black );

    const QRectF r2(
        r1.x(),
        r1.bottom() + 0.05 * innerRect.height(),
        r1.width(),
        r1.width() );

    painter->fillRect( r2, Qt::black );
}

static void qskQuestionGraphic( QPainter* painter )
{
    const double w = 40;
    const double h = 50;

    QPainterPath path;

    path.addRect( 0, 0, 1.0, 1.0 );

    path.moveTo( 0.55, 0.63 );
    path.lineTo( 0.325, 0.63 );
    path.cubicTo( 0.325, 0.578, 0.33, 0.554, 0.3425, 0.532 );
    path.cubicTo( 0.3575, 0.51, 0.385, 0.484, 0.43, 0.452 );
    path.cubicTo( 0.495, 0.406, 0.5125, 0.388, 0.5225, 0.374 );
    path.cubicTo( 0.5325, 0.358, 0.54, 0.342, 0.54, 0.328 );
    path.cubicTo( 0.54, 0.304, 0.53, 0.286, 0.51, 0.272 );
    path.cubicTo( 0.49, 0.258, 0.46, 0.252, 0.425, 0.252 );
    path.cubicTo( 0.3925, 0.252, 0.355, 0.258, 0.315, 0.27 );
    path.cubicTo( 0.275, 0.28, 0.235, 0.298, 0.1925, 0.32 );
    path.lineTo( 0.195, 0.162 );
    path.cubicTo( 0.245, 0.148, 0.29, 0.138, 0.3325, 0.132 );
    path.cubicTo( 0.3725, 0.124, 0.4125, 0.122, 0.4525, 0.122 );
    path.cubicTo( 0.5525, 0.122, 0.63, 0.138, 0.685, 0.172 );
    path.cubicTo( 0.7375, 0.204, 0.765, 0.252, 0.765, 0.316 );
    path.cubicTo( 0.7625, 0.348, 0.755, 0.378, 0.74, 0.404 );
    path.cubicTo( 0.725, 0.43, 0.695, 0.458, 0.6575, 0.488 );
    path.cubicTo( 0.5875, 0.536, 0.57, 0.552, 0.5625, 0.566 );
    path.cubicTo( 0.55, 0.578, 0.55, 0.592, 0.55, 0.63 );

    path.closeSubpath();

    path.addRect( 0.325, 0.66, 0.55 - 0.325, ( 0.55 - 0.325 ) * w / h );

    painter->scale( w, h );
    painter->setPen( Qt::NoPen );
    painter->setBrush( QColor( Qt::black ) );
    painter->drawPath( path );
}

static void qskInformationGraphic( QPainter* painter )
{
    const double w = 40;
    const double h = 50;

    const QRectF dotRect( 0.3 * w, 0.15 * h, 0.4 * w, 0.2 * h );
    const QRectF barRect( 0.3 * w, 0.4 * h, 0.4 * w, 0.5 * h );

    QPainterPath path;

    path.addRect( 0, 0, w, h );
    path.addEllipse( dotRect );

    const double dx = 0.33 * barRect.width();
    const double dy = 0.25 * dotRect.height();

    path.moveTo( barRect.left(), barRect.top() );
    path.cubicTo(
        barRect.left() + dx, barRect.top() + dy,
        barRect.left() + 2 * dx, barRect.top() + dy,
        barRect.right(), barRect.top() );

    path.lineTo( barRect.right(), barRect.bottom() );
    path.lineTo( barRect.left(), barRect.bottom() );
    path.closeSubpath();

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
            case QskStandardSymbol::Warning:
            {
                qskWarningGraphic( &painter );
                break;
            }
            case QskStandardSymbol::Critical:
            {
                qskCriticalGraphic( &painter );
                break;
            }
            case QskStandardSymbol::Question:
            {
                qskQuestionGraphic( &painter );
                break;
            }
            case QskStandardSymbol::Information:
            default:
            {
                qskInformationGraphic( &painter );
                break;
            }
        }
    }

    return graphics[ symbolType ];
}

#include "moc_QskStandardSymbol.cpp"
