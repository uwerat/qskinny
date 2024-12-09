/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_OPTIONS_H
#define QSK_TEXT_OPTIONS_H

#include "QskGlobal.h"

#include <qmetatype.h>
#include <qtextoption.h>

#include <limits>

class QSK_EXPORT QskTextOptions
{
    Q_GADGET

    Q_PROPERTY( TextFormat format READ format WRITE setFormat )
    Q_PROPERTY( Qt::TextElideMode elideMode READ elideMode WRITE setElideMode )
    Q_PROPERTY( WrapMode wrapMode READ wrapMode WRITE setWrapMode )
    Q_PROPERTY( FontSizeMode fontSizeMode READ fontSizeMode WRITE setFontSizeMode )
    Q_PROPERTY( int maximumLineCount READ maximumLineCount WRITE setMaximumLineCount )

  public:
    enum FontSizeMode : quint8
    {
        FixedSize,
        HorizontalFit,
        VerticalFit,
        Fit
    };
    Q_ENUM( FontSizeMode )

    enum WrapMode : quint8
    {
        NoWrap = QTextOption::NoWrap,
        WordWrap = QTextOption::WordWrap,
        WrapAnywhere = QTextOption::WrapAnywhere,
        Wrap = QTextOption::WrapAtWordBoundaryOrAnywhere
    };
    Q_ENUM( WrapMode )

    enum TextFormat : quint8
    {
        PlainText = Qt::PlainText,
        RichText = Qt::RichText,
        AutoText = Qt::AutoText,
        MarkdownText = Qt::MarkdownText,
        StyledText = 4 // taken from QQuickText::StyledText
    };
    Q_ENUM( TextFormat )

    constexpr QskTextOptions() noexcept;

    constexpr TextFormat format() const noexcept;
    void setFormat( TextFormat ) noexcept;

    TextFormat effectiveFormat( const QString& text ) const;

    constexpr Qt::TextElideMode elideMode() const noexcept;
    void setElideMode( Qt::TextElideMode ) noexcept;

    constexpr Qt::TextElideMode effectiveElideMode() const noexcept;

    constexpr FontSizeMode fontSizeMode() const noexcept;
    void setFontSizeMode( FontSizeMode ) noexcept;

    constexpr WrapMode wrapMode() const noexcept;
    void setWrapMode( WrapMode ) noexcept;

    constexpr int maximumLineCount() const noexcept;
    void setMaximumLineCount( int ) noexcept;

    constexpr bool operator==( const QskTextOptions& other ) const noexcept;
    constexpr bool operator!=( const QskTextOptions& other ) const noexcept;

    QskHashValue hash( QskHashValue seed ) const noexcept;

    int textFlags() const noexcept;

  private:
    int m_maximumLineCount;
    unsigned int m_fontSizeMode : 2;
    unsigned int m_wrapMode : 4;
    unsigned int m_format : 3;
    unsigned int m_elideMode : 2;
};

inline constexpr QskTextOptions::QskTextOptions() noexcept
    : m_maximumLineCount( std::numeric_limits< int >::max() )
    , m_fontSizeMode( QskTextOptions::FixedSize )
    , m_wrapMode( QskTextOptions::NoWrap )
    , m_format( PlainText ) // AutoText ???
    , m_elideMode( Qt::ElideNone )
{
}

inline void QskTextOptions::setFormat( TextFormat format ) noexcept
{
    m_format = format;
}

constexpr inline QskTextOptions::TextFormat QskTextOptions::format() const noexcept
{
    return static_cast< QskTextOptions::TextFormat >( m_format );
}

inline void QskTextOptions::setElideMode( Qt::TextElideMode elideMode ) noexcept
{
    m_elideMode = elideMode;
}

constexpr inline Qt::TextElideMode QskTextOptions::elideMode() const noexcept
{
    return static_cast< Qt::TextElideMode >( m_elideMode );
}

constexpr inline Qt::TextElideMode QskTextOptions::effectiveElideMode() const noexcept
{
    return ( wrapMode() != QskTextOptions::NoWrap ) ?
        Qt::ElideNone : static_cast< Qt::TextElideMode >( m_elideMode );
}

inline void QskTextOptions::setWrapMode( WrapMode wrapMode ) noexcept
{
    m_wrapMode = wrapMode;
}

constexpr inline QskTextOptions::WrapMode QskTextOptions::wrapMode() const noexcept
{
    return static_cast< QskTextOptions::WrapMode >( m_wrapMode );
}

inline void QskTextOptions::setFontSizeMode( FontSizeMode fontSizeMode ) noexcept
{
    m_fontSizeMode = fontSizeMode;
}

constexpr inline QskTextOptions::FontSizeMode QskTextOptions::fontSizeMode() const noexcept
{
    return static_cast< QskTextOptions::FontSizeMode >( m_fontSizeMode );
}

inline void QskTextOptions::setMaximumLineCount( int lineCount ) noexcept
{
    m_maximumLineCount = lineCount;
}

constexpr inline int QskTextOptions::maximumLineCount() const noexcept
{
    return m_maximumLineCount;
}

inline constexpr bool QskTextOptions::operator==(
    const QskTextOptions& other ) const noexcept
{
    return ( m_format == other.m_format ) &&
           ( m_elideMode == other.m_elideMode ) &&
           ( m_wrapMode == other.m_wrapMode ) &&
           ( m_fontSizeMode == other.m_fontSizeMode ) &&
           ( m_maximumLineCount == other.m_maximumLineCount );
}

inline constexpr bool QskTextOptions::operator!=(
    const QskTextOptions& other ) const noexcept
{
    return !( *this == other );
}

#ifndef QT_NO_DEBUG_STREAM
class QDebug;

QSK_EXPORT QDebug operator<<( QDebug, const QskTextOptions& );
#endif

Q_DECLARE_TYPEINFO( QskTextOptions, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskTextOptions )

#endif
