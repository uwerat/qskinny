/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FUSION_PALETTE_H
#define QSK_FUSION_PALETTE_H

#include "QskSkin.h"
#include "QskFusionGlobal.h"
#include "QskRgbValue.h"

#include <qpalette.h>

class QskFusionPalette
{
  public:
    QskFusionPalette( QskSkin::ColorScheme = QskSkin::LightScheme );
    QskFusionPalette( const QPalette& );

    ~QskFusionPalette();

    QRgb active( QPalette::ColorRole ) const;
    QRgb disabled( QPalette::ColorRole ) const;

    QRgb color( QPalette::ColorGroup, QPalette::ColorRole ) const;

    QRgb lighter( QPalette::ColorGroup, QPalette::ColorRole, int factor ) const;
    QRgb darker( QPalette::ColorGroup, QPalette::ColorRole, int factor ) const;

    QRgb outline;
    QRgb button;
    QRgb groove;
    QRgb tabFrame;
    QRgb highlightedOutline;
    QRgb error;

  private:
    void initExtraColors();

    void setColor( QPalette::ColorRole, QRgb );
    void setColor( QPalette::ColorGroup, QPalette::ColorRole, QRgb );

    QRgb m_colors[ 2 ][ QPalette::NColorRoles ] = { };
};

inline QRgb QskFusionPalette::color(
    QPalette::ColorGroup group, QPalette::ColorRole role ) const
{
    Q_ASSERT( group >= 0 && group < 2 );
    return m_colors[group][role];
}

inline QRgb QskFusionPalette::lighter( QPalette::ColorGroup group,
    QPalette::ColorRole role, int factor ) const
{
    return QskRgb::lighter( color( group, role ), factor );
}

inline QRgb QskFusionPalette::darker( QPalette::ColorGroup group,
    QPalette::ColorRole role, int factor ) const
{
    return QskRgb::darker( color( group, role ), factor );
}

inline QRgb QskFusionPalette::active( QPalette::ColorRole role ) const
{
    return m_colors[ QPalette::Active ][ role ];
}

inline QRgb QskFusionPalette::disabled( QPalette::ColorRole role ) const
{
    return m_colors[ QPalette::Disabled ][ role ];
}

inline void QskFusionPalette::setColor(
    QPalette::ColorRole role, QRgb rgb )
{
    setColor( QPalette::Active, role, rgb );
    setColor( QPalette::Disabled, role, rgb );
}

inline void QskFusionPalette::setColor(
    QPalette::ColorGroup group, QPalette::ColorRole role, QRgb rgb )
{
    Q_ASSERT( group >= 0 && group < 2 );
    m_colors[group][role] = rgb;
}

#endif
