/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_STATUS_INDICATOR_SKINLET_H
#define QSK_STATUS_INDICATOR_SKINLET_H

#include "QskGlobal.h"
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
    virtual ~QskStatusIndicatorSkinlet();

    virtual QRectF subControlRect( const QskSkinnable*,
        QskAspect::Subcontrol ) const override;

protected:
    virtual QSGNode* updateSubNode(
        const QskSkinnable*, quint8 nodeRole, QSGNode* ) const override;

private:
    QRect graphicRect( const QskStatusIndicator* ) const;
    QSGNode* updateGraphicNode( const QskStatusIndicator*, QSGNode* ) const;
};

#endif

