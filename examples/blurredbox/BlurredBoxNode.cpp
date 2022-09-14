#include "BlurredBoxNode.h"
#include "BlurredBoxMaterial.h"
#include "BlurredBoxTextureProvider.h"

#include <QskBoxShapeMetrics.h>

#include <qcolor.h>
#include <qsgmaterial.h>
#include <qsgmaterialshader.h>

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QQuickWindow>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
QSK_QT_PRIVATE_END

#include <limits>

class BlurredBoxNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    BlurredBoxNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_TexturedPoint2D(), 4 )
    {
    }

    QSGGeometry geometry;
    BlurredBoxMaterial material;
    QRectF rect;
};

BlurredBoxNode::BlurredBoxNode()
    : QSGGeometryNode( *new BlurredBoxNodePrivate )
{
    Q_D( BlurredBoxNode );

    setGeometry( &d->geometry );
    setMaterial( &d->material );
    setFlag(QSGNode::UsePreprocess);
}

void BlurredBoxNode::preprocess()
{
    Q_D( BlurredBoxNode );
    if(d->material.m_texture)
    {
        if (auto* dynamicTexture = qobject_cast<QSGDynamicTexture*>(d->material.m_texture.get() ))
        {
            dynamicTexture->updateTexture();
        }
    }
}

void BlurredBoxNode::setBlurData( const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QRectF& rectOnScreen, float opacity, float blurDirections,
    float blurQuality, float blurSize, BlurredBoxTextureProvider* textureProvider)
{
    Q_D( BlurredBoxNode );

    d->rect = rect;

    if(!d->material.m_texture)
    {
        d->material.m_texture = std::unique_ptr<QSGTexture>(textureProvider->texture(rectOnScreen.toRect()));
    }

    QSGGeometry::updateTexturedRectGeometry( &d->geometry, d->rect, {0.0,0.0,1.0,1.0} /* texture coordinates */);

    // update all four corner radii
    const auto size = std::min( d->rect.width(), d->rect.height() );
    d->material.m_rectCornerRadii = {
        static_cast< float >( std::min( 1.0, shape.radius( Qt::TopLeftCorner ).width() / size)),
        static_cast< float >( std::min( 1.0, shape.radius( Qt::TopRightCorner ).width() / size)),
        static_cast< float >( std::min( 1.0, shape.radius( Qt::BottomRightCorner ).width() / size)),
        static_cast< float >( std::min( 1.0, shape.radius( Qt::BottomLeftCorner ).width() / size))
    };

    // update the blurring radii
    d->material.m_blurRadius = {
        static_cast< float >( blurSize / rectOnScreen.width()),
        static_cast< float >( blurSize / rectOnScreen.height())
    };

    // updated rectangle's aspect ratio
    const auto cond = rect.width() >= rect.height();
    d->material.m_rectAspect = {
        static_cast<float>(cond ? rect.width() / rect.height() : 1.0),
        static_cast<float>(cond ? 1.0 : rect.height() / rect.width())
    };

    d->material.m_opacity = opacity;
    d->material.m_blurDirections = std::max(blurDirections, std::numeric_limits<float>::min());
    d->material.m_blurQuality = std::max(blurQuality, std::numeric_limits<float>::min());
    d->material.m_edgeSoftness = static_cast<float>(1.0f / std::max(1.0, rect.width()));

    markDirty( QSGNode::DirtyGeometry );
    markDirty( QSGNode::DirtyMaterial );
}
