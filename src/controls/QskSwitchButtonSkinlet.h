/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SWITCH_BUTTON_SKINLET_H
#define QSK_SWITCH_BUTTON_SKINLET_H

#include "QskSkinlet.h"

class QSK_EXPORT QskSwitchButtonSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        GrooveRole,
        HandleRole,

        RoleCount
    };

    Q_INVOKABLE QskSwitchButtonSkinlet( QskSkin* = nullptr );
    ~QskSwitchButtonSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QRectF grooveRect( const QskSkinnable*, const QRectF& ) const;
    QRectF handleRect( const QskSkinnable*, const QRectF& ) const;
};

#endif
