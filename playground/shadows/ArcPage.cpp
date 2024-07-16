/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "ArcPage.h"
#include "ShadowedArc.h"
#include "Slider.h"

#include <QskGridBox.h>

namespace
{
    class ControlPanel : public QskGridBox
    {
        Q_OBJECT

      public:
        ControlPanel( ShadowedArc* arc, QQuickItem* parent = nullptr )
            : QskGridBox( parent )
        {
            setMargins( 5 );
            setSpacing( 10 );

            {
                auto slider = new Slider( "Start", 0, 360, 10, arc->startAngle() );
                connect( slider, &Slider::valueChanged, arc, &ShadowedArc::setStartAngle );

                addItem( slider, 0, 0 );
            }
            {
                auto slider = new Slider( "Span", -360, 360, 10, arc->spanAngle() );
                connect( slider, &Slider::valueChanged, arc, &ShadowedArc::setSpanAngle );

                addItem( slider, 0, 1 );
            }
            {
                auto slider = new Slider( "Extent", 0, 100, 1, arc->thickness() );
                connect( slider, &Slider::valueChanged, arc, &ShadowedArc::setThickness );

                addItem( slider, 1, 0 );
            }
            {
                auto slider = new Slider( "Border", 0, 50, 1, arc->borderWidth() );
                connect( slider, &Slider::valueChanged, arc, &ShadowedArc::setBorderWidth );

                addItem( slider, 1, 1);
            }
        }
    };

    class Arc : public ShadowedArc
    {
      public:
        Arc()
        {
            setStartAngle( 45.0 );
            setSpanAngle( 270.0 );
            setThickness( 10.0 );

            setBorderWidth( 2.0 );
            setBorderColor( QColor( 0, 0, 0, 150 ) );

#if 0
            const QskGradientStops stops =
            {
                { 0.1, Qt::darkRed  },
                { 0.1, Qt::darkYellow },
                { 0.5, Qt::darkYellow },
                { 0.5, Qt::darkBlue },
                { 0.75, Qt::darkBlue },
                { 0.75, Qt::darkRed }
            };
#endif
#if 0
            const QskGradientStops stops =
            {
                { 0.1, Qt::darkRed  },
                { 0.5, Qt::darkYellow },
                { 0.75, Qt::darkBlue },
                { 1.0, Qt::darkRed }
            };
#endif
#if 1
            QskGradientStops stops;
            for ( int i = 0; i < 100; i++ )
            {
                const auto pos = i * 1.0 / 100;

                stops += { pos, ( i % 2 ) ? Qt::darkRed : Qt::darkBlue };
                stops += { pos, ( i % 2 ) ? Qt::darkBlue : Qt::darkRed };
            }
#endif

            setFillGradient( stops );
        }
    };
}

ArcPage::ArcPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    auto arc = new Arc();
    arc->setMargins( 40 ); // some extra space for testing the offsets

    auto panel = new ControlPanel( arc );
    panel->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );

    addItem( panel );
    addItem( arc );
}

#include "ArcPage.moc"
