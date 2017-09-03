/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxColors.h"
#include "QskRgbValue.h"
#include "QskNamespace.h"

#include <QDebug>

static void qskRegisterBoxColors()
{
    qRegisterMetaType< QskBoxColors >();
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterBoxColors )

static inline void qskMirrorEdges(
    Qt::Orientations orientations, const QColor* from, QColor* to )
{
    if ( orientations == Qt::Vertical )
    {
        to[ Qt::TopLeftCorner ] = from[ Qt::BottomLeftCorner ];
        to[ Qt::BottomLeftCorner ] = from[ Qt::TopLeftCorner ];
        to[ Qt::TopRightCorner ] = from[ Qt::BottomRightCorner ];
        to[ Qt::BottomRightCorner ] = from[ Qt::TopRightCorner ];
    }
    else if ( orientations == Qt::Horizontal )
    {
        to[ Qt::TopLeftCorner ] = from[ Qt::TopRightCorner ];
        to[ Qt::BottomLeftCorner ] = from[ Qt::BottomRightCorner ];
        to[ Qt::TopRightCorner ] = from[ Qt::TopLeftCorner ];
        to[ Qt::BottomRightCorner ] = from[ Qt::BottomLeftCorner ];
    }
    else if ( orientations == ( Qt::Vertical | Qt::Horizontal ) )
    {
        to[ Qt::TopLeftCorner ] = from[ Qt::BottomRightCorner ];
        to[ Qt::BottomLeftCorner ] = from[ Qt::TopRightCorner ];
        to[ Qt::TopRightCorner ] = from[ Qt::BottomLeftCorner ];
        to[ Qt::BottomRightCorner ] = from[ Qt::TopLeftCorner ];
    }
    else
    {
        to[ Qt::TopLeftCorner ] = from[ Qt::TopLeftCorner ];
        to[ Qt::BottomLeftCorner ] = from[ Qt::BottomLeftCorner ];
        to[ Qt::TopRightCorner ] = from[ Qt::TopRightCorner ];
        to[ Qt::BottomRightCorner ] = from[ Qt::BottomRightCorner ];
    }
}

QskBoxColors::QskBoxColors()
{
}

QskBoxColors::QskBoxColors( const QColor& border, const QColor& fill )
{
    setBorderColor( border );
    setFillColor( fill );
}

QskBoxColors::~QskBoxColors()
{
}

bool QskBoxColors::operator==( const QskBoxColors& other ) const
{
    for ( size_t i = 0; i < 4; i++ )
    {
        if ( ( m_border[i] != other.m_border[i] )
            || ( m_fill[i] != other.m_fill[i] ) )
        {
            return false;
        }
    }

    return true;
}

void QskBoxColors::setBorderColor( const QColor& color )
{
    m_border[ 0 ] = m_border[ 1 ] = m_border[ 2 ] = m_border[ 3 ] = color.toRgb();
}

void QskBoxColors::setBorderColor( const QColor& left, const QColor& top,
    const QColor& right, const QColor& bottom )
{
    m_border[ Qsk::Left ] = left.toRgb();
    m_border[ Qsk::Top ] = top.toRgb();
    m_border[ Qsk::Right ] = right.toRgb();
    m_border[ Qsk::Bottom ] = bottom.toRgb();
}

void QskBoxColors::setBorderColor( Qt::Edges edges, const QColor& color )
{
    const QColor c = color.toRgb();

    if ( edges & Qt::TopEdge )
        m_border[ Qsk::Top ] = c;

    if ( edges & Qt::LeftEdge )
        m_border[ Qsk::Left ] = c;

    if ( edges & Qt::RightEdge )
        m_border[ Qsk::Right ] = c;

    if ( edges & Qt::BottomEdge )
        m_border[ Qsk::Bottom ] = c;
}

QColor QskBoxColors::borderColor( Qt::Edge edge ) const
{
    switch( edge )
    {
        case Qt::TopEdge:
            return m_border[ Qsk::Top ];

        case Qt::LeftEdge:
            return m_border[ Qsk::Left ];

        case Qt::RightEdge:
            return m_border[ Qsk::Right ];

        case Qt::BottomEdge:
            return m_border[ Qsk::Bottom ];
    }

    return QColor();
}

void QskBoxColors::setFillColor( const QColor& color )
{
    m_fill[ 0 ] = m_fill[ 1 ] = m_fill[ 2 ] = m_fill[ 3 ] = color.toRgb();
}

void QskBoxColors::setFillColor(
    const QColor& topLeft, const QColor& topRight,
    const QColor& bottomLeft, const QColor& bottomRight )
{
    m_fill[ Qt::TopLeftCorner ] = topLeft.toRgb();
    m_fill[ Qt::TopRightCorner ] = topRight.toRgb();
    m_fill[ Qt::BottomLeftCorner ] = bottomLeft.toRgb();
    m_fill[ Qt::BottomRightCorner ] = bottomRight.toRgb();
}

