/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxMetrics.h"
#include "QskBoxShapeMetrics.h"
#include "QskBoxBorderMetrics.h"
#include "QskVertex.h"
#include "QskFunctions.h"

QskBoxMetrics::QskBoxMetrics( const QRectF& rect,
        const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border )
    : outerRect( rect )
{
    isOutsideRounded = !shape.isRectangle();

    if ( !isOutsideRounded )
    {
        isInsideRounded = false;
        isOutsideSymmetric = true;
        stepSymmetries = Qt::Vertical | Qt::Horizontal;
        preferredOrientation = Qt::Vertical;

        const auto bw = border.widths();
        hasBorder = bw.width() > 0.0 || bw.height() > 0.0;

        innerRect = qskValidOrEmptyInnerRect( rect, bw );

        return;
    }

    isOutsideSymmetric = shape.isRectellipse();

    {
        const auto tl = shape.topLeft();
        const auto tr = shape.topRight();
        const auto bl = shape.bottomLeft();
        const auto br = shape.bottomRight();

        if ( tl.isEmpty() || tr.isEmpty() || ( tl.height() == tr.height() ) )
        {
            if ( bl.isEmpty() || br.isEmpty() || ( bl.height() == br.height() ) )
                stepSymmetries |= Qt::Vertical;
        }

        if ( tl.isEmpty() || bl.isEmpty() || ( tl.width() == bl.width() ) )
        {
            if ( tr.isEmpty() || br.isEmpty() || ( tr.width() == br.width() ) )
                stepSymmetries |= Qt::Horizontal;
        }
    }

    for ( int i = 0; i < 4; i++ )
    {
        auto& c = corners[ i ];

        const auto radius = shape.radius( static_cast< Qt::Corner >( i ) );

        c.radiusX = qBound( 0.0, radius.width(), 0.5 * outerRect.width() );
        c.radiusY = qBound( 0.0, radius.height(), 0.5 * outerRect.height() );
        c.stepCount = QskVertex::ArcIterator::segmentHint( qMax( c.radiusX, c.radiusY ) );

        switch ( i )
        {
            case Qt::TopLeftCorner:
                c.centerX = outerRect.left() + c.radiusX;
                c.centerY = outerRect.top() + c.radiusY;
                c.sx = -1.0;
                c.sy = -1.0;
                break;

            case Qt::TopRightCorner:
                c.centerX = outerRect.right() - c.radiusX;
                c.centerY = outerRect.top() + c.radiusY;
                c.sx = +1.0;
                c.sy = -1.0;
                break;

            case Qt::BottomLeftCorner:
                c.centerX = outerRect.left() + c.radiusX;
                c.centerY = outerRect.bottom() - c.radiusY;
                c.sx = -1.0;
                c.sy = +1.0;
                break;

            case Qt::BottomRightCorner:
                c.centerX = outerRect.right() - c.radiusX;
                c.centerY = outerRect.bottom() - c.radiusY;
                c.sx = +1.0;
                c.sy = +1.0;
                break;
        }
    }

    {
        const auto cleft = qMax( corners[ Qt::TopLeftCorner ].centerX,
            corners[ Qt::BottomLeftCorner ].centerX );

        const auto cright = qMin( corners[ Qt::TopRightCorner ].centerX,
            corners[ Qt::BottomRightCorner ].centerX );

        const auto ctop = qMax( corners[ Qt::TopLeftCorner ].centerY,
            corners[ Qt::TopRightCorner ].centerY );

        const auto cbottom = qMin( corners[ Qt::BottomLeftCorner ].centerY,
            corners[ Qt::BottomRightCorner ].centerY );

        // now the bounding rectangle of the fill area

        const auto bw = border.widths();
        hasBorder = bw.width() > 0.0 || bw.height() > 0.0;

        qreal l = outerRect.left() + bw.left();
        qreal t = outerRect.top() + bw.top();
        qreal r = outerRect.right() - bw.right();
        qreal b = outerRect.bottom() - bw.bottom();

        l = qMin( l, cright );
        r = qMax( r, cleft );
        t = qMin( t, cbottom );
        b = qMax( b, ctop );

        if ( l > r )
            l = r = r + 0.5 * ( l - r );

        if ( t > b )
            t = b = b + 0.5 * ( t - b );

        innerRect.setCoords( l, t, r, b );
    }

    const QskMargins margins(
        innerRect.left() - outerRect.left(),
        innerRect.top() - outerRect.top(),
        outerRect.right() - innerRect.right(),
        outerRect.bottom() - innerRect.bottom() );

    isBorderRegular = margins.isEquidistant();

    isInsideRounded = false;

    for ( int i = 0; i < 4; i++ )
    {
        auto& c = corners[ i ];

        if ( c.sx < 0.0 )
            c.radiusInnerX = c.radiusX - margins.left();
        else
            c.radiusInnerX = c.radiusX - margins.right();

        if ( c.sy < 0.0 )
            c.radiusInnerY = c.radiusY - margins.top();
        else
            c.radiusInnerY = c.radiusY - margins.bottom();

        if ( c.radiusInnerX > 0.0 && c.radiusInnerY > 0.0 )
        {
            c.centerInnerX = c.centerX;
            c.centerInnerY = c.centerY;

            isInsideRounded = true;
        }
        else
        {
            /*
                not enough space for a rounded border -> the inner side
                becomes rectangular
             */
            c.radiusInnerX = c.radiusInnerY = 0.0;
            c.centerInnerX = ( c.sx < 0.0 ) ? innerRect.left() : innerRect.right();
            c.centerInnerY = ( c.sy < 0.0 ) ? innerRect.top() : innerRect.bottom();
        }
    }

    {
        const auto tl = corners[ Qt::TopLeftCorner ].innerStepCount();
        const auto tr = corners[ Qt::TopRightCorner ].innerStepCount();
        const auto bl = corners[ Qt::BottomLeftCorner ].innerStepCount();
        const auto br = corners[ Qt::BottomRightCorner ].innerStepCount();

        if ( qMax( tl, tr ) + qMax( bl, br ) >= qMax( tl, bl ) + qMax( tr, br ) )
            preferredOrientation = Qt::Vertical;
        else
            preferredOrientation = Qt::Horizontal;
    }
}

int QskBoxMetrics::outerStepCount() const
{
    return corners[0].stepCount + corners[1].stepCount
        + corners[2].stepCount + corners[3].stepCount;
}

int QskBoxMetrics::innerStepCount() const
{
    return corners[0].innerStepCount() + corners[1].innerStepCount()
        + corners[2].innerStepCount() + corners[3].innerStepCount();
}
