/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FONT_ROLE_H
#define QSK_FONT_ROLE_H

#include "QskGlobal.h"
#include <qmetatype.h>

class QSK_EXPORT QskFontRole
{
    Q_GADGET

    Q_PROPERTY( Category category READ category WRITE setCategory )
    Q_PROPERTY( Emphasis emphasis READ emphasis WRITE setEmphasis )

  public:
    enum Category : quint8
    {
        Caption,
        Body,
        Subtitle,
        Title,
        Headline,
        Display
    };
    Q_ENUM( Category );

    enum Emphasis : quint8
    {
        VeryLow,
        Low,

        Normal,

        High,
        VeryHigh
    };
    Q_ENUM( Emphasis );

    constexpr QskFontRole( Category = Body, Emphasis = Normal ) noexcept;

    constexpr bool operator==( const QskFontRole& ) const noexcept;
    constexpr bool operator!=( const QskFontRole& ) const noexcept;

    void setCategory( Category ) noexcept;
    constexpr Category category() const noexcept;

    void setEmphasis( Emphasis ) noexcept;
    constexpr Emphasis emphasis() const noexcept;

    QskHashValue hash( QskHashValue seed = 0 ) const noexcept;

  private:
    Category m_category;
    Emphasis m_emphasis;
};

inline constexpr QskFontRole::QskFontRole( Category category, Emphasis emphasis ) noexcept
    : m_category( category )
    , m_emphasis( emphasis )
{
}

inline constexpr bool QskFontRole::operator==( const QskFontRole& other ) const noexcept
{
    return ( m_category == other.m_category ) && ( m_emphasis == other.m_emphasis );
}

inline constexpr bool QskFontRole::operator!=( const QskFontRole& other ) const noexcept
{
    return !( *this == other );
}

inline void QskFontRole::setCategory( Category category ) noexcept
{
    m_category = category;
}

inline constexpr QskFontRole::Category QskFontRole::category() const noexcept
{
    return m_category;
}

inline void QskFontRole::setEmphasis( Emphasis emphasis ) noexcept
{
    m_emphasis = emphasis;
}

inline constexpr QskFontRole::Emphasis QskFontRole::emphasis() const noexcept
{
    return m_emphasis;
}

inline QskHashValue qHash( const QskFontRole fontRole, QskHashValue seed = 0 ) noexcept
{
    return fontRole.hash( seed );
}

namespace std
{
    template< > struct hash< QskFontRole >
    {
        size_t operator()( const QskFontRole& fontRole ) const noexcept
        {
            return fontRole.hash();
        }
    };
}

#ifndef QT_NO_DEBUG_STREAM

QSK_EXPORT QDebug operator<<( QDebug, const QskFontRole& );

#endif

Q_DECLARE_TYPEINFO( QskFontRole, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskFontRole )

#endif
