/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_OPTIONS_H
#define QSK_BOX_OPTIONS_H

#include "QskGlobal.h"

#include <QMarginsF>
#include <QColor>

class QSK_EXPORT QskBoxOptions
{
public:
    QskBoxOptions();

    void setBorder( qreal width );
    void setBorder( qreal left, qreal top, qreal right, qreal bottom );

    void setRadius( qreal radius );

    void setBorderRgb( QRgb rgb );
    void setBorderRgb( QRgb rgbLeft, QRgb rgbTop, QRgb rgbRight, QRgb rgbBottom );

    void setFillRgb( QRgb rgb );
    void setFillRgb( QRgb rgbTopLeft, QRgb rgbTopRight,
        QRgb rgbBottomRight, QRgb rgbBottomLeft );

    bool isVisible() const;

    QMarginsF unitedMargins() const;
    QMarginsF padding() const;

    uint metricsHash( uint seed = 0 ) const;
    uint colorsHash( uint seed = 0 ) const;

public:
    QMarginsF shadows;
    QMarginsF borders;

    struct Radius
    {
        qreal topLeftX;
        qreal topLeftY;

        qreal topRightX;
        qreal topRightY;

        qreal bottomRightX;
        qreal bottomRightY;

        qreal bottomLeftX;
        qreal bottomLeftY;

    } radius;

    struct Colors
    {
        // borders
        QRgb borderLeft;
        QRgb borderTop;
        QRgb borderRight;
        QRgb borderBottom;

        // background
        QRgb fillTopLeft;
        QRgb fillTopRight;
        QRgb fillBottomRight;
        QRgb fillBottomLeft;

    } color;
};

#endif
