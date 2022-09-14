#pragma once

#include <QskSkinlet.h>
#include <memory>
#include "BlurredBoxTextureProvider.h"

class BlurredBoxSkinlet final : public QskSkinlet
{
  public:
    enum NodeRole
    {
        PanelRole
    };

    BlurredBoxSkinlet(std::shared_ptr<BlurredBoxTextureProvider> textureProvider);
    ~BlurredBoxSkinlet() override;
    QRectF subControlRect(
        const QskSkinnable*, const QRectF& contentsRect, QskAspect::Subcontrol ) const override;
    QSGNode* updateSubNode(
        const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const override;
private:
    std::shared_ptr<BlurredBoxTextureProvider> m_textureProvider;
};
