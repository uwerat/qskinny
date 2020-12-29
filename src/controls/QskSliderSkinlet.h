/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SLIDER_SKINLET_H
#define QSK_SLIDER_SKINLET_H

#include "QskSkinlet.h"

class QskSlider;

class QSK_EXPORT QskSliderSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
        GrooveRole,
        FillRole,
        HandleRole
    };

    Q_INVOKABLE QskSliderSkinlet( QskSkin* = nullptr );
    ~QskSliderSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QRectF panelRect( const QskSlider*, const QRectF& ) const;
    QRectF grooveRect( const QskSlider*, const QRectF& ) const;
    QRectF fillRect( const QskSlider*, const QRectF& ) const;
    QRectF handleRect( const QskSlider*, const QRectF& ) const;
    QRectF scaleRect( const QskSlider*, const QRectF& ) const;

    QRectF innerRect( const QskSlider*, const QRectF&, QskAspect::Subcontrol ) const;
};

#endif
