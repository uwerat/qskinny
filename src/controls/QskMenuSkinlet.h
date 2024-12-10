/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_MENU_SKINLET_H
#define QSK_MENU_SKINLET_H

#include "QskPopupSkinlet.h"
#include <memory>

class QskMenu;

class QSK_EXPORT QskMenuSkinlet : public QskPopupSkinlet
{
    Q_GADGET

    using Inherited = QskPopupSkinlet;

  public:
    enum NodeRole : quint8
    {
        ContentsRole = Inherited::RoleCount,
        PanelRole,

        RoleCount
    };

    Q_INVOKABLE QskMenuSkinlet( QskSkin* = nullptr );
    ~QskMenuSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    int sampleCount( const QskSkinnable*, QskAspect::Subcontrol ) const override;

    QRectF sampleRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, int index ) const override;

    int sampleIndexAt( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, const QPointF& ) const override;

    QskAspect::States sampleStates( const QskSkinnable*,
        QskAspect::Subcontrol, int index ) const override;

    QVariant sampleAt( const QskSkinnable*,
        QskAspect::Subcontrol, int index ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

    QSGNode* updateContentsNode( const QskPopup*, QSGNode* ) const;
    QSGNode* updateMenuNode( const QskSkinnable*, QSGNode* ) const;

    QSGNode* updateSampleNode( const QskSkinnable*,
        QskAspect::Subcontrol, int index, QSGNode* ) const override;

  private:
    QRectF cursorRect( const QskSkinnable*, const QRectF&, int index ) const;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
