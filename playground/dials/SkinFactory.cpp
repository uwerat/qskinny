/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SkinFactory.h"

#include "Dial.h"
#include "DialSkinlet.h"

#include <QskBoxBorderColors.h>
#include <QskTextLabel.h>
#include <QskSkinHintTableEditor.h>
#include <QskSkin.h>
#include <QskPlatform.h>
#include <QskRgbValue.h>

namespace
{
    inline QFont qskFont( qreal pointSize )
    {
        QFont font( "Roboto" );
        font.setPointSizeF( pointSize / qskDpiScaled( 1.0 ) );
        return font;
    }

    class Skin : public QskSkin
    {
      public:
        Skin()
        {
            using namespace QskRgb;

            declareSkinlet< Dial, DialSkinlet >();

            setFont( QskSkin::DefaultFont, qskFont( 13 ) );
            setFont( QskSkin::LargeFont, qskFont( 20 ) );

            const auto rgb1 = qRgb( 1, 16, 27 ); // Maastricht blue
            const auto rgb2 = qRgb( 255, 0, 22 ); // Ruddy
            const auto rgb3 = qRgb( 41, 234, 212 ); // Turquoise
            const auto rgb4 = qRgb( 253, 255, 252 ); // baby powder

            QskSkinHintTableEditor ed( &hintTable() );

            ed.setColor( QskTextLabel::Text, rgb4 );

            {
                using Q = Dial;

                ed.setBoxBorderMetrics( Q::Panel, 2 );
                ed.setBoxShape( Q::Panel, 100, Qt::RelativeSize );
                ed.setGradient( Q::Panel, rgb1 );
                ed.setBoxBorderColors( Q::Panel, rgb3 );

                ed.setBoxBorderMetrics( Q::Knob, 2 );
                ed.setStrutSize( Q::Knob, 30, 30 );
                ed.setBoxShape( Q::Knob, 100, Qt::RelativeSize );

                QskGradient gradient( rgb2, rgb1 );
                gradient.setLinearDirection( 0.0, 0.0, 1.0, 1.0 );
                ed.setGradient( Q::Knob, gradient );

                ed.setMetric( Q::Needle | QskAspect::Size, 2 );
                ed.setMetric( Q::Needle | QskAspect::Margin, 10 );
                ed.setColor( Q::Needle, rgb2 );

                ed.setSpacing( Q::TickLabels, 4 );
                ed.setStrutSize( Q::TickLabels, 2, 15 );
                ed.setColor( Q::TickLabels, rgb4 );
                ed.setFontRole( Q::TickLabels, QskSkin::SmallFont );
            }
        }
    };

}

QStringList SkinFactory::skinNames() const
{
    return { "Skin" };
}

QskSkin* SkinFactory::createSkin( const QString& skinName )
{
    if ( skinName == "Skin" )
        return new Skin();

    return nullptr;
}

#include "moc_SkinFactory.cpp"
