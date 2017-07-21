/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TAB_BUTTON_SKINLET_H
#define QSK_TAB_BUTTON_SKINLET_H

#include "QskGlobal.h"
#include "QskSkinlet.h"

class QskTabButton;

class QSK_EXPORT QskTabButtonSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

public:
    enum NodeRole
    {
        ButtonRole,
        TextRole
    };

    Q_INVOKABLE QskTabButtonSkinlet( QskSkin* = nullptr );
    virtual ~QskTabButtonSkinlet();

    virtual QRectF subControlRect(
        const QskSkinnable*, QskAspect::Subcontrol ) const override;

protected:
    virtual QSGNode* updateSubNode(
        const QskSkinnable*, quint8 nodeRole, QSGNode* ) const override;

private:
    QRectF textRect( const QskTabButton* ) const;
    QRectF panelRect( const QskTabButton* ) const;

    QSGNode* updateButtonNode( const QskTabButton*, QSGNode* ) const;
    QSGNode* updateTextNode( const QskTabButton*, QSGNode* ) const;
};

#endif
