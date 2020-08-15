/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskColorFilter.h"
#include "QskRgbValue.h"

#include <qbrush.h>
#include <qpen.h>
#include <qvariant.h>

static inline QRgb qskSubstitutedRgb(
    const QVector< QPair< QRgb, QRgb > >& substitions, QRgb rgba )
{
    // usually we have 2-3 substitutions, so we can simply iterate
    // and don't need to introduce some sort of sorting or search index

    const QRgb rgb = rgba | QskRgb::AlphaMask;

    for ( const auto& s : substitions )
    {
        if ( rgb == s.first )
        {
            return ( s.second & QskRgb::ColorMask ) |
                ( rgba & QskRgb::AlphaMask );
        }
    }

    return rgba;
}

static inline QColor qskSubstitutedColor(
    const QVector< QPair< QRgb, QRgb > >& substitions, const QColor& color )
{
    return QColor::fromRgba( qskSubstitutedRgb( substitions, color.rgba() ) );
}

static inline QBrush qskSubstitutedBrush(
    const QVector< QPair< QRgb, QRgb > >& substitions, const QBrush& brush )
{
    QBrush newBrush;

    const QGradient* gradient = brush.gradient();
    if ( gradient )
    {
        bool isModified = false;

        QGradientStops stops = gradient->stops();
        for ( auto& stop : stops )
        {
            const QColor c = qskSubstitutedColor( substitions, stop.second );
            if ( c != stop.second )
            {
                stop.second = c;
                isModified = true;
            }
        }

        if ( isModified )
        {
            newBrush = brush;

            QGradient* newGradient = const_cast< QGradient* >( newBrush.gradient() );
            newGradient->setStops( stops );
        }
    }
    else
    {
        const QColor c = qskSubstitutedColor( substitions, brush.color() );
        if ( c != brush.color() )
        {
            newBrush = brush;
            newBrush.setColor( c );
        }
    }

    return newBrush;
}

static inline QskColorFilter qskInterpolatedFilter(
    const QskColorFilter& from, const QskColorFilter& to, qreal progress )
{
    if ( progress <= 0.0 )
        return from;

    if ( progress >= 1.0 )
        return to;

    if ( from == to )
        return from;

    QskColorFilter interpolated;

    for ( const auto& pairTo : to.substitutions() )
    {
        QRgb rgb = pairTo.first;

        for ( auto pairFrom : from.substitutions() )
        {
            if ( pairTo.first == pairFrom.first )
            {
                rgb = pairFrom.second;
                break;
            }
        }

        rgb = QskRgb::interpolated( rgb, pairTo.second, progress );

        if ( rgb != pairTo.first )
            interpolated.addColorSubstitution( pairTo.first, rgb );
    }

    /*
        we might have substitutaions in from for colors we don't
        have in to. Then the target color is the original color
        of the substitution in from.
     */

    for ( const auto& pairFrom : from.substitutions() )
    {
        bool hasRgb = false;

        for ( const auto& pairTo : to.substitutions() )
        {
            if ( pairTo.first == pairFrom.first )
            {
                hasRgb = true;
                break;
            }
        }

        if ( !hasRgb )
        {
            const auto rgb = QskRgb::interpolated(
                pairFrom.second, pairFrom.first, progress );

            if ( rgb != pairFrom.first )
                interpolated.addColorSubstitution( pairFrom.first, rgb );
        }
    }

    return interpolated;
}

void QskColorFilter::addColorSubstitution( QRgb from, QRgb to )
{
    for ( auto& substitution : m_substitutions )
    {
        if ( substitution.first == from )
        {
            substitution.second = to;
            return;
        }
    }

    m_substitutions += qMakePair( from, to );
}

void QskColorFilter::reset()
{
    m_substitutions.clear();
}

QPen QskColorFilter::substituted( const QPen& pen ) const
{
    if ( m_substitutions.isEmpty() || pen.style() == Qt::NoPen )
        return pen;

    const QBrush newBrush = qskSubstitutedBrush( m_substitutions, pen.brush() );
    if ( newBrush.style() == Qt::NoBrush )
        return pen;

    QPen newPen = pen;
    newPen.setBrush( newBrush );
    return newPen;
}

QBrush QskColorFilter::substituted( const QBrush& brush ) const
{
    if ( m_substitutions.isEmpty() || brush.style() == Qt::NoBrush )
        return brush;

    const QBrush newBrush = qskSubstitutedBrush( m_substitutions, brush );
    return ( newBrush.style() != Qt::NoBrush ) ? newBrush : brush;
}

QColor QskColorFilter::substituted( const QColor& color ) const
{
    return qskSubstitutedColor( m_substitutions, color );
}

QRgb QskColorFilter::substituted( const QRgb& rgb ) const
{
    return qskSubstitutedRgb( m_substitutions, rgb );
}

QskColorFilter QskColorFilter::interpolated(
    const QskColorFilter& other, qreal progress ) const
{
    return qskInterpolatedFilter( *this, other, progress );
}

QVariant QskColorFilter::interpolate(
    const QskColorFilter& from, const QskColorFilter& to, qreal progress )
{
    return QVariant::fromValue( qskInterpolatedFilter( from, to, progress ) );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskColorFilter& filter )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "Filter" << '(';
    for ( const auto& s : filter.substitutions() )
        debug << '[' << s.first << "->" << s.second << "]";
    debug << ')';

    return debug;
}

#endif
