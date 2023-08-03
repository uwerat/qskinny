/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskGlobal.h>
#include <QskGradient.h>
#include <qstring.h>
#include <qmetatype.h>

class ChartSample
{
    Q_GADGET

    Q_PROPERTY( QString title READ title WRITE setTitle )
    Q_PROPERTY( qreal value READ value WRITE setValue )
    Q_PROPERTY( QskGradient gradient READ gradient WRITE setGradient )

  public:
    ChartSample() noexcept = default;
    ChartSample( const QString&, qreal, const QskGradient& ) noexcept;

    void setSample( const QString&, qreal, const QskGradient& ) noexcept;

    bool operator==( const ChartSample& ) const noexcept;
    bool operator!=( const ChartSample& ) const noexcept;

    QString title() const noexcept;
    void setTitle( const QString& ) noexcept;

    qreal value() const noexcept;
    void setValue( qreal ) noexcept;

    QskGradient gradient() const noexcept;
    void setGradient( const QskGradient& ) noexcept;

  private:
    QString m_title;
    qreal m_value = 0.0;
    QskGradient m_gradient;
};

Q_DECLARE_METATYPE( ChartSample )

inline ChartSample::ChartSample( const QString& title, qreal value,
        const QskGradient& gradient ) noexcept
    : m_title( title )
    , m_value( value )
    , m_gradient( gradient )
{
}

inline bool ChartSample::operator==( const ChartSample& other ) const noexcept
{
    return ( m_value == other.m_value )
        && ( m_title == other.m_title )
        && ( m_gradient == other.m_gradient );
}

inline bool ChartSample::operator!=( const ChartSample& other ) const noexcept
{
    return ( !( *this == other ) );
}

inline QString ChartSample::title() const noexcept
{
    return m_title;
}

inline void ChartSample::setTitle( const QString& title ) noexcept
{
    m_title = title;
}

inline qreal ChartSample::value() const noexcept
{
    return m_value;
}

inline void ChartSample::setValue( qreal value ) noexcept
{
    m_value = value;
}

inline QskGradient ChartSample::gradient() const noexcept
{
    return m_gradient;
}

inline void ChartSample::setGradient( const QskGradient& gradient ) noexcept
{
    m_gradient = gradient;
}

inline void ChartSample::setSample(
    const QString& title, qreal value, const QskGradient& gradient ) noexcept
{
    m_title = title;
    m_value = value;
    m_gradient = gradient;
}

#ifndef QT_NO_DEBUG_STREAM

    class QDebug;
    QDebug operator<<( QDebug, const ChartSample& );

#endif
