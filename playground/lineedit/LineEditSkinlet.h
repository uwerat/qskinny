/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef _LINE_EDIT_SKINLET_H
#define _LINE_EDIT_SKINLET_H

#include "QskGlobal.h"
#include "QskSkinlet.h"

class LineEdit;

class LineEditSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

public:
    enum NodeRole
    {
        BackgroundRole,
        ForegroundRole
    };

    Q_INVOKABLE LineEditSkinlet( QskSkin* = nullptr );
    virtual ~LineEditSkinlet();

    virtual QRectF subControlRect( const QskSkinnable*,
        QskAspect::Subcontrol ) const override;

protected:
    virtual QSGNode* updateSubNode( const QskSkinnable*,
        quint8, QSGNode* ) const override;

private:
    QRectF panelRect( const LineEdit* ) const;

    QSGNode* updateBackgroundNode( const LineEdit*, QSGNode* ) const;
    QSGNode* updateForegroundNode( const LineEdit*, QSGNode* ) const;
};

#endif
