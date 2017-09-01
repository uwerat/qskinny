/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PUSH_BUTTON_SKINLET_H
#define QSK_PUSH_BUTTON_SKINLET_H

#include "QskGlobal.h"
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
        TextRole,
        GraphicRole
    };

    Q_INVOKABLE QskPushButtonSkinlet( QskSkin* = nullptr );
    virtual ~QskPushButtonSkinlet();

    virtual QRectF subControlRect(
        const QskSkinnable*, QskAspect::Subcontrol ) const override;

protected:
    virtual QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

private:
    QRectF textRect( const QskPushButton* ) const;
    QRectF graphicRect( const QskPushButton* ) const;

    QSGNode* updateTextNode( const QskPushButton*, QSGNode* ) const;
};

#endif

