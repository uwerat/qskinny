/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "ProgressBarPage.h"

#include <QskAnimator.h>
#include <QskGraphicProvider.h>
#include <QskGraphic.h>
#include <QskGradient.h>
#include <QskHctColor.h>
#include <QskProgressBar.h>
#include <QskProgressRing.h>
#include <QskRgbValue.h>

#include <QQuickWindow>

namespace
{
    class ProgressBar : public QskProgressBar
    {
      public:
        ProgressBar( QskLinearBox* box )
            : QskProgressBar( box )
        {
            setOrientation( ( box->orientation() == Qt::Horizontal )
                ? Qt::Vertical : Qt::Horizontal );

            setBoundaries( 0, 100 );
        }

        void setTheme( const QRgb base )
        {
            const QskHctColor hctColor( base );

            QVector< QRgb > colors;
            colors += hctColor.toned( 75 ).rgb();
            colors += hctColor.toned( 60 ).rgb();
            colors += hctColor.toned( 45 ).rgb();
            colors += hctColor.toned( 30 ).rgb();

            setFillGradient( qskBuildGradientStops( colors, true ) );
        }
    };

    class DeterminateIndicatorsAnimator : public QskAnimator
    {
      public:
        DeterminateIndicatorsAnimator( const QVector< QskProgressIndicator* >& indicators )
              : QskAnimator()
              , m_indicators( indicators )
        {
            setAutoRepeat( true );
            setDuration( 3000 );
        }

        void advance( qreal value ) override
        {
            for( auto* indicator : m_indicators )
            {
                indicator->setValueAsRatio( value );
            }
        }

      private:
        const QVector< QskProgressIndicator* > m_indicators;
    };
}

ProgressBarPage::ProgressBarPage( QQuickItem* parent )
    : Page( Qt::Horizontal, parent )
{
    setSpacing( 40 );
    populate();
}

void ProgressBarPage::populate()
{
    auto hBox = new QskLinearBox( Qt::Horizontal, this );
    hBox->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
    hBox->setSpacing( 20 );

    QVector< QskProgressIndicator* > determinateIndicators;

    {
        auto bar = new ProgressBar( hBox );
        bar->setTheme( QskRgb::LightSteelBlue );
        bar->setValue( 100 );
    }

    {
        auto bar = new ProgressBar( hBox );
        bar->setTheme( QskRgb::DodgerBlue );
        bar->setValue( 75 );
    }

    {
        auto bar = new ProgressBar( hBox );
        bar->setTheme( QskRgb::DodgerBlue );
        bar->setOrigin( 60 );
        bar->setValue( 25 );
    }

    {
        auto bar = new ProgressBar( hBox );
        determinateIndicators.append( bar );
    }

    {
        auto bar = new ProgressBar( hBox );
        bar->setIndeterminate( true );
    }

    auto vBox = new QskLinearBox( Qt::Vertical, this );
    vBox->setSpacing( 20 );
    vBox->setExtraSpacingAt( Qt::BottomEdge );

    {
        auto bar = new ProgressBar( vBox );
        bar->setTheme( QskRgb::OrangeRed );
        bar->setValue( 100 );
    }

    {
        auto bar = new ProgressBar( vBox );
        bar->setTheme( QskRgb::DeepPink );
        bar->setMaximum( 40 );
        bar->setValue( 25 );
    }

    {
        auto bar = new ProgressBar( vBox );
        bar->setTheme( QskRgb::DeepPink );
        bar->setOrigin( 40 );
        bar->setValue( 10 );
    }

    {
        auto bar = new ProgressBar( vBox );
        determinateIndicators.append( bar );
    }

    {
        auto bar = new ProgressBar( vBox );
        bar->setIndeterminate( true );
    }

    const auto sizes = { QskProgressRing::SmallSize, QskProgressRing::NormalSize,
                        QskProgressRing::LargeSize };

    auto determinateRingsHBox = new QskLinearBox( Qt::Horizontal, vBox );

    auto indeterminateRingsHBox = new QskLinearBox( Qt::Horizontal, vBox );

    for( const auto size : sizes )
    {
        for( const auto indeterminate : { true, false } )
        {
            auto* ring = new QskProgressRing( determinateRingsHBox );
            ring->setSize( size );
            ring->setLayoutAlignmentHint( Qt::AlignCenter );

            QQuickItem* parentItem;

            if( indeterminate )
            {
                parentItem = indeterminateRingsHBox;
                ring->setIndeterminate( true );
            }
            else
            {
                parentItem = determinateRingsHBox;
                determinateIndicators.append( ring );
            }

            ring->setParent( parentItem );
            ring->setParentItem( parentItem );
        }
    }

    connect( this, &QskItem::windowChanged, this, [this, determinateIndicators]( QQuickWindow* window )
    {
        if( window )
        {
            m_determinateIndicatorsAnimator.reset( new DeterminateIndicatorsAnimator( determinateIndicators ) );
            m_determinateIndicatorsAnimator->setWindow( window );
            m_determinateIndicatorsAnimator->start();
        }
    } );
}
