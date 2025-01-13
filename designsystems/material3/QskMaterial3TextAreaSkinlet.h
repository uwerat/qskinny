/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_MATERIAL3_TEXTAREA_SKINLET_H
#define QSK_MATERIAL3_TEXTAREA_SKINLET_H

#include "QskMaterial3Global.h"
#include "QskTextAreaSkinlet.h"

class QSK_MATERIAL3_EXPORT QskMaterial3TextAreaSkinlet : public QskTextAreaSkinlet
{
    Q_GADGET

    using Inherited = QskTextAreaSkinlet;

  public:
    Q_INVOKABLE QskMaterial3TextAreaSkinlet( QskSkin* = nullptr );
    ~QskMaterial3TextAreaSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;
};

#endif
