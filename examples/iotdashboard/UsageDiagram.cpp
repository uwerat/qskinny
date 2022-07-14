/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "UsageDiagram.h"
#include "Diagram.h"

#include <QskGridBox.h>
#include <QskSkin.h>
#include <QskTextLabel.h>

QSK_SUBCONTROL( UsageDiagramLegend, Panel )
QSK_SUBCONTROL( UsageDiagramLegend, Symbol )

QSK_STATE( UsageDiagramLegend, Water, ( QskAspect::FirstUserState << 1 ) )
QSK_STATE( UsageDiagramLegend, Electricity, ( QskAspect::FirstUserState << 2 ) )
QSK_STATE( UsageDiagramLegend, Gas, ( QskAspect::FirstUserState << 3 ) )

QSK_SUBCONTROL( UsageDiagramBox, Panel )
QSK_SUBCONTROL( UsageDiagramBox, DaysBox )
QSK_SUBCONTROL( UsageDiagramBox, DayText )

namespace
{
    class LegendItem : public QskLinearBox
    {
      public:
        LegendItem( QskAspect::State state, QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, parent )
        {
            setSpacing( 10 );

            auto symbol = new QskBox( true );
            symbol->setSubcontrolProxy( QskBox::Panel, UsageDiagramLegend::Symbol );
            symbol->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
            symbol->setSkinStateFlag( state );

            QString text;

            if( state == UsageDiagramLegend::Water )
            {
                text = "Water";
            }
            else if( state == UsageDiagramLegend::Electricity )
            {
                text = "Electricity";
            }
            else if( state == UsageDiagramLegend::Gas )
            {
                text = "Gas";
            }

            auto label = new QskTextLabel( text );
            label->setFontRole( QskSkin::TinyFont );

            addItem( symbol );
            addItem( label );
        }

    };
}

UsageDiagramLegend::UsageDiagramLegend( QQuickItem* parent )
    : QskLinearBox( parent )
{
    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    setMargins( 10, 10, 20, 10 );
    setSpacing( 30 );

    addItem( new LegendItem( UsageDiagramLegend::Water ) );
    addItem( new LegendItem( UsageDiagramLegend::Electricity ) );
    addItem( new LegendItem( UsageDiagramLegend::Gas ) );
}

