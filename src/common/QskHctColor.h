/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_HCT_COLOR_H
#define QSK_HCT_COLOR_H

#include "QskGlobal.h"
#include <qcolor.h>

/*
    For M(aterial)3 the new HTC color system has been created, that
    is based on H(ue), (C)hroma, (T)one:

        https://material.io/blog/science-of-color-design

    This system allows to create color palettes by varying the tone
    for given hue/chroma values.

    https://material-foundation.github.io/material-theme-builder/#/custom
    shows how to create a tonal palette from a given RGB color.
 */

class QSK_EXPORT QskHctColor
{
  public:
    constexpr QskHctColor() noexcept = default;
    constexpr QskHctColor( qreal hue, qreal chrome, qreal tone = 40 ) noexcept;

    QskHctColor( QRgb );

    void setHue( qreal hue );
    constexpr qreal hue() const noexcept;

    void setChroma( qreal chroma ) noexcept;
    constexpr qreal chroma() const noexcept;

    void setTone( qreal tone ) noexcept;
    constexpr qreal tone() const noexcept;

    constexpr QskHctColor toned( qreal tone ) const noexcept;

    void setRgb( QRgb );
    QRgb rgb() const;

  private:
    qreal m_hue = 0;    // [0.0, 360.0[
    qreal m_chroma = 0;
    qreal m_tone = 0;   // [0.0, 100.0]
};

Q_DECLARE_TYPEINFO( QskHctColor, Q_MOVABLE_TYPE );

inline constexpr QskHctColor::QskHctColor( qreal hue, qreal chroma, qreal tone ) noexcept
    : m_hue( hue )
    , m_chroma( chroma )
    , m_tone( tone )
{
}

inline constexpr qreal QskHctColor::hue() const noexcept
{
    return m_hue;
}

inline constexpr qreal QskHctColor::chroma() const noexcept
{
    return m_chroma;
}

inline constexpr qreal QskHctColor::tone() const noexcept
{
    return m_tone;
}

inline constexpr QskHctColor QskHctColor::toned( qreal tone ) const noexcept
{
    return QskHctColor( m_hue, m_chroma, tone );
}

#ifndef QT_NO_DEBUG_STREAM

    class QDebug;
    QSK_EXPORT QDebug operator<<( QDebug, const QskHctColor& );

#endif

#endif
