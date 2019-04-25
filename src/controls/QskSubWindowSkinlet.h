/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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
    enum NodeRole
    {
        PanelRole = QskPopupSkinlet::OverlayRole + 1,
        TitleBarRole,
        SymbolRole,
        TitleRole
    };

    Q_INVOKABLE QskSubWindowSkinlet( QskSkin* = nullptr );
    ~QskSubWindowSkinlet() override;

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
