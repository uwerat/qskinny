/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_POPUP_SKINLET_H
#define QSK_POPUP_SKINLET_H

#include "QskSkinlet.h"

class QskPopup;

class QSK_EXPORT QskPopupSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        OverlayRole
    };

    Q_INVOKABLE QskPopupSkinlet( QskSkin* = nullptr );
    ~QskPopupSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSGNode* updateOverlayNode( const QskPopup*, QSGNode* ) const;
};

#endif
