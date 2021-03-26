#ifndef DSKIN_H
#define DSKIN_H

#include <QskGradient.h>
#include <QskSkin.h>

class Skin : public QskSkin
{
    public:
        class Palette
        {
            public:
                Palette( QskGradient menuBar, QskGradient mainContent, QskGradient box,
                         QColor lightDisplay, QColor pieChart, QskGradient roundButton,
                         QColor weekdayBox, QColor text )
                    : menuBar( menuBar )
                    , mainContent( mainContent )
                    , box( box )
                    , lightDisplay( lightDisplay )
                    , pieChart( pieChart )
                    , roundButton( roundButton )
                    , weekdayBox( weekdayBox )
                    , text( text )
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
        {"#f4f4f4"}, "#000000" )
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
        {"#0c0c0c"}, "#ffffff" )
        , parent )
        {
        }
};

#endif // SKIN_H
