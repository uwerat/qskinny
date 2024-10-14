/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_MATERIAL3_INPUT_SKINLET_H
#define QSK_MATERIAL3_INPUT_SKINLET_H

#include "QskTextInputSkinlet.h"

class QSK_EXPORT QskFluent2TextInputSkinlet : public QskTextInputSkinlet
{
    Q_GADGET

    using Inherited = QskTextInputSkinlet;

  public:
    Q_INVOKABLE QskFluent2TextInputSkinlet( QskSkin* = nullptr );
    ~QskFluent2TextInputSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF adjustSizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;
};

#endif
