/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_MATERIAL_SKIN_H
#define QSK_MATERIAL_SKIN_H

#include "QskMaterialGlobal.h"
#include <QskSkin.h>
#include <QskRgbValue.h>
#include <memory>

struct ColorPalette
{
    enum Lightness { light, dark } lightness;

    QColor primary;
    QColor primaryVariant;
    QColor onPrimary;

    QColor secondary;
    QColor secondaryVariant;
    QColor onSecondary;

    QColor background;
    QColor onBackground;

    QColor error;
    QColor onError;

    QColor primaryNoSaturation = QColor::fromHsl(primary.hslHue(), 0,
        primary.lightness() );

    QColor secondaryNoSaturation =
        QColor::fromHsl(secondary.hslHue(), 0,
        secondary.lightness() );

    QColor secondaryVariantNoSaturation =
        QColor::fromHsl(secondaryVariant.hslHue(), 0,
        secondaryVariant.lightness() +
        secondaryVariant.hslSaturation() );

    qreal disabledOccupancy = 0.2;
    qreal widgetBackgroundDisabled = 0.6;

    qreal hover = 0.1;
    qreal focused = 0.4;
    qreal pressed = 0.5;
    qreal disabled = 0.3;

    ColorPalette(
        Lightness lightness = light,
        QColor primary = QColor::fromRgb(0x6200EE),
        QColor primaryVariant = QColor::fromRgb(0x3700B3),
        QColor onPrimary = Qt::white,
        QColor secondary = QColor::fromRgb(0x03DAC6),
        QColor secondaryVariant = QColor::fromRgb(0x018786),
        QColor onSecondary = Qt::white,
        QColor background = QColor::fromRgba( QskRgb::Grey100 ),
        QColor onBackground = Qt::black,
        QColor error = QColor::fromRgb(0xB00020),
        QColor onError = Qt::white):
        lightness(lightness),
        primary(primary),
        primaryVariant(primaryVariant),
        onPrimary(onPrimary),
        secondary(secondary),
        secondaryVariant(secondaryVariant),
        onSecondary(onSecondary),
        background(background),
        onBackground(onBackground),
        error(error),
        onError(onError)
    {
        primaryNoSaturation = QColor::fromHsl(primary.hslHue(), 0,
            primary.lightness() );

        secondaryNoSaturation = QColor::fromHsl(secondary.hslHue(),
            0,
            secondary.lightness() );

        secondaryVariantNoSaturation =
            QColor::fromHsl( secondaryVariant.hslHue(), 0,
            secondaryVariant.lightness() );
    }

    inline QColor elevated( const QColor target, const float level = 1 ) const {
        return ( lightness == light ) ? target.darker(100 + level * 15)
      : target.lighter(130 + level * 30);
    }
};

class QSK_MATERIAL_EXPORT QskMaterialSkin : public QskSkin
{
    Q_OBJECT

    using Inherited = QskSkin;

  public:
    QskMaterialSkin( ColorPalette, QObject* parent = nullptr );
    ~QskMaterialSkin() override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
