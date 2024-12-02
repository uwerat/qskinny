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
    QSK_STATES( Filled )

    enum NodeRole
    {
        PanelRole,
        GrooveRole,
        FillRole,
        TicksRole,
        HandleRole,
        LabelContainerRole,
        LabelTextRole,

        RoleCount
    };

    Q_INVOKABLE QskSliderSkinlet( QskSkin* = nullptr );
    ~QskSliderSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

    int sampleCount( const QskSkinnable*, QskAspect::Subcontrol ) const override;

    QRectF sampleRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, int index ) const override;

    QVariant sampleAt( const QskSkinnable*,
        QskAspect::Subcontrol, int index ) const override;

    QskAspect::States sampleStates( const QskSkinnable*,
        QskAspect::Subcontrol, int ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

    QSGNode* updateSampleNode( const QskSkinnable*,
        QskAspect::Subcontrol, int index, QSGNode* ) const override;

    virtual QVector< qreal > graduation( const QskSlider* ) const;
    bool hasGraduation( const QskSlider* ) const;

  private:
    QRectF panelRect( const QskSlider*, const QRectF& ) const;
    QRectF fillRect( const QskSlider*, const QRectF& ) const;
    QRectF handleRect( const QskSlider*, const QRectF& ) const;
    QRectF tickRect( const QskSlider*, const QRectF&, int index ) const;

    QRectF labelContainerRect( const QskSlider*, const QRectF& ) const;
};

#endif
