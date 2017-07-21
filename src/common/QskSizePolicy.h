/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SIZE_POLICY_H_
#define QSK_SIZE_POLICY_H_

#include "QskGlobal.h"
#include <QMetaType>

class QDebug;

class QSK_EXPORT QskSizePolicy
{
    Q_GADGET

    Q_PROPERTY( Policy horizontalPolicy READ horizontalPolicy WRITE setHorizontalPolicy )
    Q_PROPERTY( Policy verticalPolicy READ verticalPolicy WRITE setVerticalPolicy )

public:
    enum Flag
    {
        GrowFlag = 1,
        ExpandFlag = 2,
        ShrinkFlag = 4,
        IgnoreFlag = 8
    };

    enum Policy
    {
        Fixed = 0,
        Minimum = GrowFlag,
        Maximum = ShrinkFlag,
        Preferred = GrowFlag | ShrinkFlag,
        MinimumExpanding = GrowFlag | ExpandFlag,
        Expanding = GrowFlag | ShrinkFlag | ExpandFlag,
        Ignored = ShrinkFlag | GrowFlag | IgnoreFlag,
        Constrained = IgnoreFlag
    };

    Q_ENUM( Flag )
    Q_ENUM( Policy )

    QskSizePolicy();
    QskSizePolicy( Policy horizontalPolicy, Policy verticalPolicy );

    bool operator==( const QskSizePolicy& other ) const;
    bool operator!=( const QskSizePolicy& other ) const;

    void setHorizontalPolicy( Policy );
    Policy horizontalPolicy() const;

    void setVerticalPolicy( Policy );
    Policy verticalPolicy() const;

    Policy policy( Qt::Orientation ) const;
    void setPolicy( Qt::Orientation, Policy );

private:
    quint32 m_horizontalPolicy : 4;
    quint32 m_verticalPolicy : 4;
};

inline QskSizePolicy::QskSizePolicy():
    m_horizontalPolicy( Ignored ),
    m_verticalPolicy( Ignored )
{
}

inline QskSizePolicy::QskSizePolicy( Policy horizontalPolicy, Policy verticalPolicy ):
    m_horizontalPolicy( horizontalPolicy ),
    m_verticalPolicy( verticalPolicy )
{
}

inline bool QskSizePolicy::operator==( const QskSizePolicy& other ) const
{
    return ( other.m_horizontalPolicy == m_horizontalPolicy )
        && ( other.m_verticalPolicy == m_verticalPolicy );
}

inline bool QskSizePolicy::operator!=( const QskSizePolicy& other ) const
{
    return !( *this == other );
}

inline void QskSizePolicy::setHorizontalPolicy( Policy policy )
{
    m_horizontalPolicy = static_cast< quint32 >( policy );
}

inline QskSizePolicy::Policy QskSizePolicy::horizontalPolicy() const
{
    return static_cast< Policy >( m_horizontalPolicy );
}

inline void QskSizePolicy::setVerticalPolicy( Policy policy )
{
    m_verticalPolicy = static_cast< quint32 >( policy );
}

inline QskSizePolicy::Policy QskSizePolicy::verticalPolicy() const
{
    return static_cast< Policy >( m_verticalPolicy );
}

#ifndef QT_NO_DEBUG_STREAM
QSK_EXPORT QDebug operator<<( QDebug, const QskSizePolicy& );
#endif

Q_DECLARE_TYPEINFO( QskSizePolicy, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskSizePolicy )

#endif
