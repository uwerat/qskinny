/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskSkinlet.h>
#include <memory>

class CircularChartSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
        ArcRole,
        SegmentRole,
        SegmentLabelRole
    };

    Q_INVOKABLE CircularChartSkinlet( QskSkin* = nullptr );
    ~CircularChartSkinlet() override;

    void updateNode( QskSkinnable*, QSGNode* ) const override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

    int sampleCount( const QskSkinnable*, QskAspect::Subcontrol ) const override;

    QRectF sampleRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, int index ) const override;

    int sampleIndexAt( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, const QPointF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

    QSGNode* updateSampleNode( const QskSkinnable*,
        QskAspect::Subcontrol, int index, QSGNode* ) const override;

  private:
    QSGNode* updateArcSegmentNode( const QskSkinnable*, QSGNode*,
        qreal, const QColor&, const QskGradient&, const QskArcMetrics& ) const;

    QSGNode* updateArcLabelNode( const QskSkinnable*, QSGNode*,
        const QString&, const QskArcMetrics& ) const;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};
