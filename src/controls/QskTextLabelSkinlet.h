/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TEXT_LABEL_SKINLET_H
#define QSK_TEXT_LABEL_SKINLET_H

#include "QskGlobal.h"
#include "QskSkinlet.h"

class QskTextLabel;

class QSK_EXPORT QskTextLabelSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

public:
    enum NodeRole
    {
        TextRole
    };

    Q_INVOKABLE QskTextLabelSkinlet( QskSkin* = nullptr );
    virtual ~QskTextLabelSkinlet();

    virtual QRectF subControlRect( const QskSkinnable*,
        QskAspect::Subcontrol ) const override;

protected:
    virtual QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

private:
    QRectF textRect( const QskTextLabel* ) const;
    QSGNode* updateTextNode( const QskTextLabel*, QSGNode* ) const;
};

#endif
