/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include "QskPlotView.h"
#include <memory>

class Plot : public QskPlotView
{
    Q_OBJECT

    using Inherited = QskPlotView;

  public:
    class Sample
    {
      public:
        qreal timestamp = 0.0;

        qreal lowerBound = 0.0;
        qreal value = 0.0;
        qreal upperBound = 0.0;
    };

    Plot( QQuickItem* parentItem = nullptr );
    ~Plot() override;

    void setSamples( const QVector< Sample >& );

  public Q_SLOT:
    void resetAxes();
    void shiftXAxis( int steps );

  protected:
    void mousePressEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;

    void wheelEvent( QWheelEvent* ) override;
    void changeEvent( QEvent* ) override;

  private:
    QVariant labelAt( QskPlot::Axis axis, qreal pos ) const final override;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};
