/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_AREA_SKINLET_H
#define QSK_TEXT_AREA_SKINLET_H

#include "QskAbstractTextInputSkinlet.h"

class QskTextArea;

class QSK_EXPORT QskTextAreaSkinlet : public QskAbstractTextInputSkinlet
{
    Q_GADGET

    using Inherited = QskAbstractTextInputSkinlet;

  public:
    enum NodeRole : quint8
    {
        PanelRole = Inherited::RoleCount,

        PlaceholderTextRole,
        RoleCount
    };

    Q_INVOKABLE QskTextAreaSkinlet( QskSkin* = nullptr );
    ~QskTextAreaSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

    virtual QString effectivePlaceholderText( const QskTextArea* ) const;
};

#endif
