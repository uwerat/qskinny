/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcShadowNode.h"
#include "QskArcMetrics.h"
#include "QskShadowMetrics.h"

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
    struct MaterialProperties
    {
        QVector4D color{ 0, 0, 0, 1 };
        QVector2D offset;
        float radius = 1.0f;
        float thickness = 0.0f;
        float startAngle = 0.0f;
        float spanAngle = M_PI;
        float spreadRadius = 0.0f;
        float blurRadius = 0.0f;

        Q_REQUIRED_RESULT inline bool operator==(const MaterialProperties& rhs) const noexcept
        {
            return color == rhs.color && offset == rhs.offset &&
                   radius == rhs.radius && thickness == rhs.thickness &&
                   startAngle == rhs.startAngle && spanAngle == rhs.spanAngle &&
                   spreadRadius == rhs.spreadRadius && blurRadius == rhs.blurRadius;
        }

        Q_REQUIRED_RESULT inline bool operator!=(const MaterialProperties& rhs) const noexcept
        {
            return !(*this == rhs);
        }
    };

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

        MaterialProperties properties;
    };
}

namespace
{
    class ShaderRhi final : public RhiShader
    {
        struct UniformBuffer
        {    
            float matrix[4][4];
            MaterialProperties properties;        
            float opacity = 1.0f;

            static_assert(sizeof(properties) == sizeof(float) * 12, "Assuming packed layout!");
        };

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

            const bool dirty = matOld == nullptr || state.isOpacityDirty() || state.isMatrixDirty() || 
                matOld->compare( matNew ) != 0;

            UniformBuffer buffer;

            const auto dstBufferSize = state.uniformData()->size();
            const auto srcBufferSize = sizeof(buffer);

            Q_ASSERT( dstBufferSize >= srcBufferSize );
            if ( dstBufferSize >= srcBufferSize )
            {
                auto* data = state.uniformData()->data();
                std::memcpy(buffer.matrix, state.combinedMatrix().constData(), sizeof(buffer.matrix));            
                buffer.properties = matNew->properties;
                buffer.opacity = state.opacity();
                std::memcpy(data, &buffer, sizeof(buffer));
            }
            return dirty;
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
            int opacity = -1;
            int color = -1;
            int offset = -1;
            int radius = -1;
            int thickness = -1;
            int startAngle = -1;
            int spanAngle = -1;
            int spreadRadius = -1;
            int blurRadius = -1;
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
            id.opacity = p->uniformLocation( "opacity" );
            id.color = p->uniformLocation( "color" );
            id.offset = p->uniformLocation( "offset" );
            id.radius = p->uniformLocation( "radius" );
            id.thickness = p->uniformLocation( "thickness" );
            id.startAngle = p->uniformLocation( "startAngle" );
            id.spanAngle = p->uniformLocation( "spanAngle" );
            id.spreadRadius = p->uniformLocation( "spreadRadius" );
            id.blurRadius = p->uniformLocation( "blurRadius" );
        }

        void updateState( const QSGMaterialShader::RenderState& state,
            QSGMaterial* const newMaterial, QSGMaterial* const oldMaterial) override
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

            const auto updateMaterial = ( oldMaterial == nullptr ) ||
                                        ( newMaterial->compare( oldMaterial ) != 0 ) ||
                                        ( state.isCachedMaterialDataDirty() );

            if ( updateMaterial )
            {
                const auto* const material = static_cast< const Material* >( newMaterial );
                const auto& properties = material->properties;

                p->setUniformValue( id.color, properties.color);
                p->setUniformValue( id.offset, properties.offset);
                p->setUniformValue( id.radius, properties.radius);
                p->setUniformValue( id.thickness, properties.thickness);
                p->setUniformValue( id.startAngle, properties.startAngle);
                p->setUniformValue( id.spanAngle, properties.spanAngle);
                p->setUniformValue( id.spreadRadius, properties.spreadRadius);
                p->setUniformValue( id.blurRadius, properties.blurRadius);
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
        const auto& lhs = *static_cast< const Material* >( this );
        const auto& rhs = *static_cast< const Material* >( other );
        return ( lhs.properties == rhs.properties ) ? 0 : QSGMaterial::compare( other );
    }
}

class QskArcShadowNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskArcShadowNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_TexturedPoint2D(), 4 )
    {
    }

    QSGGeometry geometry;
    Material material;
    QRectF rect;
};

QskArcShadowNode::QskArcShadowNode() : QSGGeometryNode( *new QskArcShadowNodePrivate )
{
    Q_D( QskArcShadowNode );

    setGeometry( &d->geometry );
    setMaterial( &d->material );

    d->geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );
    d->material.setFlag( QSGMaterial::Blending );
}

QskArcShadowNode::~QskArcShadowNode() = default;

