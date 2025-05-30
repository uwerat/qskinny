/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SUB_WINDOW_SKINLET_H
#define QSK_SUB_WINDOW_SKINLET_H

#include "QskPopupSkinlet.h"

class QskSubWindow;

class QSK_EXPORT QskSubWindowSkinlet : public QskPopupSkinlet
{
    Q_GADGET

    using Inherited = QskPopupSkinlet;

  public:
    enum NodeRole : quint8
    {
        PanelRole = QskPopupSkinlet::RoleCount,
        TitleBarRole,
        SymbolRole,
        TitleRole,

        RoleCount
    };

    Q_INVOKABLE QskSubWindowSkinlet( QskSkin* = nullptr );
    ~QskSubWindowSkinlet() override;

    virtual QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    qreal titleBarHeight( const QskSubWindow* ) const;

    QRectF titleBarRect( const QskSubWindow*, const QRectF& ) const;
    QRectF symbolRect( const QskSubWindow*, const QRectF& ) const;
    QRectF titleRect( const QskSubWindow*, const QRectF& ) const;
};

#endif
