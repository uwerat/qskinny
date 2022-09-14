#pragma once

#include <QSGTextureProvider>
#include <QImage>

class QSGTexture;
class QQuickWindow;

class BlurredBoxTextureProvider final : public QSGTextureProvider
{
public:
    BlurredBoxTextureProvider(QQuickWindow* window);
    BlurredBoxTextureProvider(QQuickWindow* window, QImage image);
    ~BlurredBoxTextureProvider() override;
    QSGTexture* texture() const override;
    QSGTexture* texture(QRect roi) const;
private:
    QQuickWindow* m_window = nullptr;
    QImage m_image;
};
