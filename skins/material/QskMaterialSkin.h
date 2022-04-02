/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_MATERIAL_SKIN_H
#define QSK_MATERIAL_SKIN_H

#include "QskMaterialGlobal.h"
#include <QskSkin.h>
#include <QskRgbValue.h>

class QSK_MATERIAL_EXPORT QskMaterialPalette
{
  public:
    enum Lightness
    {
        Light,
        Dark
    };

    QskMaterialPalette( Lightness lightness )
        : m_lightness( lightness )
    {
    }

    inline QRgb elevated( const QRgb rgb, const float level = 1 ) const
    {
        return ( m_lightness == Light )
            ? QskRgb::darker( rgb, 100 + level * 15 )
            : QskRgb::lighter( rgb, 130 + level * 30 );
    }

    inline QRgb toDisabled( const QRgb rgb ) const
    {
        return QskRgb::toTransparentF( rgb, this->m_disabled );
    }

  public:
    QRgb primary;
    QRgb primaryVariant;
    QRgb onPrimary;

    QRgb secondary;
    QRgb secondaryVariant;
    QRgb onSecondary;

    QRgb background;
    QRgb onBackground;

    QRgb error;
    QRgb onError;

    QRgb primaryNoSaturation;
    QRgb secondaryNoSaturation;
    QRgb secondaryVariantNoSaturation;

    const qreal disabledOccupancy = 0.2;
    const qreal widgetBackgroundDisabled = 0.6;

    const qreal hover = 0.1;
    const qreal focused = 0.4;
    const qreal pressed = 0.5;

  private:
    const qreal m_disabled = 0.3;

    const Lightness m_lightness;
};

class QSK_MATERIAL_EXPORT QskMaterialSkin : public QskSkin
{
    Q_OBJECT

    using Inherited = QskSkin;

  public:
    QskMaterialSkin( const QskMaterialPalette&, QObject* parent = nullptr );
    ~QskMaterialSkin() override;
};

#endif
