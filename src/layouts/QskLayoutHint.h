/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LAYOUT_HINT_H
#define QSK_LAYOUT_HINT_H

#include "QskGlobal.h"
#include <qglobal.h>
#include <limits>

class QDebug;

class QSK_EXPORT QskLayoutHint
{
  public:
    QskLayoutHint();
    QskLayoutHint( qreal minimum, qreal preferred, qreal maximum );

    void normalize();

    QskLayoutHint normalized() const;
    bool isDefault() const;

    qreal size( int which ) const;
    void setSize( int which, qreal size );

    bool operator==( const QskLayoutHint& ) const;
    bool operator!=( const QskLayoutHint& ) const;

    void setMinimum( qreal value );
    qreal minimum() const;

    void setPreferred( qreal value );
    qreal preferred() const;

    void setMaximum( qreal value );
    qreal maximum() const;

    void setSizes( qreal minimum, qreal preferred, qreal maximum );
    void expandTo( const QskLayoutHint& );

    void expandMinimum( qreal value );
    void expandPreferred( qreal value );
    void expandMaximum( qreal value );

    static qreal combined( int which, qreal value1, qreal value2 );
    static constexpr qreal unlimited = std::numeric_limits< float >::max();

  private:
    qreal m_minimum;
    qreal m_preferred;
    qreal m_maximum;
};

inline qreal QskLayoutHint::minimum() const
{
    return m_minimum;
}

inline void QskLayoutHint::setMinimum( qreal value )
{
    m_minimum = value;
}

inline qreal QskLayoutHint::preferred() const
{
    return m_preferred;
}

inline void QskLayoutHint::setPreferred( qreal value )
{
    m_preferred = value;
}

inline qreal QskLayoutHint::maximum() const
{
    return m_maximum;
}

inline void QskLayoutHint::setMaximum( qreal value )
{
    m_maximum = value;
}

inline void QskLayoutHint::expandMinimum( qreal value )
{
    if ( value > m_minimum )
        m_minimum = value;
}

inline void QskLayoutHint::expandPreferred( qreal value )
{
    if ( value > m_preferred )
        m_preferred = value;
}

inline void QskLayoutHint::expandMaximum( qreal value )
{
    if ( value > m_maximum )
        m_maximum = value;
}

inline void QskLayoutHint::setSizes(
    qreal minimum, qreal preferred, qreal maximum )
{
    m_minimum = minimum;
    m_preferred = preferred;
    m_maximum = maximum;
}

inline bool QskLayoutHint::operator==( const QskLayoutHint& other ) const
{
    return ( m_preferred == other.m_preferred )
        && ( m_minimum == other.m_minimum )
        && ( m_maximum == other.m_maximum );
}

inline bool QskLayoutHint::operator!=( const QskLayoutHint& other ) const
{
    return !( *this == other );
}

inline QskLayoutHint QskLayoutHint::normalized() const
{
    auto hint = *this;
    hint.normalize();
    return hint;
}

#ifndef QT_NO_DEBUG_STREAM

QSK_EXPORT QDebug operator<<( QDebug, const QskLayoutHint& );

#endif

Q_DECLARE_TYPEINFO( QskLayoutHint, Q_MOVABLE_TYPE );

#endif
