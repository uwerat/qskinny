/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TAB_VIEW_SKINLET_H
#define QSK_TAB_VIEW_SKINLET_H

#include "QskGlobal.h"
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
    virtual ~QskTabViewSkinlet();

    virtual QRectF subControlRect(
        const QskSkinnable*, QskAspect::Subcontrol ) const override;

protected:
    virtual QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

private:
    QRectF pageRect( const QskTabView* ) const;
    QRectF tabBarRect( const QskTabView* ) const;

    QSGNode* updatePageNode( const QskTabView*, QSGNode* ) const;
};

#endif
