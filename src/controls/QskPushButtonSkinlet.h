/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_PUSH_BUTTON_SKINLET_H
#define QSK_PUSH_BUTTON_SKINLET_H

#include "QskSkinlet.h"

class QskPushButton;

class QSK_EXPORT QskPushButtonSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
        SplashRole,
        TextRole,
        IconRole,

        RoleCount
    };

    Q_INVOKABLE QskPushButtonSkinlet( QskSkin* = nullptr );
    ~QskPushButtonSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QRectF splashRect( const QskPushButton*, const QRectF& ) const;

    QSGNode* updateTextNode( const QskPushButton*, QSGNode* ) const;
    QSGNode* updateSplashNode( const QskPushButton*, QSGNode* ) const;
};

#endif
