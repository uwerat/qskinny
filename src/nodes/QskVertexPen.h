/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_VERTEX_PEN_H
#define QSK_VERTEX_PEN_H

#include "QskGlobal.h"
#include <QColor>

class QSK_EXPORT QskVertexPen
{
public:
    QskVertexPen();

    bool isValid() const;
    bool isVisible() const;

    void setWidth( double );
    double width() const;

    void setColor( const QColor& color );
    void setColors( const QColor& darkColor, const QColor& lightColor );

    bool isMonochrome() const;

    QColor lightColor() const;
    QColor darkColor() const;

    // -1.0: sunken, 0.0: flat, 1.0: raised
    void setShadeFactor( double );
    double shadeFactor() const;

private:
    double m_width;
    double m_shadeFactor;
    QColor m_colorDark;
    QColor m_colorLight;
};

#endif
