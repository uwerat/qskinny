/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_PROGRESS_RING_SKINLET_H
#define QSK_PROGRESS_RING_SKINLET_H

#include "QskProgressIndicatorSkinlet.h"

class QskProgressRing;

class QSK_EXPORT QskProgressRingSkinlet : public QskProgressIndicatorSkinlet
{
    Q_GADGET

    using Inherited = QskProgressIndicatorSkinlet;

  public:
    Q_INVOKABLE QskProgressRingSkinlet( QskSkin* = nullptr );
    ~QskProgressRingSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateGrooveNode( const QskProgressIndicator*, QSGNode* ) const override;
    QSGNode* updateFillNode( const QskProgressIndicator*, QSGNode* ) const override;
};

#endif
