/******************************************************************************
 * QSkinny - Copyright (C) 2023 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_COMBO_BOX_SKINLET_H
#define QSK_COMBO_BOX_SKINLET_H

#include "QskSkinlet.h"

class QskComboBox;

class QSK_EXPORT QskComboBoxSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
        GraphicRole,
        TextRole,
        OpenMenuGraphicRole,
        SplashRole,

        RoleCount
    };

    Q_INVOKABLE QskComboBoxSkinlet( QskSkin* = nullptr );
    ~QskComboBoxSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QRectF splashRect( const QskComboBox*, const QRectF& ) const;

    QSGNode* updateTextNode( const QskComboBox*, QSGNode* ) const;
    QSGNode* updateSplashNode( const QskComboBox*, QSGNode* ) const;
};

#endif
