/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FLUENT2_TEXTAREA_SKINLET_H
#define QSK_FLUENT2_TEXTAREA_SKINLET_H

#include "QskFluent2Global.h"
#include "QskTextAreaSkinlet.h"

class QSK_FLUENT2_EXPORT QskFluent2TextAreaSkinlet : public QskTextAreaSkinlet
{
    Q_GADGET

    using Inherited = QskTextAreaSkinlet;

  public:
    Q_INVOKABLE QskFluent2TextAreaSkinlet( QskSkin* = nullptr );
    ~QskFluent2TextAreaSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;
};

#endif
