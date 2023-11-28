/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "QskControl.h"
#include "QskPlotNamespace.h"

class QskPlotItem;

class QskTickmarks;
class QskIntervalF;
class QTransform;

class QskPlotView : public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Panel, AxisScale, Canvas )

    QskPlotView( QQuickItem* parent = nullptr );
    ~QskPlotView() override;

    void setBoundaries( QskPlot::Axis, qreal, qreal );
    void setBoundaries( QskPlot::Axis, const QskIntervalF& );
    QskIntervalF boundaries( QskPlot::Axis axis ) const;

    QskTickmarks tickmarks( QskPlot::Axis axis ) const;

    // scales -> item coordinates
    QTransform transformation( QskPlot::Axis xAxis, QskPlot::Axis yAxis ) const;
    QRectF canvasRect() const;

    virtual QVariant labelAt( QskPlot::Axis, qreal pos ) const;

  protected:
    void geometryChange( const QRectF&, const QRectF& ) override;
    void updateNode( QSGNode* ) override;
    void updateResources() override;

    void changeEvent( QEvent* ) override;

  private:
    friend class QskPlotItem;

    void attachItem( QskPlotItem* );
    void detachItem( QskPlotItem* );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};
