/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TEXT_OPTIONS_H
#define QSK_TEXT_OPTIONS_H

#include "QskGlobal.h"

#include <qmetatype.h>
#include <qtextoption.h>

class QDebug;

class QSK_EXPORT QskTextOptions
{
    Q_GADGET

    Q_PROPERTY( TextFormat format READ format WRITE setFormat )
    Q_PROPERTY( Qt::TextElideMode elideMode READ elideMode WRITE setElideMode )
    Q_PROPERTY( WrapMode wrapMode READ wrapMode WRITE setWrapMode )
    Q_PROPERTY( FontSizeMode fontSizeMode READ fontSizeMode WRITE setFontSizeMode )
    Q_PROPERTY( int maximumLineCount READ maximumLineCount WRITE setMaximumLineCount )

  public:
    enum FontSizeMode
    {
        FixedSize,
        HorizontalFit,
        VerticalFit,
        Fit
    };

    enum WrapMode
    {
        NoWrap = QTextOption::NoWrap,
        WordWrap = QTextOption::WordWrap,
        WrapAnywhere = QTextOption::WrapAnywhere,
        Wrap = QTextOption::WrapAtWordBoundaryOrAnywhere
    };
    Q_ENUM( WrapMode )

    QskTextOptions();

    enum TextFormat
    {
        PlainText = Qt::PlainText,
        RichText = Qt::RichText,
        AutoText = Qt::AutoText,
        StyledText = 4 // taken from QQuickText::StyledText
    };
    Q_ENUM( TextFormat )

    TextFormat format() const;
    void setFormat( TextFormat );

    TextFormat effectiveFormat( const QString& text ) const;

    Qt::TextElideMode elideMode() const;
    void setElideMode( Qt::TextElideMode );

    Qt::TextElideMode effectiveElideMode() const;

    FontSizeMode fontSizeMode() const;
    void setFontSizeMode( FontSizeMode );

    WrapMode wrapMode() const;
    void setWrapMode( WrapMode );

    int maximumLineCount() const;
    void setMaximumLineCount( int );

    bool operator==( const QskTextOptions& other ) const;
    bool operator!=( const QskTextOptions& other ) const;

    int textFlags() const;

  private:
    int m_maximumLineCount;
    FontSizeMode m_fontSizeMode : 2;
    WrapMode m_wrapMode : 4;
    TextFormat m_format : 3;
    Qt::TextElideMode m_elideMode : 2;
};

inline bool QskTextOptions::operator!=( const QskTextOptions& other ) const
{
    return !( *this == other );
}

QSK_EXPORT uint qHash( const QskTextOptions&, uint seed = 0 ) noexcept;

#ifndef QT_NO_DEBUG_STREAM
QSK_EXPORT QDebug operator<<( QDebug, const QskTextOptions& );
#endif

Q_DECLARE_TYPEINFO( QskTextOptions, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskTextOptions )

#endif
