/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_MATERIAL3_TEXTFIELD_SKINLET_H
#define QSK_MATERIAL3_TEXTFIELD_SKINLET_H

#include "QskMaterial3Global.h"
#include "QskTextFieldSkinlet.h"

class QSK_MATERIAL3_EXPORT QskMaterial3TextFieldSkinlet : public QskTextFieldSkinlet
{
    Q_GADGET

    using Inherited = QskTextFieldSkinlet;

  public:
    Q_INVOKABLE QskMaterial3TextFieldSkinlet( QskSkin* = nullptr );
    ~QskMaterial3TextFieldSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;
};

#endif
