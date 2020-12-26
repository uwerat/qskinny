/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SkinFactory.h"

#include "ButtonBar.h"
#include "Speedometer.h"
#include "SpeedometerSkinlet.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskColorFilter.h>
#include <QskFunctions.h>
#include <QskMargins.h>
#include <QskTextLabel.h>
#include <QskAnimationHint.h>
#include <QskSetup.h>
#include <QskSkinTransition.h>
#include <QskSkinHintTableEditor.h>
#include <QskSkin.h>

#include <QEasingCurve>

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
            declareSkinlet< Speedometer, SpeedometerSkinlet >();

            setFont( QskSkin::DefaultFont, qskFont( 13 ) );
            setFont( QskSkin::LargeFont, qskFont( 20 ) );

            setSkinHint( ButtonBar::Indicator | QskAspect::GraphicRole, SkinFactory::Indicator );
        }
    };

    class Skin1 : public Skin
    {
      public:
        Skin1() 
        {
            const QColor color1( "#363636" ); // Jet
            const QColor color2( "#242F40" ); // Yankees blue
            const QColor color3( "#CCA43B" ); // Satin sheet gold
            const QColor color4( "#E5E5E5" ); // Platinum
            const QColor color5( "#FFFFFF" ); // white

            QskSkinHintTableEditor ed( &hintTable() );

            ed.setColor( QskTextLabel::Text, color3 );

            {
                using Q = Speedometer;

                ed.setBoxBorderMetrics( Q::Panel, 5 );
                ed.setBoxShape( Q::Panel, 30, Qt::RelativeSize );
                ed.setGradient( Q::Panel,
                    QskGradient( QskGradient::Vertical, color2, color4 ) );
                ed.setBoxBorderColors( Q::Panel, color3 );

                ed.setBoxBorderMetrics( Q::Knob, 5 );
                ed.setStrutSize( Q::Knob, 20, 20 );
                ed.setBoxShape( Q::Knob, 100, Qt::RelativeSize );
                ed.setGradient( Q::Knob, color2 );
                ed.setBoxBorderColors( Q::Knob, color4 );

                ed.setMetric( Q::Needle | QskAspect::Size, 4 );
                ed.setMetric( Q::Needle | QskAspect::Margin, 15 );
                ed.setColor( Q::Needle, color4 );

                ed.setSpacing( Q::TickLabels, 3 );
                ed.setStrutSize( Q::TickLabels, 3, 25 );
                ed.setColor( Q::TickLabels, color4 );
                ed.setFontRole( Q::TickLabels, QskSkin::SmallFont );
            }

            {
                // all SVGs on the header/footer are plain white

                QskColorFilter filter;
                filter.addColorSubstitution( Qt::white, color3.rgb() );

                setGraphicFilter( SkinFactory::Indicator, filter );
            }
        }
    };

    class Skin2 : public Skin
    {
      public:
        Skin2() 
        {
            const QColor color1( "#011627" ); // Maastricht blue
            const QColor color2( "#FF0022" ); // ruddy
            const QColor color3( "#41EAD4" ); // Turquoise
            const QColor color4( "#FDFFFC" ); // baby powder
            const QColor color5( "#B91372" ); // red violet

            QskSkinHintTableEditor ed( &hintTable() );

            ed.setColor( QskTextLabel::Text, color4 );

            {
                using Q = Speedometer;

                ed.setBoxBorderMetrics( Q::Panel, 2 );
                ed.setBoxShape( Q::Panel, 100, Qt::RelativeSize );
                ed.setGradient( Q::Panel, color1 );
                ed.setBoxBorderColors( Q::Panel, color3 );

                ed.setBoxBorderMetrics( Q::Knob, 2 );
                ed.setStrutSize( Q::Knob, 30, 30 );
                ed.setBoxShape( Q::Knob, 100, Qt::RelativeSize );
                ed.setGradient( Q::Knob,
                    QskGradient( QskGradient::Diagonal, color2, color1 ) );

                ed.setMetric( Q::Needle | QskAspect::Size, 2 );
                ed.setMetric( Q::Needle | QskAspect::Margin, 10 );
                ed.setColor( Q::Needle, color2 );

                ed.setSpacing( Q::TickLabels, 4 );
                ed.setStrutSize( Q::TickLabels, 2, 15 );
                ed.setColor( Q::TickLabels, color4 );
                ed.setFontRole( Q::TickLabels, QskSkin::SmallFont );
            }
        }
    };

}

QStringList SkinFactory::skinNames() const
{
    return { "Skin1", "Skin2" };
}

QskSkin* SkinFactory::createSkin( const QString& skinName )
{
    if ( skinName == "Skin1" )
        return new Skin1();

    if ( skinName == "Skin2" )
        return new Skin2();

    return nullptr;
}

void SkinFactory::rotateSkin()
{
    const auto names = skinNames();

    int index = names.indexOf( qskSetup->skinName() );
    index = ( index + 1 ) % names.size();

    auto oldSkin = qskSetup->skin();
    if ( oldSkin->parent() == qskSetup )
        oldSkin->setParent( nullptr ); // otherwise setSkin deletes it

    auto newSkin = qskSetup->setSkin( names[ index ] );

    QskSkinTransition transition;

    transition.setSourceSkin( oldSkin );
    transition.setTargetSkin( newSkin );
    transition.setAnimation( 600 );

    transition.process();

    if ( oldSkin->parent() == nullptr )
        delete oldSkin;
}

#include "moc_SkinFactory.cpp"
