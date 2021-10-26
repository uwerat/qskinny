/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "BoxShadowNode.h"
#include "QskBoxShapeMetrics.h"

#include <QColor>
#include <QSGMaterialShader>
#include <QSGMaterial>

#include <private/qsgnode_p.h>

namespace
{
    class Shader final : public QSGMaterialShader
    {
      public:
        Shader();

        char const* const* attributeNames() const override;

        void initialize() override;

        void updateState( const QSGMaterialShader::RenderState& state,
            QSGMaterial* newMaterial, QSGMaterial* oldMaterial) override;

      private:
        int m_matrixId = -1;
        int m_opacityId = -1;
        int m_aspectId = -1;
        int m_blurExtentId = -1;
        int m_radiusId = -1;
        int m_colorId = -1;
    };

    class Material final : public QSGMaterial
    {
      public:
        Material();

        QSGMaterialShader* createShader() const override;

        QSGMaterialType* type() const override;

        int compare( const QSGMaterial* other ) const override;

        QVector2D aspect = QVector2D{1.0, 1.0};
        float blurExtent = 0.0;
        QVector4D radius = QVector4D{0.0, 0.0, 0.0, 0.0};
        QColor color = Qt::black;
    };

    Shader::Shader()
    {
        const QString root( ":/qskinny/shaders/" );

        setShaderSourceFile( QOpenGLShader::Vertex, root + "boxshadow.vert" );
        setShaderSourceFile( QOpenGLShader::Fragment, root + "boxshadow.frag" );
    }

    char const* const* Shader::attributeNames() const
    {
        static char const* const names[] = { "in_vertex", "in_coord", nullptr };
        return names;
    }

    void Shader::initialize()
    {
        QSGMaterialShader::initialize();

        auto p = program();

        m_matrixId = p->uniformLocation( "matrix" );
        m_aspectId = p->uniformLocation( "aspect" );
        m_opacityId = p->uniformLocation( "opacity" );
        m_blurExtentId = p->uniformLocation( "blurExtent" );
        m_radiusId = p->uniformLocation( "radius" );
        m_colorId = p->uniformLocation( "color" );
    }

    void Shader::updateState( const QSGMaterialShader::RenderState& state,
        QSGMaterial* newMaterial, QSGMaterial* oldMaterial )
    {
        auto p = program();

        if ( state.isMatrixDirty() )
            p->setUniformValue( m_matrixId, state.combinedMatrix() );

        if ( state.isOpacityDirty() )
            p->setUniformValue( m_opacityId, state.opacity() );

        bool updateMaterial = ( oldMaterial == nullptr )
            || newMaterial->compare( oldMaterial ) != 0;

#if QT_VERSION >= QT_VERSION_CHECK( 5, 12, 0 )
        updateMaterial |= state.isCachedMaterialDataDirty();
#endif

        if ( updateMaterial )
        {
            auto material = static_cast< const Material* >( newMaterial );

            p->setUniformValue( m_aspectId, material->aspect );
            p->setUniformValue( m_blurExtentId, material->blurExtent);
            p->setUniformValue( m_radiusId, material->radius );
            p->setUniformValue( m_colorId, material->color );
        }
    }

    Material::Material()
    {
        setFlag( QSGMaterial::Blending, true );
    }

    QSGMaterialShader* Material::createShader() const
    {
        return new Shader();
    }

    QSGMaterialType* Material::type() const
    {
        static QSGMaterialType staticType;
        return &staticType;
    }

    int Material::compare( const QSGMaterial* other ) const
    {
        auto material = static_cast< const Material* >( other );

        if ( material->color == color
            && material->aspect == aspect
            && qFuzzyCompare(material->blurExtent, blurExtent)
            && qFuzzyCompare(material->radius, radius) )
        {
            return 0;
        }

        return QSGMaterial::compare(other);
    }
}

class BoxShadowNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    BoxShadowNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_TexturedPoint2D(), 4 )
    {
    }

    QSGGeometry geometry;
    Material material;

    QRectF rect;
};

BoxShadowNode::BoxShadowNode()
    : QSGGeometryNode( *new BoxShadowNodePrivate )
{
    Q_D( BoxShadowNode );

    setGeometry( &d->geometry );
    setMaterial( &d->material );
}

BoxShadowNode::~BoxShadowNode()
{
}

void BoxShadowNode::setRect( const QRectF& rect )
{
    Q_D( BoxShadowNode );

    if ( rect == d->rect )
        return;

    d->rect = rect;

    QVector2D aspect( 1.0, 1.0 );

    if ( rect.width() >= rect.height() )
        aspect.setX( rect.width() / rect.height() );
    else
        aspect.setY( rect.height() / rect.width() );

    if ( d->material.aspect != aspect )
    {
        d->material.aspect = aspect;
        markDirty( QSGNode::DirtyMaterial );
    }
}

void BoxShadowNode::setShape( const QskBoxShapeMetrics& shape )
{
    Q_D( BoxShadowNode );

    const float t = std::min( d->rect.width(), d->rect.height() );

    const float r1 = shape.radius( Qt::BottomRightCorner ).width();
    const float r2 = shape.radius( Qt::TopRightCorner ).width();
    const float r3 = shape.radius( Qt::BottomLeftCorner ).width();
    const float r4 = shape.radius( Qt::TopLeftCorner ).width();

    const auto uniformRadius = QVector4D(
        std::min( r1 / t, 1.0f ), std::min( r2 / t, 1.0f ),
        std::min( r3 / t, 1.0f ), std::min( r4 / t, 1.0f ) );

    if ( d->material.radius != uniformRadius )
    {
        d->material.radius = uniformRadius;

        markDirty( QSGNode::DirtyMaterial );
    }
}

void BoxShadowNode::setColor( const QColor& color )
{
    Q_D( BoxShadowNode );

    const auto a = color.alphaF();

    const auto c = QColor::fromRgbF(
        color.redF() * a, color.greenF() * a, color.blueF() * a, a );

    if ( d->material.color != c )
    {
        d->material.color = c;
        markDirty( QSGNode::DirtyMaterial );
    }
}

void BoxShadowNode::setBlurRadius( qreal blurRadius )
{
    Q_D( BoxShadowNode );

    if ( blurRadius <= 0.0 )
        blurRadius = 0.0;

    const float t = 0.5 * std::min( d->rect.width(), d->rect.height() );
    const float uniformExtent = blurRadius / t;

    if ( !qFuzzyCompare( d->material.blurExtent, uniformExtent ) )
    {
        d->material.blurExtent = uniformExtent;
        markDirty( QSGNode::DirtyMaterial );
    }
}

void BoxShadowNode::setClipRect( const QRectF& rect )
{
    Q_UNUSED( rect )
}

void BoxShadowNode::updateGeometry()
{
    Q_D( BoxShadowNode );

    QSGGeometry::updateTexturedRectGeometry(
        &d->geometry, d->rect, QRectF( -0.5, -0.5, 1.0, 1.0 ) );

    markDirty( QSGNode::DirtyGeometry );
}
