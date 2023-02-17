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
    enum NodeRole
    {
        IncPanel,
        IncText,
        DecPanel,
        DecText,
        TextPanel,
        TextText,
        RoleCount
    };

    Q_INVOKABLE QskSpinBoxSkinlet( QskSkin* = nullptr );

  protected:
    int sampleCount( const QskSkinnable*, QskAspect::Subcontrol ) const override;

    QRectF sampleRect( const QskSkinnable*, const QRectF&,
        QskAspect::Subcontrol, int index ) const override;

    QskAspect::States sampleStates( const QskSkinnable*,
        QskAspect::Subcontrol, int index ) const override;

    QSizeF sizeHint( const QskSkinnable*, Qt::SizeHint, const QSizeF& ) const override;
    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSGNode* updateSubNode( const QskSkinnable*, quint8 nodeRole, QSGNode* ) const override;
    QSGNode* updateSampleNode( const QskSkinnable*,
        QskAspect::Subcontrol, int index, QSGNode* node ) const override;
};

#endif
