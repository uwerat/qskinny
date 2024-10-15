/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_MATERIAL3_SKIN_H
#define QSK_MATERIAL3_SKIN_H

#include "QskMaterial3Global.h"

#include <QskSkin.h>
#include <QskBoxShapeMetrics.h>
#include <QskShadowMetrics.h>

#include <qcolor.h>

class QSK_MATERIAL3_EXPORT QskMaterial3Theme
{
  public:
    class BaseColors
    {
      public:
        BaseColors() = default;

        QRgb primary        = 0xff6750A4;
        QRgb secondary      = 0xff625B71;
        QRgb tertiary       = 0xff7D5260;
        QRgb error          = 0xffB3261E;
        QRgb neutral        = 0xff605D62;
        QRgb neutralVariant = 0xff605D66;
    };

    QskMaterial3Theme( QskSkin::ColorScheme );
    QskMaterial3Theme( QskSkin::ColorScheme, const BaseColors& );

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
    QRgb outlineVariant;

    QRgb surfaceContainerHighest;

    QRgb shadow;

    QskShadowMetrics elevation0;
    QskShadowMetrics elevation1;
    QskShadowMetrics elevation2;
    QskShadowMetrics elevation3;

    const qreal hoverOpacity = 0.08;
    const qreal focusOpacity = 0.12;
    const qreal pressedOpacity = 0.12;
    const qreal draggedOpacity = 0.16;

    QskBoxShapeMetrics shapeExtraSmall;
    QskBoxShapeMetrics shapeExtraSmallTop;
};

class QSK_MATERIAL3_EXPORT QskMaterial3Skin : public QskSkin
{
    Q_OBJECT

    using Inherited = QskSkin;

  public:
    enum GraphicRole
    {
        GraphicRoleError,
        GraphicRoleOnError,
        GraphicRoleOnErrorContainer,
        GraphicRoleOnPrimary,
        GraphicRoleOnSecondaryContainer,
        GraphicRoleOnSurface,
        GraphicRoleOnSurface38,
        GraphicRoleOnSurfaceVariant,
        GraphicRolePrimary,
        GraphicRoleSurface,
    };

    QskMaterial3Skin( QObject* parent = nullptr );
    ~QskMaterial3Skin() override;

    static constexpr QskAspect::Variation Filled = QskAspect::NoVariation;
    static constexpr QskAspect::Variation Tonal = QskAspect::Huge;
    static constexpr QskAspect::Variation Elevated = QskAspect::Large;
    static constexpr QskAspect::Variation Outlined = QskAspect::Small;
    static constexpr QskAspect::Variation Text = QskAspect::Tiny;

  protected:
    void initHints() override;

  private:
    void setupSkinlets();
    void setupFonts();
    void setupGraphicFilters( const QskMaterial3Theme& );
    void setGraphicColor( GraphicRole, QRgb );
};

#endif
