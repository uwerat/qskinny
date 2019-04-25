/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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
        ViewportRole,
        ContentsRootRole,

        HorizontalScrollBarRole,
        HorizontalScrollHandleRole,

        VerticalScrollBarRole,
        VerticalScrollHandleRole
    };

    Q_INVOKABLE QskScrollViewSkinlet( QskSkin* = nullptr );
    ~QskScrollViewSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

    virtual QSGNode* updateContentsNode( const QskScrollView*, QSGNode* ) const;
    QSGNode* contentsNode( const QskScrollView* );

  private:
    QSGNode* updateContentsRootNode( const QskScrollView*, QSGNode* ) const;

    QRectF viewportRect( const QskScrollView*, const QRectF& ) const;
    QRectF scrollBarRect( const QskScrollView*, const QRectF&, Qt::Orientation ) const;
    QRectF scrollHandleRect( const QskScrollView*, const QRectF&, Qt::Orientation ) const;
};

#endif
