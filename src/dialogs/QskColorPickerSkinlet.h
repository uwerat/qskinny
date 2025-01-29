/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_COLOR_PICKER_SKINLET_H
#define QSK_COLOR_PICKER_SKINLET_H

#include "QskSkinlet.h"

class QskColorPicker;

class QskColorPickerSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole : quint8
    {
        PanelRole,
        ColorPaneRole,
        SelectorRole,

        RoleCount
    };

    Q_INVOKABLE QskColorPickerSkinlet( QskSkin* = nullptr );
    ~QskColorPickerSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSGNode* updateColorPaneNode( const QskColorPicker*, QSGNode* ) const;

    QRectF cursorRect( const QskSkinnable*, const QRectF&, int index ) const;
};

#endif
