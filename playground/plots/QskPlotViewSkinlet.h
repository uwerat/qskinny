/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskSkinlet.h>
#include <QskPlotNamespace.h>

class QskPlotViewSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
        CanvasRole,
        AxisRole
    };

    Q_INVOKABLE QskPlotViewSkinlet( QskSkin* = nullptr );
    ~QskPlotViewSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

    int sampleCount( const QskSkinnable*,
        QskAspect::Subcontrol ) const override final;

    QRectF sampleRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, int index ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

    QSGNode* updateSampleNode( const QskSkinnable*,
        QskAspect::Subcontrol, int index, QSGNode* ) const override;

  private:
    QRectF axisRect( const QskSkinnable*, QskPlot::Axis ) const;

    QSGNode* updateAxisNode( const QskSkinnable*,
        QskPlot::Axis, QSGNode* ) const;
};
