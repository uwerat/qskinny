/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TAB_VIEW_SKINLET_H
#define QSK_TAB_VIEW_SKINLET_H

#include "QskSkinlet.h"

class QskTabView;

class QSK_EXPORT QskTabViewSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PageRole
    };

    Q_INVOKABLE QskTabViewSkinlet( QskSkin* = nullptr );
    ~QskTabViewSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QRectF pageRect( const QskTabView*, const QRectF& ) const;
    QRectF tabBarRect( const QskTabView*, const QRectF& ) const;
};

#endif
