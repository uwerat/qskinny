/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef MY_TOGGLE_BUTTON_SKINLET_H
#define MY_TOGGLE_BUTTON_SKINLET_H

#include <QskSkinlet.h>

class MyToggleButton;

class MyToggleButtonSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
        CursorRole,

        CheckedPanelRole,
        CheckedLabelRole,
        CheckedIconRole,

        UncheckedPanelRole,
        UncheckedLabelRole,
        UncheckedIconRole
    };

    Q_INVOKABLE MyToggleButtonSkinlet( QskSkin* = nullptr );
    ~MyToggleButtonSkinlet() override = default;

    QRectF subControlRect( const QskSkinnable*, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*, quint8 nodeRole, QSGNode* ) const override;

  private:
    QRectF innerRect( const QskSkinnable*, QskAspect::Subcontrol ) const;
};

#endif
