/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_PROGRESS_BAR_SKINLET_H
#define QSK_PROGRESS_BAR_SKINLET_H

#include "QskProgressIndicatorSkinlet.h"

class QskProgressBar;

class QSK_EXPORT QskProgressBarSkinlet : public QskProgressIndicatorSkinlet
{
    Q_GADGET

    using Inherited = QskProgressIndicatorSkinlet;

  public:
    enum NodeRole
    {
        GrooveStopIndicatorRole = Inherited::RoleCount,
    };

    Q_INVOKABLE QskProgressBarSkinlet( QskSkin* = nullptr );
    ~QskProgressBarSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

    int sampleCount( const QskSkinnable*, QskAspect::Subcontrol ) const override;

    QRectF sampleRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, int index ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*, quint8 nodeRole, QSGNode* ) const override;

    QSGNode* updateGrooveNode( const QskProgressIndicator*, QSGNode* ) const override;
    QSGNode* updateFillNode( const QskProgressIndicator*, QSGNode* ) const override;

    QSGNode* updateSampleNode( const QskSkinnable*,
        QskAspect::Subcontrol, int index, QSGNode* ) const override;

  private:
    QRectF barRect( const QskProgressBar* ) const;
    QRectF grooveStopIndicatorRect( const QskProgressBar* ) const;
};

#endif