void QskBoxColors::setFillColor( Qt::Corner corner, const QColor& color )
{
    if ( corner >= 0 && corner < 4 )
        m_fill[ corner ] = color.toRgb();
}

void QskBoxColors::setFillColor( Qt::Edge edge, const QColor& color )
{
    switch( edge )
    {
        case Qt::TopEdge:
        {
            m_fill[ Qt::TopLeftCorner ] = color;
            m_fill[ Qt::TopRightCorner ] = color;

            break;
        }
        case Qt::LeftEdge:
        {
            m_fill[ Qt::TopLeftCorner ] = color;
            m_fill[ Qt::BottomLeftCorner ] = color;

            break;
        }
        case Qt::RightEdge:
        {
            m_fill[ Qt::TopRightCorner ] = color;
            m_fill[ Qt::BottomRightCorner ] = color;

            break;
        }
        case Qt::BottomEdge:
        {
            m_fill[ Qt::BottomLeftCorner ] = color;
            m_fill[ Qt::BottomRightCorner ] = color;

            break;
        }
    }
}

QColor QskBoxColors::fillColor( Qt::Corner corner ) const
{
    if ( corner >= 0 && corner < 4 )
        return m_fill[ corner ];

    return QColor();
}

QskBoxColors QskBoxColors::shaded( uint alpha ) const
{
    QskBoxColors colors = *this;

    for ( int i = 0; i < 4; i++ )
    {
        colors.m_border[i].setAlpha( alpha );
        colors.m_fill[i].setAlpha( alpha );
    }

    return colors;
}

QskBoxColors QskBoxColors::mirrored( Qt::Orientations orientations ) const
{
    if ( orientations == 0 )
        return *this;

    QskBoxColors c;
    qskMirrorEdges( orientations, m_border, c.m_border );
    qskMirrorEdges( orientations, m_fill, c.m_fill );

    return c;
}

QskBoxColors QskBoxColors::rotated() const
{
    QskBoxColors c;

    c.m_border[ Qsk::Left ] = m_border[ Qsk::Top ];
    c.m_border[ Qsk::Top ] = m_border[ Qsk::Right ];
    c.m_border[ Qsk::Right ] = m_border[ Qsk::Bottom ];
    c.m_border[ Qsk::Bottom ] = m_border[ Qsk::Right ];

    c.m_fill[ Qt::TopLeftCorner ] = m_fill[ Qt::TopRightCorner ];
    c.m_fill[ Qt::TopRightCorner ] = m_fill[ Qt::BottomRightCorner ];
    c.m_fill[ Qt::BottomLeftCorner ] = m_fill[ Qt::TopLeftCorner ];
    c.m_fill[ Qt::BottomRightCorner ] = m_fill[ Qt::BottomLeftCorner ];

    return c;
}

QskBoxColors QskBoxColors::interpolated( const QskBoxColors& to, qreal ratio ) const
{
    QskBoxColors colors;

    for ( size_t i = 0; i < 4; i++ )
    {
        colors.m_border[i] = QskRgbValue::interpolated( m_border[i], to.m_border[i], ratio );
        colors.m_fill[i] = QskRgbValue::interpolated( m_fill[i], to.m_fill[i], ratio );
    }

    return colors;
}

QVariant QskBoxColors::interpolate(
    const QskBoxColors& from, const QskBoxColors& to, qreal ratio )
{
    return QVariant::fromValue( from.interpolated( to, ratio ) );
}

#ifndef QT_NO_DEBUG_STREAM

static inline void qskDebugColor( QDebug debug, const QColor& c )
{
    debug << '('
        << c.red() << ','
        << c.green() << ','
        << c.blue() << ','
        << c.alpha() << ')';
}

QDebug operator<<( QDebug debug, const QskBoxColors& colors )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "BoxColors" << '(';

    debug << "\n  Border" << '(';

    debug << " L";
    qskDebugColor( debug, colors.borderColor( Qt::LeftEdge ) );

    debug << ", T";
    qskDebugColor( debug, colors.borderColor( Qt::TopEdge ) );

    debug << ", R";
    qskDebugColor( debug, colors.borderColor( Qt::RightEdge ) );

    debug << ", B";
    qskDebugColor( debug, colors.borderColor( Qt::BottomEdge ) );

    debug << " )";

    debug << "\n  Fill" << '(';

    debug << " TL";
    qskDebugColor( debug, colors.fillColor( Qt::TopLeftCorner ) );

    debug << ", TR";
    qskDebugColor( debug, colors.fillColor( Qt::TopRightCorner ) );

    debug << ", BL";
    qskDebugColor( debug, colors.fillColor( Qt::BottomLeftCorner ) );

    debug << ", BR";
    qskDebugColor( debug, colors.fillColor( Qt::BottomRightCorner ) );

    debug << " )";

    debug << "\n)";

    return debug;
}

#endif

