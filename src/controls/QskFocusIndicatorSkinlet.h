/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FOCUS_INDICATOR_SKINLET_H
#define QSK_FOCUS_INDICATOR_SKINLET_H

#include "QskSkinlet.h"

class QskFocusIndicator;

class QSK_EXPORT QskFocusIndicatorSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        FrameRole,
        RoleCount
    };

    Q_INVOKABLE QskFocusIndicatorSkinlet( QskSkin* = nullptr );
    ~QskFocusIndicatorSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSGNode* updateFrameNode( const QskFocusIndicator*, QSGNode* ) const;
};

#endif
