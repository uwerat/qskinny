/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "SkinFactory.h"

#include <QskBoxBorderColors.h>
#include <QskGradient.h>
#include <QskGradientStop.h>
#include <QskPlatform.h>
#include <QskRgbValue.h>
#include <QskSkin.h>
#include <QskSkinHintTableEditor.h>
#include <QskTextLabel.h>

#include <LevelingSensor/QskLevelingSensor.h>
#include <LevelingSensor/QskLevelingSensorSkinlet.h>

#include <QskSlider.h>
#include <QskSliderSkinlet.h>

#include <QskTextLabel.h>
#include <QskTextLabelSkinlet.h>

namespace
{
    class Skin : public QskSkin
    {
        template< typename T >
        void style( QskSkinHintTableEditor& editor );

        template< typename Skinnable, typename Skinlet >
        void declareSkinlet( )
        {
            QskSkin::declareSkinlet< Skinnable, Skinlet >();
        }
        
        template< typename Skinnable, typename Skinlet >
        void declareSkinlet( QskSkinHintTableEditor& editor )
        {
            QskSkin::declareSkinlet< Skinnable, Skinlet >();
            style< Skinnable >( editor );
        }

        template<>
        void style< QskSlider >( QskSkinHintTableEditor& editor )
        {
            using A = QskAspect;
            using Q = QskSlider;

            const qreal extent = 40;

            // Panel

            for ( auto variation : { A::Horizontal, A::Vertical } )
            {
                const auto aspect = Q::Panel | variation;

                editor.setMetric( aspect | A::Size, extent );
                editor.setBoxBorderMetrics( aspect, 0 );
                editor.setBoxShape( aspect, 0 );
                editor.setGradient( aspect, QskGradient() );
            }

            // Groove, Fill

            for ( auto variation : { A::Horizontal, A::Vertical } )
            {
                for ( auto subControl : { Q::Groove, Q::Fill } )
                {
                    const auto aspect = subControl | variation;

                    editor.setMetric( aspect | A::Size, 0.3 * extent );

                    editor.setBoxBorderMetrics( aspect, 0 );
                    editor.setBoxShape( aspect, 0.1 * extent );
                }

                editor.setGradient( Q::Groove | variation, Qt::lightGray );
                editor.setGradient( Q::Fill | variation, Qt::darkGray );
            }

            // Handle

            for ( auto variation : { A::Horizontal, A::Vertical } )
            {
                const auto aspect = Q::Handle | variation;
                editor.setColor( aspect, Qt::black );

                editor.setBoxShape( aspect, 20.0, Qt::RelativeSize );

                const qreal sz = 0.75 * extent;
                editor.setStrutSize( aspect, sz, sz );
            }
        }

        template<>
        void style< QskLevelingSensor >( QskSkinHintTableEditor& editor )
        {
            using Q = QskLevelingSensor;

            static constexpr auto r1 = 0.9;
            static constexpr auto r2 = 1.0;

            QskGradient gradient{ {
                { 0.5, Qt::lightGray },
                { 0.5, Qt::lightGray },
                { 0.5, Qt::darkGray },
                { 1.0, Qt::darkGray },
            } };
            gradient.setLinearDirection( Qt::Vertical );

            editor.setColor( Q::Background, "dimgray" );

            editor.setStrutSize( Q::OuterDisk, { r2, r2 } );
            editor.setColor( Q::OuterDisk, Qt::white );

            editor.setGradient( Q::Horizon, gradient );
            editor.setStrutSize( Q::Horizon, { r1, r1 } );

            editor.setColor( Q::TickmarksX, Qt::black );
            editor.setStrutSize( Q::TickmarksX, { r1, 0.2 } );             // w %, h %
            editor.setHint( Q::TickmarksX, QVector3D{ 0.50, 0.75, 1.0 } ); // %
            editor.setAlignment(Q::TickmarksX, Qt::AlignCenter);

            editor.setStrutSize( Q::TickmarksXLabels, { r1, 0.15 } ); // w %, h %
            editor.setAlignment( Q::TickmarksXLabels, Qt::AlignTop | Qt::AlignHCenter );

            editor.setColor( Q::TickmarksY, Qt::black );
            editor.setStrutSize( Q::TickmarksY, { 0.1, r1 } );              // w %, h %
            editor.setHint( Q::TickmarksY, QVector3D{ 0.50, 0.75, 1.00 } ); // %
            editor.setAlignment(Q::TickmarksY, Qt::AlignCenter);

            editor.setStrutSize( Q::TickmarksYLabels, { 0.15, r1 } ); // w %, h %
            editor.setAlignment( Q::TickmarksYLabels, Qt::AlignCenter );

            editor.setColor( Q::TickmarksZ, "silver" );
            editor.setStrutSize( Q::TickmarksZ, { 0.90, 0.95 } );
            editor.setHint( Q::TickmarksZ, QVector3D{ 0.50, 0.75, 1.00 } ); // %

            editor.setStrutSize( Q::TickmarksZLabels, { 0.9, 0.0 } ); // r1 %, r2 %
            editor.setAlignment( Q::TickmarksZLabels, Qt::AlignCenter );
        }

      public:
        Skin()
        {
            QskSkinHintTableEditor editor( &hintTable() );
            declareSkinlet< QskSlider, QskSliderSkinlet >( editor );
            declareSkinlet< QskTextLabel, QskTextLabelSkinlet >();
            declareSkinlet< QskLevelingSensor, QskLevelingSensorSkinlet >( editor );
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
