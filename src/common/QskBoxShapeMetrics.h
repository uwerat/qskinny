/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_SHAPE_METRICS_H
#define QSK_BOX_SHAPE_METRICS_H

#include "QskGlobal.h"

#include <Qt>
#include <QMetaType>
#include <QSizeF>

class QDebug;
class QVariant;
class QMarginsF;

class QSK_EXPORT QskBoxShapeMetrics
{
public:
    QskBoxShapeMetrics();
    QskBoxShapeMetrics( qreal radius, Qt::SizeMode = Qt::AbsoluteSize );
    QskBoxShapeMetrics( qreal radiusX, qreal radiusY, Qt::SizeMode = Qt::AbsoluteSize );

    ~QskBoxShapeMetrics();

    bool operator==( const QskBoxShapeMetrics& ) const;
    bool operator!=( const QskBoxShapeMetrics& ) const;

    void setRadius( qreal radius );
    void setRadius( qreal radius, Qt::SizeMode );
    void setRadius( Qt::Corner, qreal radius );

    void setRadius( qreal radiusX, qreal radiusY );
    void setRadius( const QSizeF& );
    void setRadius( Qt::Corner, qreal radiusX, qreal radiusY );
    void setRadius( Qt::Corner, const QSizeF& radius );

    void setRadius( qreal topLeft, qreal topRight,
        qreal bottomLeft, qreal bottomRight );

    void setRadius( const QSizeF& topLeft, const QSizeF& topRight,
        const QSizeF& bottomLeft, const QSizeF& bottomRight );

    void setRadius(
        qreal topLeftX, qreal topLeftY,
        qreal topRightX, qreal topRightY,
        qreal bottomLeftX, qreal bottomLeftY,
        qreal bottomRightX, qreal bottomRightY );

    QSizeF radius( Qt::Corner ) const;

    bool isRectangle() const;
    bool isRectellipse() const;

    void setSizeMode( Qt::SizeMode );
    Qt::SizeMode sizeMode() const;

    void setAspectRatioMode( Qt::AspectRatioMode );
    Qt::AspectRatioMode aspectRatioMode() const;

    QskBoxShapeMetrics interpolated( const QskBoxShapeMetrics&, qreal value ) const;
    QskBoxShapeMetrics toAbsolute ( const QSizeF& ) const;
    QskBoxShapeMetrics rotated() const;

    QMarginsF padding( const QSizeF& ) const;

    uint hash( uint seed = 0 ) const;

    static QVariant interpolate( const QskBoxShapeMetrics&,
        const QskBoxShapeMetrics&, qreal progress );

private:
    QskBoxShapeMetrics( Qt::SizeMode sizeMode, const QSizeF radii[4] ):
        m_radii( { radii[0], radii[1], radii[2], radii[3] } ),
        m_sizeMode( sizeMode )
    {
    }

    QSizeF m_radii[ 4 ];
    Qt::SizeMode m_sizeMode : 2;
    Qt::AspectRatioMode m_aspectRatioMode : 2;
};

inline QskBoxShapeMetrics::QskBoxShapeMetrics():
    m_radii( { { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 } } ),
    m_sizeMode( Qt::AbsoluteSize )
{
}

inline QskBoxShapeMetrics::QskBoxShapeMetrics( qreal radius, Qt::SizeMode sizeMode ):
    QskBoxShapeMetrics( radius, radius, sizeMode )
{
}

inline bool QskBoxShapeMetrics::operator!=( const QskBoxShapeMetrics& other ) const
{
    return !( *this == other );
}

inline void QskBoxShapeMetrics::setRadius( qreal radius )
{
    setRadius( radius, radius );
}

inline void QskBoxShapeMetrics::setRadius( qreal radius, Qt::SizeMode sizeMode )
{
    setRadius( radius );
    setSizeMode( sizeMode );
}

inline void QskBoxShapeMetrics::setRadius( qreal radiusX, qreal radiusY )
{
    setRadius( radiusX, radiusY, radiusX, radiusY,
        radiusX, radiusY, radiusX, radiusY );
}

inline void QskBoxShapeMetrics::setRadius( const QSizeF& radius )
{
    setRadius( radius.width(), radius.height() );
}

inline void QskBoxShapeMetrics::setRadius( Qt::Corner corner, qreal radius )
{
    setRadius( corner, radius, radius );
}

inline void QskBoxShapeMetrics::setRadius( Qt::Corner corner, const QSizeF& radius )
{
    setRadius( corner, radius.width(), radius.height() );
}

inline void QskBoxShapeMetrics::setRadius(
    qreal topLeft, qreal topRight,
    qreal bottomLeft, qreal bottomRight )
{
    setRadius( topLeft, topLeft, topRight, topRight,
        bottomLeft, bottomLeft, bottomRight, bottomRight );
}

inline void QskBoxShapeMetrics::setRadius(
    const QSizeF& topLeft, const QSizeF& topRight,
    const QSizeF& bottomLeft, const QSizeF& bottomRight )
{
    setRadius( topLeft.width(), topLeft.height(),
        topRight.width(), topRight.height(),
        bottomLeft.width(), bottomLeft.height(),
        bottomRight.width(), bottomRight.height() );
}

inline QSizeF QskBoxShapeMetrics::radius( Qt::Corner corner ) const
{
    if ( ( corner >= Qt::TopLeftCorner ) && ( corner <= Qt::BottomRightCorner ) )
        return m_radii[ corner ];

    return QSizeF();
}

inline Qt::SizeMode QskBoxShapeMetrics::sizeMode() const
{
    return m_sizeMode;
}

inline Qt::AspectRatioMode QskBoxShapeMetrics::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

#ifndef QT_NO_DEBUG_STREAM

QSK_EXPORT QDebug operator<<( QDebug, const QskBoxShapeMetrics& );

#endif

Q_DECLARE_TYPEINFO( QskBoxShapeMetrics, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskBoxShapeMetrics )

#endif
