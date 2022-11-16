/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxBorderColors.h"
#include "QskRgbValue.h"

#include <qhashfunctions.h>
#include <qvariant.h>

static void qskRegisterBoxBorderColors()
{
    qRegisterMetaType< QskBoxBorderColors >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskBoxBorderColors >();
#endif

    QMetaType::registerConverter< QColor, QskBoxBorderColors >(
        []( const QColor& color ) { return QskBoxBorderColors( color ); } );

    QMetaType::registerConverter< QskGradient, QskBoxBorderColors >(
        []( const QskGradient& gradient ) { return QskBoxBorderColors( gradient ); } );
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterBoxBorderColors )

QskBoxBorderColors::QskBoxBorderColors()
{
}

QskBoxBorderColors::QskBoxBorderColors(
        const QskGradient& left, const QskGradient& top,
        const QskGradient& right, const QskGradient& bottom )
    : m_gradients { top, left, right, bottom }
{
}

QskBoxBorderColors::QskBoxBorderColors( const QskGradient& gradient )
    : m_gradients { gradient, gradient, gradient, gradient }
{
}

QskBoxBorderColors::~QskBoxBorderColors()
{
}

bool QskBoxBorderColors::operator==( const QskBoxBorderColors& other ) const
{
    return ( m_gradients[ 0 ] == other.m_gradients[ 0 ] ) &&
           ( m_gradients[ 1 ] == other.m_gradients[ 1 ] ) &&
           ( m_gradients[ 2 ] == other.m_gradients[ 2 ] ) &&
           ( m_gradients[ 3 ] == other.m_gradients[ 3 ] );
}

void QskBoxBorderColors::setAlpha( int alpha )
{
    for ( int i = 0; i < 4; i++ )
    {
        if ( m_gradients[ i ].isValid() )
            m_gradients[ i ].setAlpha( alpha );
    }
}

void QskBoxBorderColors::setGradients( const QskGradient& gradient )
{
    m_gradients[ 0 ] = m_gradients[ 1 ] = m_gradients[ 2 ] = m_gradients[ 3 ] = gradient;
}

void QskBoxBorderColors::setGradients( const QskGradient& left, const QskGradient& top,
    const QskGradient& right, const QskGradient& bottom )
{
    m_gradients[ Top ] = top;
    m_gradients[ Left ] = left;
    m_gradients[ Right ] = right;
    m_gradients[ Bottom ] = bottom;
}

void QskBoxBorderColors::setGradientAt( Qt::Edges edges, const QskGradient& gradient )
{
    if ( edges & Qt::TopEdge )
        m_gradients[ Top ] = gradient;

    if ( edges & Qt::LeftEdge )
        m_gradients[ Left ] = gradient;

    if ( edges & Qt::RightEdge )
        m_gradients[ Right ] = gradient;

    if ( edges & Qt::BottomEdge )
        m_gradients[ Bottom ] = gradient;
}

void QskBoxBorderColors::setLeft( const QskGradient& gradient )
{
    m_gradients[ Left ] = gradient;
}

void QskBoxBorderColors::setTop( const QskGradient& gradient )
{
    m_gradients[ Top ] = gradient;
}

void QskBoxBorderColors::setRight( const QskGradient& gradient )
{
    m_gradients[ Right ] = gradient;
}

void QskBoxBorderColors::setBottom( const QskGradient& gradient )
{
    m_gradients[ Bottom ] = gradient;
}

const QskGradient& QskBoxBorderColors::gradientAt( Qt::Edge edge ) const
{
    switch ( edge )
    {
        case Qt::TopEdge:
            return m_gradients[ Top ];

        case Qt::LeftEdge:
            return m_gradients[ Left ];

        case Qt::RightEdge:
            return m_gradients[ Right ];

        case Qt::BottomEdge:
            return m_gradients[ Bottom ];
    }

    static QskGradient noGradient;
    return noGradient;
}

bool QskBoxBorderColors::isVisible() const
{
    if ( m_gradients[ 0 ].isVisible() )
        return true;

    if ( m_gradients[ 1 ].isVisible() )
        return true;

    if ( m_gradients[ 2 ].isVisible() )
        return true;

    if ( m_gradients[ 3 ].isVisible() )
        return true;

    return false;
}

bool QskBoxBorderColors::isMonochrome() const
{
    if ( m_gradients[ 1 ] != m_gradients[ 0 ] )
        return false;

    if ( m_gradients[ 2 ] != m_gradients[ 1 ] )
        return false;

    if ( m_gradients[ 3 ] != m_gradients[ 2 ] )
        return false;

    return m_gradients[ 0 ].isMonochrome()
        && m_gradients[ 1 ].isMonochrome()
        && m_gradients[ 2 ].isMonochrome()
        && m_gradients[ 3 ].isMonochrome();
}

bool QskBoxBorderColors::isValid() const
{
    return m_gradients[ 0 ].isValid()
        || m_gradients[ 1 ].isValid()
        || m_gradients[ 2 ].isValid()
        || m_gradients[ 3 ].isValid();
}

QskBoxBorderColors QskBoxBorderColors::interpolated(
    const QskBoxBorderColors& to, qreal ratio ) const
{
    QskBoxBorderColors colors;

    for ( size_t i = 0; i < 4; i++ )
    {
#if 1
        /*
            When one border has a width of 0 we would prefer to ignore
            the color and use always use the other color. TODO ...
         */
#endif
        colors.m_gradients[ i ] =
            m_gradients[ i ].interpolated( to.m_gradients[ i ], ratio );
    }

    return colors;
}

QVariant QskBoxBorderColors::interpolate(
    const QskBoxBorderColors& from, const QskBoxBorderColors& to, qreal ratio )
{
    return QVariant::fromValue( from.interpolated( to, ratio ) );
}

QskHashValue QskBoxBorderColors::hash( QskHashValue seed ) const
{
    auto h = m_gradients[ 0 ].hash( seed );
    h = m_gradients[ 1 ].hash( h );
    h = m_gradients[ 2 ].hash( h );
    h = m_gradients[ 3 ].hash( h );

    return h;
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskBoxBorderColors& colors )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "BoxBorderColors";

    if ( !colors.isValid() )
    {
        debug << "()";
    }
    else
    {
        debug << "( ";

        if ( colors.isMonochrome() )
        {
            const auto& gradient = colors.gradientAt( Qt::LeftEdge );
            QskRgb::debugColor( debug, gradient.rgbStart() );
        }
        else
        {
            using namespace Qt;

            const char prompts[] = { 'L', 'T', 'R', 'B' };
            const Edge edges[] = { LeftEdge, TopEdge, RightEdge, BottomEdge };

            for ( int i = 0; i < 4; i++ )
            {
                if ( i != 0 )
                    debug << ", ";

                const auto& gradient = colors.gradientAt( edges[i] );

                debug << prompts[ i ] << ": ";

                if ( gradient.isValid() && gradient.isMonochrome() )
                    QskRgb::debugColor( debug, gradient.rgbStart() );
                else
                    debug << gradient;
            }
        }

        debug << " )";
    }

    return debug;
}

#endif

#include "moc_QskBoxBorderColors.cpp"
