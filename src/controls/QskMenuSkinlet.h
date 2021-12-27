#pragma once

#include <QskPopupSkinlet.h>

class QskMenu;

class QSK_EXPORT QskMenuSkinlet : public QskPopupSkinlet
{
    Q_GADGET

    using Inherited = QskPopupSkinlet;

  public:
    enum NodeRole
    {
        PanelRole = QskPopupSkinlet::RoleCount,
        RoleCount
    };

    Q_INVOKABLE QskMenuSkinlet( QskSkin* = nullptr );
    ~QskMenuSkinlet() = default;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QRectF itemRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, int index ) const override;

    int itemIndexAt( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, const QPointF& ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  private:
    QSGNode* updateContentsNode( const QskPopup*, QSGNode* ) const override;
};
