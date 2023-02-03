/******************************************************************************
 * QSkinny - Copyright (C) 2022 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef QSK_MATERIAL3_SKIN_H
#define QSK_MATERIAL3_SKIN_H

#include "QskMaterial3Global.h"

#include <QskBoxShapeMetrics.h>
#include <QskGraphicProvider.h>
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
    QRgb primary8; // ### rename to primaryHovered or so?
    QRgb primary12;
    QRgb onPrimary;
    QRgb primaryContainer;
    QRgb onPrimaryContainer;

    QRgb secondary;
    QRgb onSecondary;
    QRgb secondaryContainer;
    QRgb onSecondaryContainer;
    QRgb onSecondaryContainer8;
    QRgb onSecondaryContainer12;

    QRgb tertiary;
    QRgb onTertiary;
    QRgb tertiaryContainer;
    QRgb onTertiaryContainer;

    QRgb error;
    QRgb error8;
    QRgb error12;
    QRgb onError;
    QRgb errorContainer;
    QRgb onErrorContainer;

    QRgb background;
    QRgb onBackground;
    QRgb surface;
    QRgb surface1;
    QRgb surface2;
    QRgb surface3;
    QRgb surface4;
    QRgb surface5;

    QRgb onSurface;
    QRgb onSurface8;
    QRgb onSurface12;
    QRgb onSurface38;

    QRgb surfaceVariant;
    QRgb surfaceVariant12;
    QRgb onSurfaceVariant;
    QRgb outline;

    QRgb shadow;

    QskShadowMetrics elevation0;
    QskShadowMetrics elevation1;
    QskShadowMetrics elevation2;
    QskShadowMetrics elevation3;

    const qreal hoverOpacity = 0.08;
    const qreal focusOpacity = 0.12;
    const qreal pressedOpacity = 0.12;
    const qreal draggedOpacity = 0.16;

    QskBoxShapeMetrics shapeExtraSmallTop;

  private:
    std::array< QskHctColor, NumPaletteTypes > m_palettes;
};

class QSK_MATERIAL3_EXPORT QskMaterial3GraphicProvder : public QskGraphicProvider
{
    Q_OBJECT

    using Inherited = QskGraphicProvider;

  public:
    QskMaterial3GraphicProvder( QObject* parent = nullptr );

  protected:
    virtual const QskGraphic* loadGraphic( const QString& id ) const override;
};

class QSK_MATERIAL3_EXPORT QskMaterial3Skin : public QskSkin
{
    Q_OBJECT

    using Inherited = QskSkin;

  public:
    QskMaterial3Skin( const QskMaterial3Theme&, QObject* parent = nullptr );
    ~QskMaterial3Skin() override;

    virtual QskGraphic symbol( int symbolType ) const override;

    enum GraphicRole
    {
        GraphicRoleOnError,
        GraphicRoleOnPrimary,
        GraphicRoleOnSecondaryContainer,
        GraphicRoleOnSurface,
        GraphicRoleOnSurface38,
        GraphicRoleOnSurfaceVariant,
        GraphicRolePrimary,
        GraphicRoleSurface,
    };

    enum FontRole
    {
        M3BodyMedium = QskSkin::HugeFont + 1,
        M3BodyLarge,
        M3HeadlineSmall,
        M3LabelLarge,
    };

    static constexpr QskAspect::Variation Filled = QskAspect::NoVariation;
    static constexpr QskAspect::Variation Tonal = QskAspect::Huge;
    static constexpr QskAspect::Variation Elevated = QskAspect::Large;
    static constexpr QskAspect::Variation Outlined = QskAspect::Small;
    static constexpr QskAspect::Variation Text = QskAspect::Tiny;

  private:
    void setupFonts();
    void setupGraphicFilters( const QskMaterial3Theme& palette );
};

#endif
