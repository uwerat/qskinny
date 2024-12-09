/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextColors.h"
#include "QskRgbValue.h"

#include <qhashfunctions.h>
#include <qvariant.h>

QskHashValue QskTextColors::hash( QskHashValue seed ) const noexcept
{
    const QRgb rgb[] =
    {
        m_textColor.rgba(),
        m_styleColor.isValid() ? m_styleColor.rgba() : m_textColor.rgba(),
        m_linkColor.isValid() ? m_linkColor.rgba() : m_textColor.rgba()
    };

    return qHashBits( rgb, sizeof( rgb ), seed );
}

void QskTextColors::setTextColor( const QColor& color )
{
    m_textColor = color;
}

void QskTextColors::setStyleColor( const QColor& color )
{
    m_styleColor = color;
}

void QskTextColors::setLinkColor( const QColor& color )
{
    m_linkColor = color;
}

QskTextColors QskTextColors::interpolated(
    const QskTextColors& to, qreal ratio ) const
{
    QskTextColors colors;
    colors.m_textColor = QskRgb::interpolated( m_textColor, to.m_textColor, ratio );
    colors.m_styleColor = QskRgb::interpolated( m_styleColor, to.m_styleColor, ratio );
    colors.m_linkColor = QskRgb::interpolated( m_linkColor, to.m_linkColor, ratio );

    return colors;
}

QVariant QskTextColors::interpolate(
    const QskTextColors& from, const QskTextColors& to, qreal ratio )
{
    return QVariant::fromValue( from.interpolated( to, ratio ) );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

static inline void qskDebugColor( QDebug debug, const QColor& c )
{
    debug << '(' << c.red() << ',' << c.green() << ','
        << c.blue() << ',' << c.alpha() << ')';
}

QDebug operator<<( QDebug debug, const QskTextColors& colors )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "TextColors" << '(';

    debug << " T";

    if ( colors.textColor().isValid() )
        qskDebugColor( debug, colors.textColor() );
    else
        debug << "(invalid)";

    if ( colors.styleColor().isValid() )
    {
        debug << ", S";
        qskDebugColor( debug, colors.styleColor() );
    }

    if ( colors.linkColor().isValid() )
    {
        debug << ", L";
        qskDebugColor( debug, colors.linkColor() );
    }

    debug << " )";

    return debug;
}

#endif

#include "moc_QskTextColors.cpp"
