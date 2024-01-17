/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "PlotSkin.h"

#include <QskSkin.h>
#include <QskSkinHintTableEditor.h>

#include <QskPlotView.h>
#include <QskPlotViewSkinlet.h>

#include <QskPlotCurve.h>
#include <QskPlotCurveSkinlet.h>

#include <QskPlotGrid.h>
#include <QskPlotGridSkinlet.h>

#include <QskPlotCorridor.h>
#include <QskPlotCorridorSkinlet.h>

#include "PlotCursor.h"
#include "PlotCursorSkinlet.h"

#include <QskRgbValue.h>
#include <QskBoxBorderColors.h>
#include <QskBoxShapeMetrics.h>
#include <QskGraduationMetrics.h>
#include <QskStippleMetrics.h>

#include <QskGraduationRenderer.h>

#include <qbrush.h>

namespace
{
    inline bool isExtended( const QskSkin* skin )
    {
        auto metaObject = skin->skinletMetaObject( &QskPlotView::staticMetaObject );
        return metaObject != &QskSkinlet::staticMetaObject; // the fallback for controls
    }

    class SkinEditor : private QskSkinHintTableEditor
    {
      public:
        SkinEditor( QskSkinHintTable* table );
        void setupPlotHints();
    };
}

void PlotSkin::extendSkin( QskSkin* skin )
{
    if ( skin == nullptr || isExtended( skin ) )
        return;

    skin->declareSkinlet< QskPlotView, QskPlotViewSkinlet >();
    skin->declareSkinlet< QskPlotGrid, QskPlotGridSkinlet >();
    skin->declareSkinlet< QskPlotCurve, QskPlotCurveSkinlet >();
    skin->declareSkinlet< QskPlotCorridor, QskPlotCorridorSkinlet >();
    skin->declareSkinlet< PlotCursor, PlotCursorSkinlet >();

    SkinEditor editor( &skin->hintTable() );
    editor.setupPlotHints();
}

SkinEditor::SkinEditor( QskSkinHintTable* table )
    : QskSkinHintTableEditor( table )
{
}

void SkinEditor::setupPlotHints()
{
    using A = QskAspect;
    using namespace QskRgb;

    const auto rgbLower = DodgerBlue;
    const auto rgbUpper = MediumSeaGreen;
    const auto rgbValue = Yellow;
    {
        using Q = QskPlotView;

        // Panel
        setBoxShape( Q::Panel, 10 );
        setGradient( Q::Panel, qRgb( 240, 240, 240 ) );
        setBoxBorderMetrics( Q::Panel, 1 );
        setBoxBorderColors( Q::Panel, qRgb( 220, 220, 220 ) );

        // Canvas
        setGradient( Q::Canvas, QGradient::PremiumDark );
        setBoxBorderColors( Q::Canvas, DimGray );
        setBoxBorderMetrics( Q::Canvas, 2 );
        setBoxShape( Q::Canvas, 4 );

        // AxisScale
        const auto padding = 4; // spacing between canvas and axis
        setPadding( Q::AxisScale | A::Left, 0, 0, padding, 0 );
        setPadding( Q::AxisScale | A::Bottom, 0, padding, 0, 0 );

        setColor( Q::AxisScale, qRgb( 20, 20, 20 ) );
        setFontRole( Q::AxisScale, QskSkin::MediumFont );
        setFlag( Q::AxisScale | A::Style, QskGraduationRenderer::Backbone );

        // thickness/length of the major ticks
        setGraduationMetrics( Q::AxisScale, { 4.0, 6.0, 8.0, 1.0 } );

        // spacing between ticks and labels
        setSpacing( Q::AxisScale, 5 );
    }

    {
        using Q = QskPlotGrid;

        setColor( Q::MajorLine, White );
        setMetric( Q::MajorLine | A::Size, 1 );
        setStippleMetrics( Q::MajorLine, { 2, 6 } );

        setColor( Q::MinorLine, Gainsboro );
        setMetric( Q::MinorLine | A::Size, 1 );
        setStippleMetrics( Q::MinorLine, { 1, 10 } );
    }

    {
        using Q = QskPlotCurve;

        setMetric( Q::Line | A::Size, 2 );
        setColor( Q::Line, rgbValue );
    }

    {
        using Q = QskPlotCorridor;

        setMetric( Q::Border | A::Size, 2 );
        setColor( Q::Border | A::Lower, rgbLower );
        setColor( Q::Border | A::Upper, rgbUpper );
        setColor( Q::Corridor, toTransparent( Crimson, 150 ) );
    }

    {
        using Q = PlotCursor;

        const int alpha = 200;

        setColor( Q::Line, Qt::yellow );
        setMetric( Q::Line | A::Size, 1 );
        setStippleMetrics( Q::Line, { 4, 8 } );

        setGradient( Q::LabelPanel | A::Lower, toTransparent( rgbLower, alpha ) );
        setColor( Q::LabelText | A::Lower, Qt::white );

        setGradient( Q::LabelPanel | A::Upper, toTransparent( rgbUpper, alpha ) );
        setColor( Q::LabelText | A::Upper, Qt::white );

        setGradient( Q::LabelPanel, toTransparent( rgbValue, alpha ) );
        setColor( Q::LabelText, Qt::black );

        setBoxShape( Q::LabelPanel, 5 );
        setPadding( Q::LabelPanel, 5, 5, 5, 5 );
    }
}