namespace
{
    Q_REQUIRED_RESULT inline bool operator==(const QRectF& lhs, const QVector4D& rhs) noexcept
    {
        return lhs.x() == rhs[0] && lhs.y() == rhs[1] && lhs.width() == rhs[2] && lhs.height() == rhs[3];
    }

    Q_REQUIRED_RESULT inline bool operator!=(const QRectF& lhs, const QVector4D& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    Q_REQUIRED_RESULT inline bool compareExchange(float& dst, const float src)
    {
        if ( dst != src ) 
        {
            return true;
        }
        return false;
    }
}

void QskArcShadowNode::update( const QRectF& rect, const QskArcMetrics& arcMetrics,
    const QColor& shadowColor, const QskShadowMetrics& shadowMetrics, const qreal borderWidth )
{
    Q_D( QskArcShadowNode );
    
    const auto w = qMax(0.0, borderWidth);
    const auto shadowRect = rect.adjusted(-w, -w, +w, +w);
    const auto size = qMin( shadowRect.width(), shadowRect.height() );

    const auto color = [](const QColor& color)
    {
        return QVector4D
        {
            qBound(0.0f, ( float ) color.redF(), 1.0f),
            qBound(0.0f, ( float ) color.greenF(), 1.0f),
            qBound(0.0f, ( float ) color.blueF(), 1.0f),
            qBound(0.0f, ( float ) color.alphaF(), 1.0f)
        };
    }
    (shadowColor);

    const auto offset = [](const QskShadowMetrics& metrics, const QRectF& rect)
    {
        QVector2D offset;        
        offset[ 0 ] = ( float ) metrics.offset().x();
        offset[ 1 ] = ( float ) metrics.offset().y();

        if ( metrics.sizeMode() == Qt::AbsoluteSize ) 
        {
            offset[ 0 ] = ( float ) (metrics.offset().x() / rect.width());
            offset[ 1 ] = ( float ) (metrics.offset().y() / rect.height());            
        }

        offset[ 0 ] = qBound( -1.0f, offset[ 0 ], +1.0f );
        offset[ 1 ] = qBound( -1.0f, offset[ 1 ], +1.0f );
        offset[ 0 ] *= -1.0f; // why must I change directions here?
        offset[ 1 ] *= -1.0f; // why must I change directions here?
        return offset;
    }
    (shadowMetrics, shadowRect);   

    const auto thickness = [](const QskArcMetrics& metrics, const QRectF& rect)
    {
        auto thickness = metrics.thickness();
        const auto size = qMin( rect.width(), rect.height() );
        if ( metrics.sizeMode() == Qt::AbsoluteSize ) 
        {
            thickness = thickness / size;
        }
        thickness = qBound( 0.0, thickness, 1.0 );
        return ( float ) thickness;
    }
    (arcMetrics, shadowRect);

    const auto spreadRadius = [](const QskShadowMetrics& metrics, const QRectF& rect)
    {
        auto spreadRadius = metrics.spreadRadius();
        const auto size = qMin( rect.width(), rect.height() );
        if ( metrics.sizeMode() == Qt::AbsoluteSize ) 
        {
            spreadRadius = spreadRadius / size;
        }
        spreadRadius = qBound( 0.0, spreadRadius, 1.0 );
        return (float) spreadRadius;
    }
    (shadowMetrics, shadowRect);

    const auto blurRadius = [](const QskShadowMetrics& metrics, const QRectF& rect){
        auto blurRadius = metrics.blurRadius();
        const auto size = qMin( rect.width(), rect.height() );
        if ( metrics.sizeMode() == Qt::AbsoluteSize ) 
        {
            blurRadius = blurRadius / size;
        }
        blurRadius = qBound( 0.0, blurRadius, 1.0 );
        return (float) blurRadius;
    }(shadowMetrics, shadowRect);

    const auto startAngle = (float) qDegreesToRadians(arcMetrics.startAngle() + 90.0); // why +90 ?
    const auto spanAngle = (float) qDegreesToRadians(arcMetrics.spanAngle());
    const auto radius = ( float ) ( 1.0 - thickness - 2 * w / size );

    const MaterialProperties properties { 
        color,
        offset,
        radius, 
        thickness,
        startAngle,
        spanAngle, 
        spreadRadius,
        blurRadius
    };

    const auto dirtyGeometry = ( d->rect != shadowRect );
    const auto dirtyMaterial = ( properties != d->material.properties );

    if ( dirtyGeometry )
    {        
        d->rect = shadowRect;
        QSGGeometry::updateTexturedRectGeometry( &d->geometry, d->rect, { -0.5, -0.5, 1.0, 1.0 } );
        d->geometry.markVertexDataDirty();
        markDirty( QSGNode::DirtyGeometry );
    }

    if ( dirtyMaterial )
    {
        d->material.properties = properties;
        markDirty( QSGNode::DirtyMaterial );
    }
}