/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskGradient.h>
#include <QskSkin.h>

class Skin : public QskSkin
{
  public:
    struct Palette
    {
        QColor menuBar;
        QColor mainContent;
        QColor box;
        QColor roundButton;
        QColor roundButtonPressed;
        QColor weekdayBox;
        QColor text;
        QColor shadow;
        QColor lightDisplayKnobBorder;
        QskGradient lightDisplayColdAndWarmArc;
        QRgb circularProgressBarGroove;
        QRgb deviceGraphic;
    };

    Skin( const Palette& palette, QObject* parent = nullptr );
    ~Skin() override;

    enum SkinFontRole
    {
        TitleFont = QskSkin::HugeFont + 1,
    };

  private:
    void initHints( const Palette& palette );
};

class DaytimeSkin : public Skin
{
  public:
    DaytimeSkin( QObject* parent = nullptr )
        : Skin( palette(), parent )
    {
    }

  private:
    Palette palette() const;
};

class NighttimeSkin : public Skin
{
  public:
    NighttimeSkin( QObject* parent = nullptr )
        : Skin( palette(), parent )
    {
    }

  private:
    Palette palette() const;
};
