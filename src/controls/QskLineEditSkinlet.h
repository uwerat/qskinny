/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LINE_EDIT_SKINLET_H
#define QSK_LINE_EDIT_SKINLET_H

#include "QskGlobal.h"
#include "QskSkinlet.h"

class QskLineEdit;

class QSK_EXPORT QskLineEditSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

public:
    enum NodeRole
    {
        BackgroundRole,
        ForegroundRole
    };

    Q_INVOKABLE QskLineEditSkinlet( QskSkin* = nullptr );
    virtual ~QskLineEditSkinlet();

    virtual QRectF subControlRect( const QskSkinnable*,
        QskAspect::Subcontrol ) const override;

protected:
    virtual QSGNode* updateSubNode( const QskSkinnable*,
        quint8, QSGNode* ) const override;

private:
    QRectF panelRect( const QskLineEdit* ) const;

    QSGNode* updateBackgroundNode( const QskLineEdit*, QSGNode* ) const;
    QSGNode* updateForegroundNode( const QskLineEdit*, QSGNode* ) const;
};

#endif
