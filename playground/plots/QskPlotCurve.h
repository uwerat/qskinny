/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include "QskPlotItem.h"

#include <qpoint.h>
#include <qvector.h>

class QskPlotCurveData;
class QColor;

class QskPlotCurve : public QskPlotItem
{
    Q_OBJECT

    Q_PROPERTY( qreal lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged )
    Q_PROPERTY( QColor color READ color WRITE setColor NOTIFY colorChanged )

    using Inherited = QskPlotItem;

  public:
    QSK_SUBCONTROLS( Line )

    QskPlotCurve( QObject* = nullptr );
    ~QskPlotCurve() override;

    void setPoints( const QVector< QPointF >& );

    void setData( QskPlotCurveData* );
    QskPlotCurveData* data() const;

    QPointF interpolatedPoint( Qt::Orientation, qreal ) const;

    void setColor( const QColor& );
    QColor color() const;

    void setLineWidth( qreal );
    qreal lineWidth() const;

    void transformationChanged( ChangeFlags ) override;
    bool needsClipping() const override;

    using QskSkinnable::setColor;
    using QskSkinnable::color;

  Q_SIGNALS:
    void lineWidthChanged( qreal );
    void colorChanged( const QColor& );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};
