#include "BlurredBoxNode.h"
#include "BlurredBoxMaterial.h"

#include <QskBoxShapeMetrics.h>

#include <qcolor.h>
#include <qsgmaterial.h>
#include <qsgmaterialshader.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
QSK_QT_PRIVATE_END

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
}

void BlurredBoxNode::setBlurData( const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QRectF& rectOfScreen, const QRectF& rectOnScreen, float opacity, float blurDirections,
    float blurQuality, float blurSize )
{
    Q_D( BlurredBoxNode );

    d->rect = rect;

    const QRectF textureRect{ rectOnScreen.x() / rectOfScreen.width(),
        rectOnScreen.y() / rectOfScreen.height(), rectOnScreen.width() / rectOfScreen.width(),
        rectOnScreen.height() / rectOfScreen.height() };

    QSGGeometry::updateTexturedRectGeometry( &d->geometry, d->rect, textureRect );

    // update screen rectangle
    d->material.m_rectOfScreen = { static_cast< float >( rectOfScreen.x() ),
        static_cast< float >( rectOfScreen.y() ), static_cast< float >( rectOfScreen.width() ),
        static_cast< float >( rectOfScreen.height() ) };

    // update rectangle on screen
    d->material.m_rectOnScreen = { static_cast< float >( rectOnScreen.x() ),
        static_cast< float >( rectOnScreen.y() ), static_cast< float >( rectOnScreen.width() ),
        static_cast< float >( rectOnScreen.height() ) };

    // update all four corner radii
    d->material.m_rectCornerRadii = { static_cast< float >(
                                          shape.radius( Qt::TopLeftCorner ).width() ),
        static_cast< float >( shape.radius( Qt::TopRightCorner ).width() ),
        static_cast< float >( shape.radius( Qt::BottomRightCorner ).width() ),
        static_cast< float >( shape.radius( Qt::BottomLeftCorner ).width() ) };

    d->material.m_opacity = opacity;
    d->material.m_blurDirections = blurDirections;
    d->material.m_blurQuality = blurQuality;
    d->material.m_blurSize = blurSize;

    markDirty( QSGNode::DirtyGeometry );
    markDirty( QSGNode::DirtyMaterial );
}
