/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_PROGRESS_BAR_SKINLET_H
#define QSK_PROGRESS_BAR_SKINLET_H

#include "QskSkinlet.h"

class QskProgressBar;

class QSK_EXPORT QskProgressBarSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        GrooveRole,
        BarRole,

        RoleCount
    };

    Q_INVOKABLE QskProgressBarSkinlet( QskSkin* = nullptr );
    ~QskProgressBarSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSGNode* updateBarNode( const QskProgressBar*, QSGNode* ) const;
    QRectF barRect( const QskProgressBar* ) const;
};

#endif
