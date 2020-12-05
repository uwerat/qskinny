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
        int m_matrixLocation = -1;
        int m_opacityLocation = -1;
        int m_aspectLocation = -1;
        int m_extentLocation = -1;
        int m_radiusLocation = -1;
        int m_colorLocation = -1;
        int m_offsetLocation = -1;
    };

    class Material final : public QSGMaterial
    {
      public:
        Material();

        QSGMaterialShader* createShader() const override;

        QSGMaterialType* type() const override;

        int compare( const QSGMaterial* other ) const override;

        QVector2D aspect = QVector2D{1.0, 1.0};
        float extent = 0.0;
        QVector4D radius = QVector4D{0.0, 0.0, 0.0, 0.0};
        QColor color = Qt::black;
        QVector2D offset;
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

        m_matrixLocation = p->uniformLocation( "matrix" );
        m_aspectLocation = p->uniformLocation( "aspect" );
        m_opacityLocation = p->uniformLocation( "opacity" );
        m_extentLocation = p->uniformLocation( "extent" );
        m_offsetLocation = p->uniformLocation( "offset" );
        m_radiusLocation = p->uniformLocation( "radius" );
        m_colorLocation = p->uniformLocation( "color" );
    }

    void Shader::updateState( const QSGMaterialShader::RenderState& state,
        QSGMaterial* newMaterial, QSGMaterial* oldMaterial )
    {
        auto p = program();

        if ( state.isMatrixDirty() )
            p->setUniformValue(m_matrixLocation, state.combinedMatrix() );

        if ( state.isOpacityDirty() )
            p->setUniformValue(m_opacityLocation, state.opacity() );

        if ( oldMaterial == nullptr || newMaterial->compare( oldMaterial ) != 0 )
        {
            auto material = static_cast< const Material* >( newMaterial );

            p->setUniformValue( m_aspectLocation, material->aspect );
            p->setUniformValue( m_extentLocation, material->extent );
            p->setUniformValue( m_radiusLocation, material->radius );
            p->setUniformValue( m_colorLocation, material->color );
            p->setUniformValue( m_offsetLocation, material->offset );
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
            && material->offset == offset
            && material->aspect == aspect
            && qFuzzyCompare(material->extent, extent)
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

    QVector2D newAspect( 1.0, 1.0 );

    if ( rect.width() >= rect.height() )
        newAspect.setX( rect.width() / rect.height() );
    else
        newAspect.setY( rect.height() / rect.width() );

    if ( d->material.aspect != newAspect)
    {
        d->material.aspect = newAspect;
        markDirty( QSGNode::DirtyMaterial );
    }
}

void BoxShadowNode::setShape( const QskBoxShapeMetrics& shape )
{
    Q_D( BoxShadowNode );

    const float t = 0.5 * std::min( d->rect.width(), d->rect.height() );

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
        markDirty(QSGNode::DirtyMaterial);
    }
}

void BoxShadowNode::setShadow( qreal extent, qreal dx, qreal dy )
{
    Q_D( BoxShadowNode );

    if ( extent <= 0.0 )
        extent = 0.0;

    const auto minDimension = std::min( d->rect.width(), d->rect.height() );

    const float uniformExtent = ( extent / minDimension ) * 2.0;

    if ( !qFuzzyCompare( d->material.extent, uniformExtent ) )
    {
        d->material.extent = uniformExtent;
        markDirty(QSGNode::DirtyMaterial);
    }

    const auto uniformOffset = QVector2D( dx, dy ) / minDimension;

    if ( d->material.offset != uniformOffset)
    {
        d->material.offset = uniformOffset;
        markDirty( QSGNode::DirtyMaterial );
    }
}

void BoxShadowNode::updateGeometry()
{
    Q_D( BoxShadowNode );

    const auto sz = d->material.extent;
    const auto aspect = d->material.aspect;

    auto rect = d->rect.adjusted(
        -sz * aspect.x(), -sz * aspect.y(),
        sz * aspect.x(), sz * aspect.y() );

    auto offsetLength = d->material.offset.length();

    rect = rect.adjusted(
        -offsetLength * aspect.x(), -offsetLength * aspect.y(),
        offsetLength * aspect.x(), offsetLength * aspect.y() );

    QSGGeometry::updateTexturedRectGeometry(
        &d->geometry, rect, QRectF( 0.0, 0.0, 1.0, 1.0 ) );

    markDirty( QSGNode::DirtyGeometry );
}
