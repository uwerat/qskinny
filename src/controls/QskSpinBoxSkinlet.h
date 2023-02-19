/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SPIN_BOX_SKINLET_H
#define QSK_SPIN_BOX_SKINLET_H

#include <QskSkinlet.h>

class QSK_EXPORT QskSpinBoxSkinlet : public QskSkinlet
{
    Q_GADGET
    using Inherited = QskSkinlet;

  public:
    Q_INVOKABLE QskSpinBoxSkinlet( QskSkin* = nullptr );

    enum NodeRole
    {
        IncrementPanel, 
        DecrementPanel,
        TextPanel,

        IncrementIndicator,
        DecrementIndicator,
        Text,

        RoleCount
    };

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable* skinnable,
        Qt::SizeHint, const QSizeF& ) const override;

    int sampleCount(
        const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const override;

    QRectF sampleRect( const QskSkinnable* skinnable, const QRectF& rect,
        QskAspect::Subcontrol subControl, int index ) const override;

    QskAspect::States sampleStates( const QskSkinnable*,
        QskAspect::Subcontrol, int index ) const override;

  protected:
    QSGNode* updateSubNode(
        const QskSkinnable* skinnable, quint8 role, QSGNode* node ) const override;

    QSGNode* updateSampleNode( const QskSkinnable*, QskAspect::Subcontrol,
        int index, QSGNode* node ) const override;
};

#endif
