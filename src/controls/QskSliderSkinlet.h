/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SLIDER_SKINLET_H
#define QSK_SLIDER_SKINLET_H

#include "QskSkinlet.h"

class QskSlider;

class QSK_EXPORT QskSliderSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
        GrooveRole,
        FillRole,
        TicksRole,
        HandleRole,

        RoleCount
    };

    Q_INVOKABLE QskSliderSkinlet( QskSkin* = nullptr );
    ~QskSliderSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QRectF panelRect( const QskSlider*, const QRectF& ) const;
    QRectF fillRect( const QskSlider*, const QRectF& ) const;
    QRectF handleRect( const QskSlider*, const QRectF& ) const;
};

#endif
