/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SUB_WINDOW_AREA_SKINLET_H
#define QSK_SUB_WINDOW_AREA_SKINLET_H

#include "QskSkinlet.h"

class QskSubWindowArea;

class QSK_EXPORT QskSubWindowAreaSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole
    };

    Q_INVOKABLE QskSubWindowAreaSkinlet( QskSkin* = nullptr );
    ~QskSubWindowAreaSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSGNode* updatePanelNode( const QskSubWindowArea*, QSGNode* ) const;
};

#endif
