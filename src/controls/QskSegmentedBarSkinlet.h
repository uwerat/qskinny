/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SEGMENTED_BAR_SKINLET_H
#define QSK_SEGMENTED_BAR_SKINLET_H

#include "QskSkinlet.h"

class QskSegmentedBar;

class QSK_EXPORT QskSegmentedBarSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole : quint8
    {
        PanelRole,
        SegmentRole,
        SeparatorRole,
        CursorRole,
        SplashRole,

        TextRole,
        IconRole,

        RoleCount
    };

    Q_INVOKABLE QskSegmentedBarSkinlet( QskSkin* = nullptr );
    ~QskSegmentedBarSkinlet() override;

    QRectF subControlRect( const QskSkinnable*, const QRectF&,
        QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

    int sampleCount( const QskSkinnable*, QskAspect::Subcontrol ) const override;

    QRectF sampleRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, int index ) const override;

    QskAspect::States sampleStates( const QskSkinnable*,
        QskAspect::Subcontrol, int index ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

    QSGNode* updateSampleNode( const QskSkinnable*,
        QskAspect::Subcontrol, int index, QSGNode* ) const override;

  private:
    QSizeF segmentSizeHint(const QskSegmentedBar*, Qt::SizeHint ) const;

    QRectF segmentRect( const QskSegmentedBar*, const QRectF&, int index ) const;
    QRectF separatorRect( const QskSegmentedBar*, const QRectF&, int index ) const;
    QRectF cursorRect( const QskSegmentedBar*, const QRectF& ) const;
    QRectF splashRect( const QskSegmentedBar*, const QRectF& ) const;

    QSGNode* updateSplashNode( const QskSegmentedBar*, QSGNode* ) const;
};

#endif
