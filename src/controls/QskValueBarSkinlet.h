/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_VALUE_BAR_SKINLET_H
#define QSK_VALUE_BAR_SKINLET_H

#include "QskSkinlet.h"

class QskValueBar;

class QSK_EXPORT QskValueBarSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        GrooveRole,
        ValueFillRole,

        RoleCount,
    };

    Q_INVOKABLE QskValueBarSkinlet( QskSkin* = nullptr );
    ~QskValueBarSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSGNode* updateFillNode( const QskValueBar*, QSGNode* ) const;
    QRectF fillRect( const QskValueBar* ) const;
};

#endif
