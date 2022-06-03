/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_MATERIAL_SKIN_H
#define QSK_MATERIAL_SKIN_H

#include "QskMaterialGlobal.h"
#include <QskSkin.h>
#include <QskRgbPalette.h>

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

    QRgb surfaceVariant;
    QRgb onSurfaceVariant;
    QRgb outline;

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

    // ### add setTheme() re-implement resetColors()
};

#endif
