/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BORDER_METRICS_H
#define QSK_BORDER_HINTS_H

#include "QskGlobal.h"
#include "QskMargins.h"

#include <Qt>
#include <QMetaType>
#include <QSizeF>

class QDebug;
class QVariant;

class QSK_EXPORT QskBorderMetrics
{
public:
    QskBorderMetrics();
    QskBorderMetrics( qreal width, qreal radius = 0.0 );
    QskBorderMetrics( qreal width, qreal radiusX, qreal radiusY );

    ~QskBorderMetrics();

    bool operator==( const QskBorderMetrics& ) const;
    bool operator!=( const QskBorderMetrics& ) const;

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

    void setRadiusSizeMode( Qt::SizeMode );
    Qt::SizeMode radiusSizeMode() const;

    void setWidthAt( Qt::Edges, qreal width );
    qreal widthAt( Qt::Edge ) const;

    void setWidths( const QskMargins& );
    const QskMargins& widths() const;

    void setWidthSizeMode( Qt::SizeMode );
    Qt::SizeMode widthSizeMode() const;

    QskBorderMetrics interpolated( const QskBorderMetrics&, qreal value ) const;

    static QVariant interpolate( const QskBorderMetrics&,
        const QskBorderMetrics&, qreal progress );

private:
    QskBorderMetrics( bool widthIsRelative, const QskMargins& widths,
        bool radiusIsRelative, const QSizeF radii[4] ):
        m_widths( widths ),
        m_radii( { radii[0], radii[1], radii[2], radii[3] } ),
        m_widthIsRelative( widthIsRelative ),
        m_radiusIsRelative( radiusIsRelative )
    {
    }

    QskMargins m_widths;
    QSizeF m_radii[ 4 ];

    bool m_widthIsRelative;
    bool m_radiusIsRelative;
};

inline QskBorderMetrics::QskBorderMetrics():
    m_widthIsRelative( false ),
    m_radiusIsRelative( false )
{
}

inline QskBorderMetrics::QskBorderMetrics( qreal width, qreal radius ):
    QskBorderMetrics( width, radius, radius )
{
}

inline bool QskBorderMetrics::operator!=( const QskBorderMetrics& other ) const
{
    return !( *this == other );
}

inline void QskBorderMetrics::setRadius( qreal radius )
{
    setRadius( radius, radius );
}

inline void QskBorderMetrics::setRadius( qreal radius, Qt::SizeMode sizeMode )
{
    setRadius( radius );
    setRadiusSizeMode( sizeMode );
}

inline void QskBorderMetrics::setRadius( qreal radiusX, qreal radiusY )
{
    setRadius( radiusX, radiusY, radiusX, radiusY,
        radiusX, radiusY, radiusX, radiusY );
}

inline void QskBorderMetrics::setRadius( const QSizeF& radius )
{
    setRadius( radius.width(), radius.height() );
}

inline void QskBorderMetrics::setRadius( Qt::Corner corner, qreal radius )
{
    setRadius( corner, radius, radius );
}

inline void QskBorderMetrics::setRadius( Qt::Corner corner, const QSizeF& radius )
{
    setRadius( corner, radius.width(), radius.height() );
}

inline void QskBorderMetrics::setRadius(
    const QSizeF& topLeft, const QSizeF& topRight,
    const QSizeF& bottomLeft, const QSizeF& bottomRight )
{
    setRadius( topLeft.width(), topLeft.height(),
        topRight.width(), topRight.height(),
        bottomLeft.width(), bottomLeft.height(),
        bottomRight.width(), bottomRight.height() );
}

inline QSizeF QskBorderMetrics::radius( Qt::Corner corner ) const
{
    if ( ( corner >= Qt::TopLeftCorner ) && ( corner <= Qt::BottomRightCorner ) )
        return m_radii[ corner ];

    return QSizeF();
}

inline Qt::SizeMode QskBorderMetrics::radiusSizeMode() const
{
    return m_radiusIsRelative ? Qt::RelativeSize : Qt::AbsoluteSize;
}

inline const QskMargins& QskBorderMetrics::widths() const
{
    return m_widths;
}

inline qreal QskBorderMetrics::widthAt( Qt::Edge edge ) const
{
    return m_widths.marginAt( edge );
}

inline Qt::SizeMode QskBorderMetrics::widthSizeMode() const
{
    return m_widthIsRelative ? Qt::RelativeSize : Qt::AbsoluteSize;
}

#ifndef QT_NO_DEBUG_STREAM

QSK_EXPORT QDebug operator<<( QDebug, const QskBorderMetrics& );

#endif

Q_DECLARE_TYPEINFO( QskBorderMetrics, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskBorderMetrics )

#endif
