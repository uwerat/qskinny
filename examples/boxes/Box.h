/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

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

    void setBorder( BorderType type, QRgb );

    void setShape( const QskBoxShapeMetrics& );
    void setShape( qreal radius, Qt::SizeMode );
    void setShape( qreal radiusX, qreal radiusY, Qt::SizeMode );

    void setBorderGradient( const QskGradient& );
    void setBorderGradients( const QskGradient& left, const QskGradient& top,
        const QskGradient& right, const QskGradient& bottom );

    void setBorderWidth( int );
    void setBorderWidth( qreal left, qreal top, qreal right, qreal bottom );

    void setGradient( QRgb );
    void setGradient( Qt::GlobalColor );
    void setGradient( const QColor& );

    void setGradient( FillType, const QColor&, const QColor& );
    void setGradient( FillType, const QColor&, const QColor&, const QColor& );
    void setGradient( FillType, const QskGradientStops& );

    void setTonalGradient( FillType, QRgb, bool inverted = false );
    void setWebGradient( FillType, QGradient::Preset, bool inverted = false );

    void setTonalPalette( FillType, QRgb );

  private:
    void setPanelGradient( const QskGradient& );
};