UsageDiagram::UsageDiagram( QQuickItem* parent )
    : Diagram( parent )
{
    // These values are calculated with a boost::math::cubic_b_spline.
    // We just output the values here to get rid of the dependency:

    const qreal water[] =
    {
        64.00, 64.00, 64.00, 64.00, 64.00, 64.00, 64.00, 64.00, 64.00, 64.00,
        64.00, 63.99, 63.96, 63.85, 63.64, 63.30, 62.79, 62.09, 61.14, 59.93,
        58.42, 56.59, 54.50, 52.23, 49.84, 47.42, 45.03, 42.76, 40.68, 38.85,
        37.36, 36.27, 35.55, 35.19, 35.14, 35.39, 35.91, 36.65, 37.60, 38.73,
        40.00, 41.39, 42.87, 44.41, 46.00, 47.60, 49.19, 50.76, 52.26, 53.68,
        55.00, 56.19, 57.24, 58.15, 58.90, 59.51, 59.95, 60.23, 60.33, 60.26,
        60.00, 59.56, 58.94, 58.17, 57.27, 56.24, 55.12, 53.92, 52.65, 51.34,
        50.00, 48.65, 47.32, 46.03, 44.79, 43.65, 42.61, 41.70, 40.95, 40.37,
        40.00, 39.85, 39.94, 40.26, 40.84, 41.67, 42.77, 44.15, 45.80, 47.75,
        50.00, 52.54, 55.30, 58.19, 61.13, 64.04, 66.82, 69.40, 71.67, 73.57
    };

    const qreal electricity[] =
    {
        36.00, 36.01, 36.11, 36.37, 36.88, 37.73, 38.98, 40.73, 43.07, 46.06,
        49.80, 54.31, 59.38, 64.73, 70.09, 75.20, 79.77, 83.55, 86.24, 87.59,
        87.33, 85.26, 81.61, 76.64, 70.67, 63.98, 56.86, 49.61, 42.52, 35.89,
        30.00, 25.09, 21.14, 18.08, 15.83, 14.31, 13.45, 13.16, 13.37, 14.01,
        15.00, 16.26, 17.73, 19.36, 21.07, 22.82, 24.55, 26.19, 27.68, 28.97,
        30.00, 30.73, 31.25, 31.65, 32.04, 32.52, 33.21, 34.19, 35.58, 37.48,
        40.00, 43.17, 46.80, 50.61, 54.33, 57.71, 60.47, 62.35, 63.07, 62.38,
        60.00, 55.79, 50.12, 43.46, 36.31, 29.13, 22.43, 16.68, 12.37, 9.98,
        10.00, 12.73, 17.76, 24.50, 32.36, 40.75, 49.09, 56.77, 63.21, 67.81,
        70.00, 69.37, 66.28, 61.29, 54.96, 47.85, 40.51, 33.50, 27.37, 22.68
    };

    const qreal gas[] =
    {
        43.50, 43.50, 43.50, 43.50, 43.50, 43.50, 43.50, 43.50, 43.50, 43.50,
        43.50, 43.50, 43.50, 43.50, 43.50, 43.50, 43.50, 43.50, 43.50, 43.50,
        43.50, 43.50, 43.50, 43.50, 43.50, 43.50, 43.50, 43.50, 43.50, 43.50,
        43.50, 43.51, 43.59, 43.80, 44.20, 44.87, 45.86, 47.25, 49.10, 51.47,
        54.43, 58.01, 62.05, 66.38, 70.80, 75.12, 79.16, 82.73, 85.64, 87.70,
        88.73, 88.60, 87.42, 85.34, 82.55, 79.20, 75.47, 71.51, 67.51, 63.61,
        60.00, 56.80, 54.04, 51.68, 49.71, 48.11, 46.86, 45.95, 45.35, 45.04,
        45.00, 45.22, 45.71, 46.46, 47.50, 48.82, 50.43, 52.35, 54.58, 57.13,
        60.00, 63.17, 66.45, 69.62, 72.46, 74.74, 76.24, 76.74, 76.01, 73.84,
        70.00, 64.39, 57.37, 49.45, 41.12, 32.86, 25.18, 18.56, 13.49, 10.48
    };

    addCurve( water, sizeof( water ) / sizeof( qreal ) );
    addCurve( electricity, sizeof( electricity ) / sizeof( qreal ) );
    addCurve( gas, sizeof( gas ) / sizeof( qreal ) );

    setYMax( 100 );
}

void UsageDiagram::addCurve( const qreal values[], const size_t count )
{
    QVector< QPointF > points;
    points.reserve( count );

    for( size_t i = 0; i < count; i++ )
        points += QPointF( i, values[i] );

    addDataPoints( points, Diagram::Area );
}

UsageDiagramBox::UsageDiagramBox(const QString &title, Diagram *diagram, QQuickItem* parent )
    : Box( title, parent )
{
    diagram->setParent( this );
    diagram->setParentItem( this );

    setSubcontrolProxy( QskBox::Panel, Panel );

    auto gridBox = new QskGridBox();
    gridBox->setSpacing( 0 );

    gridBox->addItem( new UsageDiagramLegend(), 0, 0, 1, -1, Qt::AlignTop | Qt::AlignRight );
    gridBox->addItem( diagram, 0, 0, 1, -1 );

    const char* days[] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };

    for( size_t i = 0; i < sizeof( days ) / sizeof( days[0] ); i++ )
    {
        auto box = new QskBox( true );
        box->setSubcontrolProxy( QskBox::Panel, UsageDiagramBox::DaysBox );

        auto label = new QskTextLabel( days[i] );
        label->setSubcontrolProxy( QskTextLabel::Text, UsageDiagramBox::DayText );
        label->setAlignment( Qt::AlignCenter );
        label->setMargins( 0, 10, 0, 10 );

        gridBox->addItem( box, 0, i );
        gridBox->addItem( label, 1, i );
    }

    addItem( gridBox );
}

#include "moc_UsageDiagram.cpp"
