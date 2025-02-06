/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "ArcShadowNode.h"
#include <QskInternalMacros.h>

#include <qcolor.h>
#include <qsgmaterial.h>
#include <qsgmaterialshader.h>
#include <qmath.h>

#include <cstring>

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

        QVector4D m_color { 0, 0, 0, 1 };
        QVector4D m_arc = {};

        float m_spreadRadius = 0.0f;
        float m_blurRadius = 0.0f;
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
            setShaderFileName( VertexStage, root + "arcshadow.vert.qsb" );
            setShaderFileName( FragmentStage, root + "arcshadow.frag.qsb" );
        }

        bool updateUniformData( RenderState& state,
            QSGMaterial* const newMaterial, QSGMaterial* const oldMaterial ) override
        {
            const auto matOld = static_cast< Material* >( oldMaterial );
            const auto matNew = static_cast< Material* >( newMaterial );

            Q_ASSERT( state.uniformData()->size() == 108 );

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

            if ( matOld == nullptr || matNew->m_arc != matOld->m_arc )
            {
                memcpy( data + 80, &matNew->m_arc, 16 );
                changed = true;
            }

            if ( matOld == nullptr || matNew->m_spreadRadius != matOld->m_spreadRadius )
            {
                memcpy( data + 96, &matNew->m_spreadRadius, 4 );
                changed = true;
            }

            if ( matOld == nullptr || matNew->m_blurRadius != matOld->m_blurRadius )
            {
                memcpy( data + 100, &matNew->m_blurRadius, 4 );
                changed = true;
            }

            if ( state.isOpacityDirty() )
            {
                const float opacity = state.opacity();
                memcpy( data + 104, &opacity, 4 );

                changed = true;
            }

            return changed;
        }
    };
}

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

namespace
{
    // the old type of shader - specific for OpenGL

    class ShaderGL final : public QSGMaterialShader
    {
        struct Uniforms
        {
            int matrix = -1;
            int color = -1;
            int arc = -1;
            int spreadRadius = -1;
            int blurRadius = -1;
            int opacity = -1;
        };

      public:
        ShaderGL()
        {
            const QString root( ":/qskinny/shaders/" );
            setShaderSourceFile( QOpenGLShader::Vertex, root + "arcshadow.vert" );
            setShaderSourceFile( QOpenGLShader::Fragment, root + "arcshadow.frag" );
        }

        char const* const* attributeNames() const override
        {
            static char const* const names[] = { "in_vertex", "in_coord", nullptr };
            return names;
        }

        void initialize() override
        {
            QSGMaterialShader::initialize();

            const auto* const p = program();

            id.matrix = p->uniformLocation( "matrix" );
            id.color = p->uniformLocation( "color" );
            id.arc = p->uniformLocation( "arc" );
            id.spreadRadius = p->uniformLocation( "spreadRadius" );
            id.blurRadius = p->uniformLocation( "blurRadius" );
            id.opacity = p->uniformLocation( "opacity" );
        }

        void updateState( const QSGMaterialShader::RenderState& state,
            QSGMaterial* const newMaterial, QSGMaterial* const oldMaterial ) override
        {
            auto* const p = program();

            if ( state.isMatrixDirty() )
            {
                p->setUniformValue( id.matrix, state.combinedMatrix() );
            }

            if ( state.isOpacityDirty() )
            {
                p->setUniformValue( id.opacity, state.opacity() );
            }

            auto updateMaterial = ( oldMaterial == nullptr ) ||
                ( newMaterial->compare( oldMaterial ) != 0 );

            updateMaterial |= state.isCachedMaterialDataDirty();

            if ( updateMaterial )
            {
                const auto* const material = static_cast< const Material* >( newMaterial );

                p->setUniformValue( id.color, material->m_color );
                p->setUniformValue( id.arc, material->m_arc );
                p->setUniformValue( id.spreadRadius, material->m_spreadRadius );
                p->setUniformValue( id.blurRadius, material->m_blurRadius );
            }
        }

      private:
        Uniforms id;
    };
}

#endif

namespace
{
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

    int Material::compare( const QSGMaterial* const other ) const
    {
        auto material = static_cast< const Material* >( other );

        if ( ( material->m_color == m_color )
            && ( material->m_arc == m_arc )
            && qFuzzyCompare( material->m_spreadRadius, m_spreadRadius )
            && qFuzzyCompare( material->m_blurRadius, m_blurRadius ) )
        {
            return 0;
        }

        return QSGMaterial::compare( other );
    }
}

class ArcShadowNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    ArcShadowNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_TexturedPoint2D(), 4 )
    {
    }

    QSGGeometry geometry;
    Material material;
    QRectF rect;
};

ArcShadowNode::ArcShadowNode()
    : QSGGeometryNode( *new ArcShadowNodePrivate )
{
    Q_D( ArcShadowNode );

    setGeometry( &d->geometry );
    setMaterial( &d->material );

    d->geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );
    d->material.setFlag( QSGMaterial::Blending );
}

ArcShadowNode::~ArcShadowNode() = default;

void ArcShadowNode::setShadowData(
    const QRectF& rect, qreal spreadRadius, qreal blurRadius,
    qreal startAngle, qreal spanAngle, const QColor& color )
{
    if ( qFuzzyIsNull( spanAngle ) || color.alpha() == 0 )
    {
        setBoundingRectangle( {} );
        return;
    }

    Q_D( ArcShadowNode );

    if ( d->rect != rect )
    {
        setBoundingRectangle( rect ); // bounding rectangle includig spread/blur
    }

    const auto size = qMin( rect.width(), rect.height() );

    {
#if 1
        const auto a = color.alphaF();
        const QVector4D c( color.redF() * a, color.greenF() * a, color.blueF() * a, a );
#else
        const QVector4D c( color.redF(), color.greenF(), color.blueF(), color.alphaF() );
#endif

        if ( d->material.m_color != c )
        {
            d->material.m_color = c;
            markDirty( QSGNode::DirtyMaterial );
        }
    }

    {
        const float r = spreadRadius / size;

        if ( d->material.m_spreadRadius != r )
        {
            d->material.m_spreadRadius = r;
            markDirty( QSGNode::DirtyMaterial );
        }
    }

    {
        const float r = blurRadius / size;

        if ( d->material.m_blurRadius != r )
        {
            d->material.m_blurRadius = r;
            markDirty( QSGNode::DirtyMaterial );
        }
    }

    {
        QVector4D arc( 0.0, 0.0, 1.0, 0.0 );

        {
            const auto a1 = qDegreesToRadians( startAngle + 0.5 * spanAngle );
            const auto a2 = qDegreesToRadians( 0.5 * qAbs( spanAngle ) );

            arc = QVector4D( ::cos( a1 ), ::sin( a1 ), ::cos( a2 ), ::sin( a2 ) );
        }

        if ( d->material.m_arc != arc )
        {
            d->material.m_arc = arc;
            markDirty( QSGNode::DirtyMaterial );
        }
    }
}

void ArcShadowNode::setBoundingRectangle( const QRectF& rect )
{
    Q_D( ArcShadowNode );

    if ( d->rect == rect )
        return;

    d->rect = rect;

    QSGGeometry::updateTexturedRectGeometry(
        &d->geometry, d->rect, { -0.5, -0.5, 1.0, 1.0 } );
    d->geometry.markVertexDataDirty();

    markDirty( QSGNode::DirtyGeometry );
}
