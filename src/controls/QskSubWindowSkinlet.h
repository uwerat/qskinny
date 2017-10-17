/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SUB_WINDOW_SKINLET_H
#define QSK_SUB_WINDOW_SKINLET_H

#include "QskGlobal.h"
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
        TitleBarRole
    };

    Q_INVOKABLE QskSubWindowSkinlet( QskSkin* = nullptr );
    virtual ~QskSubWindowSkinlet();

    virtual QRectF subControlRect(
        const QskSkinnable*, QskAspect::Subcontrol ) const override;

protected:
    virtual QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

private:
    QSGNode* updateTitleBarNode( const QskSubWindow*, QSGNode* ) const;

    qreal titleBarHeight( const QskSubWindow* ) const;
    QRectF titleBarRect( const QskSubWindow* ) const;

};

#endif

