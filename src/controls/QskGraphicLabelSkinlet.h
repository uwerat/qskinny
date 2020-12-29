/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRAPHIC_LABEL_SKINLET_H
#define QSK_GRAPHIC_LABEL_SKINLET_H

#include "QskSkinlet.h"

class QskGraphicLabel;

class QSK_EXPORT QskGraphicLabelSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        GraphicRole
    };

    Q_INVOKABLE QskGraphicLabelSkinlet( QskSkin* = nullptr );
    ~QskGraphicLabelSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QRect graphicRect( const QskGraphicLabel*, const QRectF& ) const;
    QSGNode* updateGraphicNode( const QskGraphicLabel*, QSGNode* ) const;
};

#endif
