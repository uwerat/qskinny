/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FLUENT2_SKIN_H
#define QSK_FLUENT2_SKIN_H

#include "QskFluent2Global.h"
#include <QskSkin.h>

class QskFluent2Theme;

class QSK_FLUENT2_EXPORT QskFluent2Skin : public QskSkin
{
    Q_OBJECT

    using Inherited = QskSkin;

  public:
    QskFluent2Skin( QObject* parent = nullptr );
    ~QskFluent2Skin() override;

    void addTheme( QskAspect::Section, const QskFluent2Theme& );
    void setup();

    enum GraphicRole
    {
        GraphicRoleFillColorTextDisabled,
        GraphicRoleFillColorTextOnAccentDisabled,
        GraphicRoleFillColorTextOnAccentPrimary,
        GraphicRoleFillColorTextOnAccentSecondary,
        GraphicRoleFillColorTextPrimary,
        GraphicRoleFillColorTextSecondary,
    };

    enum FontRole
    {
        Caption = TinyFont,
        Body = DefaultFont,
        BodyStrong = SmallFont,
        BodyLarge = MediumFont,
        Subtitle = LargeFont,
        Title = HugeFont,
        TitleLarge,
        Display,
    };

    static constexpr QskAspect::Variation Standard = QskAspect::NoVariation;
    static constexpr QskAspect::Variation Accent = QskAspect::Large;

  private:
    void setupFonts();
    void setupGraphicFilters( const QskFluent2Theme& );
    void setGraphicColor( GraphicRole, QRgb );
};

#endif
