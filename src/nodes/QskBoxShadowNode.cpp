/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxShadowNode.h"
#include "QskBoxShapeMetrics.h"

#include <qcolor.h>
#include <qsgmaterialshader.h>
#include <qsgmaterial.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
QSK_QT_PRIVATE_END

// QSGMaterialRhiShader became QSGMaterialShader in Qt6

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    #include <QSGMaterialRhiShader>
    using RhiShader = QSGMaterialRhiShader;
#else
    using RhiShader = QSGMaterialShader;
#endif

namespace
{
    class Material final : public QSGMaterial
    {
      public:
        Material();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
        QSGMaterialShader* createShader() const override;
#else
        QSGMaterialShader* createShader( QSGRendererInterface::RenderMode ) const override;
#endif

        QSGMaterialType* type() const override;

        int compare( const QSGMaterial* other ) const override;

        QVector2D m_aspect = QVector2D{ 1, 1 };
        QVector4D m_radius = QVector4D{ 0, 0, 0, 0 };
        QVector4D m_color = QVector4D{ 0, 0, 0, 1 };
        float m_blurExtent = 0.0;
    };
}

namespace
{
    class ShaderRhi final : public RhiShader
    {
      public:
        ShaderRhi()
        {
            const QString root( ":/qskinny/shaders/" );

            setShaderFileName( VertexStage, root + "boxshadow.vert.qsb" );
            setShaderFileName( FragmentStage, root + "boxshadow.frag.qsb" );
        }

        bool updateUniformData( RenderState& state,
            QSGMaterial* newMaterial, QSGMaterial* oldMaterial ) override
        {
            const auto matOld = static_cast< Material* >( oldMaterial );
            const auto matNew = static_cast< Material* >( newMaterial );

            Q_ASSERT( state.uniformData()->size() >= 112 );

            auto data = state.uniformData()->data();
            bool changed = false;

            if ( state.isMatrixDirty() )
            {
                const auto matrix = state.combinedMatrix();
                memcpy( data + 0, matrix.constData(), 64 );

                changed = true;
            }

            if ( matOld == nullptr || matNew->m_color != matOld->m_color )
            {
                memcpy( data + 64, &matNew->m_color, 16 );
                changed = true;
            }

            if ( matOld == nullptr || matNew->m_radius != matOld->m_radius )
            {
                memcpy( data + 80, &matNew->m_radius, 16 );
                changed = true;
            }

            if ( matOld == nullptr || matNew->m_aspect != matOld->m_aspect )
            {
                memcpy( data + 96, &matNew->m_aspect, 8 );
                changed = true;
            }

            if ( matOld == nullptr || matNew->m_blurExtent != matOld->m_blurExtent )
            {
                memcpy( data + 104, &matNew->m_blurExtent, 4 );
                changed = true;
            }

            if ( state.isOpacityDirty() )
            {
                const float opacity = state.opacity();
                memcpy( data + 108, &opacity, 4 );

                changed = true;
            }

            return changed;
        }
    };
}

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

namespace
{
    // the old type of shader - spcific for OpenGL

    class ShaderGL final : public QSGMaterialShader
    {
      public:
        ShaderGL()
        {
            const QString root( ":/qskinny/shaders/" );

            setShaderSourceFile( QOpenGLShader::Vertex, root + "boxshadow.vert" );
            setShaderSourceFile( QOpenGLShader::Fragment, root + "boxshadow.frag" );
        }

        char const* const* attributeNames() const override
        {
            static char const* const names[] = { "in_vertex", "in_coord", nullptr };
            return names;
        }

        void initialize() override
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

        void updateState( const QSGMaterialShader::RenderState& state,
            QSGMaterial* newMaterial, QSGMaterial* oldMaterial) override
        {
            auto p = program();

            if ( state.isMatrixDirty() )
                p->setUniformValue( m_matrixId, state.combinedMatrix() );

            if ( state.isOpacityDirty() )
                p->setUniformValue( m_opacityId, state.opacity() );

            bool updateMaterial = ( oldMaterial == nullptr )
                || newMaterial->compare( oldMaterial ) != 0;

            updateMaterial |= state.isCachedMaterialDataDirty();

            if ( updateMaterial )
            {
                auto material = static_cast< const Material* >( newMaterial );

                p->setUniformValue( m_aspectId, material->m_aspect );
                p->setUniformValue( m_blurExtentId, material->m_blurExtent);
                p->setUniformValue( m_radiusId, material->m_radius );
                p->setUniformValue( m_colorId, material->m_color );
            }
        }

