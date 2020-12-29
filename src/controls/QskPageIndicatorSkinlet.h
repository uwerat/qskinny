/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PAGE_INDICATOR_SKINLET_H
#define QSK_PAGE_INDICATOR_SKINLET_H

#include "QskSkinlet.h"

class QskPageIndicator;

class QSK_EXPORT QskPageIndicatorSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
        BulletsRole
    };

    Q_INVOKABLE QskPageIndicatorSkinlet( QskSkin* = nullptr );
    ~QskPageIndicatorSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSGNode* updateBulletsNode( const QskPageIndicator*, QSGNode* ) const;
    QRectF bulletRect( const QskPageIndicator*, const QRectF&, int index ) const;
};

#endif
