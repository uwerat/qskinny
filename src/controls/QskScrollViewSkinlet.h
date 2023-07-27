/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SCROLL_VIEW_SKINLET_H
#define QSK_SCROLL_VIEW_SKINLET_H

#include "QskSkinlet.h"

class QskScrollView;

class QSK_EXPORT QskScrollViewSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
        ViewportRole,
        ContentsRootRole,

        HorizontalScrollBarRole,
        HorizontalScrollHandleRole,

        VerticalScrollBarRole,
        VerticalScrollHandleRole,

        RoleCount
    };

    Q_INVOKABLE QskScrollViewSkinlet( QskSkin* = nullptr );
    ~QskScrollViewSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

    virtual QSGNode* updateContentsNode( const QskScrollView*, QSGNode* ) const;

  private:
    QSGNode* updateContentsRootNode( const QskScrollView*, QSGNode* ) const;
    QSGNode* updateScrollBarNode(
        const QskScrollView*, QskAspect::Subcontrol, QSGNode* ) const;

    QRectF viewportRect( const QskScrollView*, const QRectF& ) const;
    QRectF scrollBarRect( const QskScrollView*, const QRectF&, Qt::Orientation ) const;
    QRectF scrollHandleRect( const QskScrollView*, const QRectF&, Qt::Orientation ) const;
};

#endif
