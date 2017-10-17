/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SLIDER_SKINLET_H
#define QSK_SLIDER_SKINLET_H

#include "QskGlobal.h"
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
    virtual ~QskSliderSkinlet();

    virtual QRectF subControlRect( const QskSkinnable*,
        QskAspect::Subcontrol ) const override;

protected:
    virtual QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

private:
    QRectF panelRect( const QskSlider* ) const;
    QRectF grooveRect( const QskSlider* ) const;
    QRectF fillRect( const QskSlider* ) const;
    QRectF handleRect( const QskSlider* ) const;
    QRectF scaleRect( const QskSlider* ) const;

    QRectF innerRect( const QskSlider*, QskAspect::Subcontrol ) const;
};

#endif
