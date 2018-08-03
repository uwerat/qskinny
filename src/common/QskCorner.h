/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_CORNER_H
#define QSK_CORNER_H

#include "QskBoxShapeMetrics.h"

#include <qmetatype.h>
#include <qnamespace.h>

class QDebug;
class QSizeF;

class QSK_EXPORT QskCorner
{
    Q_GADGET

    Q_PROPERTY( qreal radius READ radius WRITE setRadius )
    Q_PROPERTY( Qt::SizeMode sizeMode READ sizeMode WRITE setSizeMode )
    Q_PROPERTY( Qt::AspectRatioMode aspectRatioMode READ aspectRatioMode WRITE setAspectRatioMode )

  public:
    QskCorner();
    QskCorner( Qt::SizeMode, qreal radius );

    qreal radius() const;
    void setRadius( qreal width );

    Qt::SizeMode sizeMode() const;
    void setSizeMode( Qt::SizeMode );

    Qt::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode( Qt::AspectRatioMode );

    const QskBoxShapeMetrics& metrics() const;

  private:
    // QskCorner could be replaced by QskBoxShapeMetrics TODO ...
    QskBoxShapeMetrics m_metrics;
};

inline const QskBoxShapeMetrics& QskCorner::metrics() const
{
    return m_metrics;
}

Q_DECLARE_TYPEINFO( QskCorner, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskCorner )

#endif
