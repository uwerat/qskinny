/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskSkinlet.h>

class Dial;

class DialSkinlet : public QskSkinlet
{
    Q_GADGET

  public:
    enum NodeRole
    {
        PanelRole,
        LabelsRole,
        KnobRole,
        NeedleRole
    };

    Q_INVOKABLE DialSkinlet( QskSkin* skin = nullptr );

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

  protected:
    virtual QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* node ) const override;

  private:
    QRectF scaleRect( const Dial* ) const;
    QLineF needlePoints( const Dial* ) const;

    QSGNode* updateLabelsNode( const Dial*, QSGNode* ) const;
    QSGNode* updateNeedleNode( const Dial*, QSGNode* ) const;
};
