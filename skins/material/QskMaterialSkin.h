/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_MATERIAL_SKIN_H
#define QSK_MATERIAL_SKIN_H

#include "QskMaterialGlobal.h"
#include <QskSkin.h>
#include <QskRgbPalette.h>
#include <QskShadowMetrics.h>

#include <array>

class QSK_MATERIAL_EXPORT QskMaterialTheme
{
  public:
    enum Lightness
    {
        Light,
        Dark
    };

    enum PaletteType
    {
        Primary,
        Secondary,
        Tertiary,
        Error,
        Neutral,
        NeutralVariant,

        NumPaletteTypes
    };

    QskMaterialTheme( Lightness );
    QskMaterialTheme( Lightness, std::array< QskRgbPalette, NumPaletteTypes > );

    QRgb primary;
    QColor primary12;
    QRgb onPrimary;
    QRgb primaryContainer;
    QRgb onPrimaryContainer;

    QRgb secondary;
    QRgb onSecondary;
    QRgb secondaryContainer;
    QRgb onSecondaryContainer;

    QRgb tertiary;
    QRgb onTertiary;
    QRgb tertiaryContainer;
    QRgb onTertiaryContainer;

    QRgb error;
    QRgb onError;
    QRgb errorContainer;
    QRgb onErrorContainer;

    QRgb background;
    QRgb onBackground;
    QRgb surface;
    QRgb onSurface;
    QColor onSurface12;
    QColor onSurface38;

    QRgb surfaceVariant;
    QColor surfaceVariant12;
    QRgb onSurfaceVariant;
    QRgb outline;

    QRgb shadow;

    QskShadowMetrics elevationLight1;
    QskShadowMetrics elevationLight2;

    const qreal hoverOpacity = 0.08;
    const qreal focusOpacity = 0.12;
    const qreal pressedOpacity = 0.12;
    const qreal draggedOpacity = 0.16;

  private:
    std::array< QskRgbPalette, NumPaletteTypes > m_palettes;
};

class QSK_MATERIAL_EXPORT QskMaterialSkin : public QskSkin
{
    Q_OBJECT

    using Inherited = QskSkin;

  public:
    QskMaterialSkin( const QskMaterialTheme&, QObject* parent = nullptr );
    ~QskMaterialSkin() override;

    enum FontRole
    {
        M3BodyLarge = QskSkin::HugeFont + 1,
        M3LabelLarge,
    };

    // ### add setTheme() re-implement resetColors()

  private:
    void setupFonts();
};

#endif
