/****************************************************************************
**
** Copyright 2021 Edelhirsch Software GmbH. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of the copyright holder nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

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
