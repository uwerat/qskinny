/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_STATUS_INDICATOR_SKINLET_H
#define QSK_STATUS_INDICATOR_SKINLET_H

#include "QskSkinlet.h"

class QskStatusIndicator;

class QSK_EXPORT QskStatusIndicatorSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        GraphicRole
    };

    Q_INVOKABLE QskStatusIndicatorSkinlet( QskSkin* = nullptr );
    ~QskStatusIndicatorSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QRect graphicRect( const QskStatusIndicator*, const QRectF& ) const;
    QSGNode* updateGraphicNode( const QskStatusIndicator*, QSGNode* ) const;
};

#endif
