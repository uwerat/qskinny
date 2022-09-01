#pragma once

#include <QskSGNode.h>
#include <QskSkinlet.h>

class BlurredBoxSkinlet : public QskSkinlet
{
  public:
    enum NodeRole
    {
        PanelRole
    };

    BlurredBoxSkinlet();
    QRectF subControlRect(
        const QskSkinnable*, const QRectF& contentsRect, QskAspect::Subcontrol ) const override;
    QSGNode* updateSubNode(
        const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const override;
};
