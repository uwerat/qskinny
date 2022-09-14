#include "BlurredBoxTextureProvider.h"

#include <QQuickWindow>
#include <QSGTexture>

#include <memory>

BlurredBoxTextureProvider::BlurredBoxTextureProvider(QQuickWindow *window) : m_window(window), m_image(window->grabWindow())
{
}

BlurredBoxTextureProvider::~BlurredBoxTextureProvider()
{
    m_window = nullptr;
}

QSGTexture* BlurredBoxTextureProvider::texture() const
{
    return m_window->createTextureFromImage(m_image);
}

QSGTexture *BlurredBoxTextureProvider::texture(QRect roi) const
{
    if (!m_image.rect().contains(roi))
    {
        QImage black(roi.width(), roi.height(), QImage::Format_RGBA8888);
        black.fill(Qt::black);
        return m_window->createTextureFromImage(black);
    }
    const auto image = m_image.copy(roi);
    return m_window->createTextureFromImage(image);
}
