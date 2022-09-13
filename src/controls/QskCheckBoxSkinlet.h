/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_CHECK_BOX_SKINLET_H
#define QSK_CHECK_BOX_SKINLET_H

#include "QskSkinlet.h"

class QskCheckBox;

class QSK_EXPORT QskCheckBoxSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
        BoxRole,
        IndicatorRole,
        TextRole,

        RoleCount
    };

    Q_INVOKABLE QskCheckBoxSkinlet( QskSkin* = nullptr );
    ~QskCheckBoxSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QRectF textRect( const QskCheckBox*, const QRectF& ) const;
    QRectF boxRect( const QskCheckBox*, const QRectF& ) const;

    QSGNode* updateIndicatorNode( const QskCheckBox*, QSGNode* ) const;
    QSGNode* updateTextNode( const QskCheckBox*, QSGNode* ) const;
};

#endif
