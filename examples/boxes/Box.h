/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef BOX_H
#define BOX_H

#include <QskRgbPalette.h>
#include <QskBox.h>

class Box : public QskBox
{
  public:
    enum FillType
    {
        Unfilled,
        Solid,
        Horizontal,
        Vertical,
        Diagonal
    };

    enum BorderType
    {
        NoBorder,
        Flat,
        Raised1,
        Sunken1,
        Raised2,
        Sunken2
    };

    Box( QQuickItem* parentItem = nullptr );

    void setBackground( FillType, QskRgbPalette::Theme, bool inverted = false );
    void setBorder( BorderType type, QskRgbPalette::Theme );

    void setShape( const QskBoxShapeMetrics& );
    void setShape( qreal radius, Qt::SizeMode );
    void setShape( qreal radiusX, qreal radiusY, Qt::SizeMode );

    void setBorderColor( const QColor& );
    void setBorderColors( const QColor& left, const QColor& top,
        const QColor& right, const QColor& bottom );

    void setBorderWidth( int );
    void setBorderWidth( qreal left, qreal top, qreal right, qreal bottom );

    void setGradient( QRgb );
    void setGradient( Qt::GlobalColor );
    void setGradient( const QColor& );

    void setGradient( QskGradient::Orientation, const QColor&, const QColor& );

    void setGradient( QskGradient::Orientation,
        const QColor&, const QColor&, const QColor& );

    void setGradient( const QskGradient& gradient );
    void setGradient( const QskGradient::Orientation, QskRgbPalette::Theme );
};

#endif