      private:
        int m_matrixId = -1;
        int m_opacityId = -1;
        int m_aspectId = -1;
        int m_blurExtentId = -1;
        int m_radiusId = -1;
        int m_colorId = -1;
    };
}

#endif

Material::Material()
{
    setFlag( QSGMaterial::Blending, true );

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    setFlag( QSGMaterial::SupportsRhiShader, true );
#endif
}

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

QSGMaterialShader* Material::createShader() const
{
    if ( !( flags() & QSGMaterial::RhiShaderWanted ) )
        return new ShaderGL();

    return new ShaderRhi();
}

#else

QSGMaterialShader* Material::createShader( QSGRendererInterface::RenderMode ) const
{
    return new ShaderRhi();
}

#endif

QSGMaterialType* Material::type() const
{
    static QSGMaterialType staticType;
    return &staticType;
}

int Material::compare( const QSGMaterial* other ) const
{
    auto material = static_cast< const Material* >( other );

    if ( ( material->m_color == m_color )
        && ( material->m_aspect == m_aspect )
        && qFuzzyCompare(material->m_blurExtent, m_blurExtent)
        && qFuzzyCompare(material->m_radius, m_radius) )
    {
        return 0;
    }

    return QSGMaterial::compare( other );
}

class QskBoxShadowNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskBoxShadowNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_TexturedPoint2D(), 4 )
    {
    }

    QSGGeometry geometry;
    Material material;

    QRectF rect;
};

QskBoxShadowNode::QskBoxShadowNode()
    : QSGGeometryNode( *new QskBoxShadowNodePrivate )
{
    Q_D( QskBoxShadowNode );

    setGeometry( &d->geometry );
    setMaterial( &d->material );
}

QskBoxShadowNode::~QskBoxShadowNode()
{
}

void QskBoxShadowNode::setShadowData(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    qreal blurRadius, const QColor& color )
{
    Q_D( QskBoxShadowNode );

    if ( rect != d->rect )
    {
        d->rect = rect;

        QSGGeometry::updateTexturedRectGeometry(
            &d->geometry, d->rect, QRectF( -0.5, -0.5, 1.0, 1.0 ) );

        markDirty( QSGNode::DirtyGeometry );

        QVector2D aspect( 1.0, 1.0 );

        if ( rect.width() >= rect.height() )
            aspect.setX( rect.width() / rect.height() );
        else
            aspect.setY( rect.height() / rect.width() );

        if ( d->material.m_aspect != aspect )
        {
            d->material.m_aspect = aspect;
            markDirty( QSGNode::DirtyMaterial );
        }
    }

    {
        const float t = std::min( d->rect.width(), d->rect.height() );

        const float r1 = shape.radius( Qt::BottomRightCorner ).width();
        const float r2 = shape.radius( Qt::TopRightCorner ).width();
        const float r3 = shape.radius( Qt::BottomLeftCorner ).width();
        const float r4 = shape.radius( Qt::TopLeftCorner ).width();

        const auto uniformRadius = QVector4D(
            std::min( r1 / t, 1.0f ), std::min( r2 / t, 1.0f ),
            std::min( r3 / t, 1.0f ), std::min( r4 / t, 1.0f ) );

        if ( d->material.m_radius != uniformRadius )
        {
            d->material.m_radius = uniformRadius;
            markDirty( QSGNode::DirtyMaterial );
        }
    }

    {
        if ( blurRadius <= 0.0 )
            blurRadius = 0.0;

        const float t = 0.5 * std::min( d->rect.width(), d->rect.height() );
        const float uniformExtent = blurRadius / t;

        if ( !qFuzzyCompare( d->material.m_blurExtent, uniformExtent ) )
        {
            d->material.m_blurExtent = uniformExtent;
            markDirty( QSGNode::DirtyMaterial );
        }
    }

    {
        const auto a = color.alphaF();

        const QVector4D c( color.redF() * a, color.greenF() * a, color.blueF() * a, a );

        if ( d->material.m_color != c )
        {
            d->material.m_color = c;
            markDirty( QSGNode::DirtyMaterial );
        }
    }
}
