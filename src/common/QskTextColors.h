/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_COLORS_H
#define QSK_TEXT_COLORS_H

#include "QskGlobal.h"

#include <qcolor.h>
#include <qmetatype.h>

class QDebug;
class QVariant;

class QSK_EXPORT QskTextColors
{
    Q_GADGET

    Q_PROPERTY( QColor textColor READ textColor WRITE setTextColor )
    Q_PROPERTY( QColor styleColor READ styleColor WRITE setStyleColor )
    Q_PROPERTY( QColor linkColor READ linkColor WRITE setLinkColor )

  public:
    QskTextColors( const QColor& text = QColor(),
        const QColor& style = QColor(), const QColor& link = QColor() );

    QColor textColor() const;
    void setTextColor( const QColor& );
    void setTextColor( QRgb );
    void setTextColor( Qt::GlobalColor );

    QColor styleColor() const;
    void setStyleColor( const QColor& );
    void setStyleColor( QRgb );
    void setStyleColor( Qt::GlobalColor );

    QColor linkColor() const;
    void setLinkColor( const QColor& );
    void setLinkColor( QRgb );
    void setLinkColor( Qt::GlobalColor );

    QskTextColors interpolated( const QskTextColors&, qreal value ) const;

    static QVariant interpolate( const QskTextColors&,
        const QskTextColors&, qreal ratio );

    QskHashValue hash( QskHashValue seed = 0 ) const noexcept;

  private:
    QColor m_textColor;
    QColor m_styleColor;
    QColor m_linkColor;
};

inline QskTextColors::QskTextColors(
        const QColor& text, const QColor& style, const QColor& link )
    : m_textColor( text )
    , m_styleColor( style )
    , m_linkColor( link )
{
}

inline QColor QskTextColors::textColor() const
{
    return m_textColor;
}

inline QColor QskTextColors::linkColor() const
{
    return m_linkColor;
}

inline QColor QskTextColors::styleColor() const
{
    return m_styleColor;
}

inline void QskTextColors::setTextColor( QRgb rgb )
{
    setTextColor( QColor::fromRgba( rgb ) );
}

inline void QskTextColors::setTextColor( Qt::GlobalColor color )
{
    setTextColor( QColor( color ) );
}

inline void QskTextColors::setStyleColor( QRgb rgb )
{
    setStyleColor( QColor::fromRgba( rgb ) );
}

inline void QskTextColors::setStyleColor( Qt::GlobalColor color )
{
    setStyleColor( QColor( color ) );
}

inline void QskTextColors::setLinkColor( QRgb rgb )
{
    setLinkColor( QColor::fromRgba( rgb ) );
}

inline void QskTextColors::setLinkColor( Qt::GlobalColor color )
{
    setLinkColor( QColor( color ) );
}

#ifndef QT_NO_DEBUG_STREAM

QSK_EXPORT QDebug operator<<( QDebug, const QskTextColors& );

#endif

Q_DECLARE_METATYPE( QskTextColors )

#endif
