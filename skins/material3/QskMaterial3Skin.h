/******************************************************************************
 * QSkinny - Copyright (C) 2022 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef QSK_MATERIAL3_SKIN_H
#define QSK_MATERIAL3_SKIN_H

#include "QskMaterial3Global.h"

#include <QskHctColor.h>
#include <QskSkin.h>
#include <QskShadowMetrics.h>

#include <array>

class QSK_MATERIAL3_EXPORT QskMaterial3Theme
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

    QskMaterial3Theme( Lightness );
    QskMaterial3Theme( Lightness, std::array< QskHctColor, NumPaletteTypes > );

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
    QColor surface1;
    QColor surface2;
    QColor surface3;
    QColor surface4;
    QColor surface5;

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
    QskShadowMetrics elevationLight3;

    const qreal hoverOpacity = 0.08;
    const qreal focusOpacity = 0.12;
    const qreal pressedOpacity = 0.12;
    const qreal draggedOpacity = 0.16;

  private:
    std::array< QskHctColor, NumPaletteTypes > m_palettes;
};

class QSK_MATERIAL3_EXPORT QskMaterial3Skin : public QskSkin
{
    Q_OBJECT

    using Inherited = QskSkin;

  public:
    QskMaterial3Skin( const QskMaterial3Theme&, QObject* parent = nullptr );
    ~QskMaterial3Skin() override;

    enum FontRole
    {
        M3BodyMedium = QskSkin::HugeFont + 1,
        M3BodyLarge,
        M3HeadlineSmall,
        M3LabelLarge,
    };

  private:
    void setupFonts();
};

#endif
