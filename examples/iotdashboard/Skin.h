/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef SKIN_H
#define SKIN_H

#include <QskGradient.h>
#include <QskSkin.h>

class Skin : public QskSkin
{
    public:
        class Palette
        {
            public:
                Palette( const QskGradient& menuBar, const QskGradient& mainContent, const QskGradient& box,
                         const QColor& lightDisplay, const QColor& pieChart, const QskGradient& roundButton,
                         const QColor& weekdayBox, const QColor& text, const QColor& shadow )
                    : menuBar( menuBar )
                    , mainContent( mainContent )
                    , box( box )
                    , lightDisplay( lightDisplay )
                    , pieChart( pieChart )
                    , roundButton( roundButton )
                    , weekdayBox( weekdayBox )
                    , text( text )
                    , shadow( shadow )
                {
                }
                QskGradient menuBar;
                QskGradient mainContent;
                QskGradient box;
                QColor lightDisplay;
                QColor pieChart;
                QskGradient roundButton;
                QColor weekdayBox;
                QColor text;
                QColor shadow;
        };

        Skin( const Palette& palette, QObject* parent = nullptr );
        virtual ~Skin();

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
        "#ffffff", "#ffffff", {"#f7f7f7"},
        {"#f4f4f4"}, Qt::black, Qt::black )
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
        "#000000", "#000000", {"#0a0a0a"},
        {"#0c0c0c"}, Qt::white, Qt::white )
        , parent )
        {
        }
};

#endif // SKIN_H
