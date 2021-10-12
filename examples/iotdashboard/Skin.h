/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskGradient.h>
#include <QskSkin.h>

class Skin : public QskSkin
{
  public:
    class Palette
    {
      public:
        Palette( const QskGradient& menuBar, const QskGradient& mainContent,
                const QskGradient& box, const QColor& lightDisplay,
                const QskGradient& roundButton, const QColor& weekdayBox,
                const QColor& text, const QColor& shadow,
                const QskGradient& circularProgressBarGroove )
            : menuBar( menuBar )
            , mainContent( mainContent )
            , box( box )
            , lightDisplay( lightDisplay )
            , roundButton( roundButton )
            , weekdayBox( weekdayBox )
            , text( text )
            , shadow( shadow )
            , circularProgressBarGroove( circularProgressBarGroove )
        {
        }
        QskGradient menuBar;
        QskGradient mainContent;
        QskGradient box;
        QColor lightDisplay;
        QskGradient roundButton;
        QColor weekdayBox;
        QColor text;
        QColor shadow;
        QskGradient circularProgressBarGroove;
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
        : Skin(
            Skin::Palette( {"#6D7BFB"}, {"#fbfbfb"}, {"#ffffff"},
            "#ffffff", {"#f7f7f7"}, {"#f4f4f4"}, Qt::black, Qt::black,
            { QskGradient::Radial, { { 0.0, 0xc4c4c4 }, { 0.5, 0xf8f8f8 }, { 1.0, 0xc4c4c4 } } } )
            , parent )
    {
    }
};

class NighttimeSkin : public Skin
{
  public:
    NighttimeSkin( QObject* parent = nullptr )
        : Skin(
            Skin::Palette( {"#2937A7"}, {"#040404"}, {"#000000"},
            "#000000", {"#0a0a0a"}, {"#0c0c0c"}, Qt::white, Qt::white,
            { QskGradient::Radial, { { 0.0, 0x666666 }, { 0.5, 0x222222 }, { 1.0, 0x333333 } } } )
            , parent )
    {
    }
};
