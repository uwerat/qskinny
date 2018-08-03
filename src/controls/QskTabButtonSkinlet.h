/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TAB_BUTTON_SKINLET_H
#define QSK_TAB_BUTTON_SKINLET_H

#include "QskGlobal.h"
#include "QskSkinlet.h"

class QSK_EXPORT QskTabButtonSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
        TextRole
    };

    Q_INVOKABLE QskTabButtonSkinlet( QskSkin* = nullptr );
    ~QskTabButtonSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;
};

#endif
