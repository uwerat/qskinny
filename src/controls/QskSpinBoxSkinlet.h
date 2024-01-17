/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SPIN_BOX_SKINLET_H
#define QSK_SPIN_BOX_SKINLET_H

#include <QskSkinlet.h>

class QSK_EXPORT QskSpinBoxSkinlet : public QskSkinlet
{
    Q_GADGET
    using Inherited = QskSkinlet;

  public:
    Q_INVOKABLE QskSpinBoxSkinlet( QskSkin* = nullptr );

    enum NodeRole
    {
        PanelRole,
        TextPanelRole,
        TextRole,

        UpPanelRole,
        UpIndicatorRole,

        DownPanelRole,
        DownIndicatorRole,

        RoleCount
    };

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable* skinnable,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode(
        const QskSkinnable* skinnable, quint8 role, QSGNode* node ) const override;

  private:
    QRectF textPanelRect( const QskSkinnable*, const QRectF& ) const;

    QRectF buttonRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const;
};

#endif
