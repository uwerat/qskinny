/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FLUENT2_TEXTFIELD_SKINLET_H
#define QSK_FLUENT2_TEXTFIELD_SKINLET_H

#include "QskFluent2Global.h"
#include "QskTextFieldSkinlet.h"

class QSK_FLUENT2_EXPORT QskFluent2TextFieldSkinlet : public QskTextFieldSkinlet
{
    Q_GADGET

    using Inherited = QskTextFieldSkinlet;

  public:
    Q_INVOKABLE QskFluent2TextFieldSkinlet( QskSkin* = nullptr );
    ~QskFluent2TextFieldSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;
};

#endif
