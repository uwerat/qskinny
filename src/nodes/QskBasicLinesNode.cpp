/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskBasicLinesNode.h"

#include <qsgmaterial.h>
#include <qsggeometry.h>
#include <QTransform>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
QSK_QT_PRIVATE_END

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

        QVector4D m_color = QVector4D{ 0, 0, 0, 1 };
        Qt::Orientations m_pixelAlignment;
    };

    class ShaderRhi final : public RhiShader
    {
      public:

        ShaderRhi()
        {
            const QString root( ":/qskinny/shaders/" );

            setShaderFileName( VertexStage, root + "crisplines.vert.qsb" );
            setShaderFileName( FragmentStage, root + "crisplines.frag.qsb" );
        }

        bool updateUniformData( RenderState& state,
            QSGMaterial* newMaterial, QSGMaterial* oldMaterial ) override
        {
            auto matOld = static_cast< Material* >( oldMaterial );
            auto matNew = static_cast< Material* >( newMaterial );

            Q_ASSERT( state.uniformData()->size() >= 88 );

            auto data = state.uniformData()->data();
            bool changed = false;

            const auto matrix = state.combinedMatrix();

            if ( state.isMatrixDirty() )
            {
                memcpy( data + 0, matrix.constData(), 64 );
                changed = true;
            }

            if ( ( matOld == nullptr ) || ( matNew->m_color != matOld->m_color ) )
            {
                // state.opacity() TODO ...
                memcpy( data + 64, &matNew->m_color, 16 );
                changed = true;
            }

            if ( state.isMatrixDirty() || ( matOld == nullptr )
                || ( matNew->m_pixelAlignment != matOld->m_pixelAlignment ) )
            {
                /*
                    We do not need to upload the size as it is available
                    from the matrix. But the shader needs to know wether to
                    round or not TODO ...
                 */
                QVector2D size;

                if ( matNew->m_pixelAlignment & Qt::Horizontal )
                    size.setX( 2.0 / matrix( 0, 0 ) );

                if ( matNew->m_pixelAlignment & Qt::Vertical )
                    size.setY( -2.0 / matrix( 1, 1 ) );

                memcpy( data + 80, &size, 8 );
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

            setShaderSourceFile( QOpenGLShader::Vertex,
                ":/qskinny/shaders/crisplines.vert" );

            setShaderSourceFile( QOpenGLShader::Fragment,
                ":/qt-project.org/scenegraph/shaders/flatcolor.frag" );
        }

        char const* const* attributeNames() const override
        {
            static char const* const names[] = { "in_vertex", nullptr };
            return names;
        }

        void initialize() override
        {
            QSGMaterialShader::initialize();

            auto p = program();

            m_matrixId = p->uniformLocation( "matrix" );
            m_colorId = p->uniformLocation( "color" );
            m_sizeId = p->uniformLocation( "size" );
        }

        void updateState( const QSGMaterialShader::RenderState& state,
            QSGMaterial* newMaterial, QSGMaterial* oldMaterial) override
        {
            auto p = program();

            const auto matrix = state.combinedMatrix();

            if ( state.isMatrixDirty() )
                p->setUniformValue( m_matrixId, matrix );

            bool updateMaterial = ( oldMaterial == nullptr )
                || newMaterial->compare( oldMaterial ) != 0;

            updateMaterial |= state.isCachedMaterialDataDirty();

            if ( updateMaterial )
            {
                auto material = static_cast< const Material* >( newMaterial );

                p->setUniformValue( m_colorId, material->m_color );

                QVector2D size;

                if ( material->m_pixelAlignment & Qt::Horizontal )
                    size.setX( 2.0 / matrix( 0, 0 ) );

                if ( material->m_pixelAlignment & Qt::Vertical )
                    size.setY( -2.0 / matrix( 1, 1 ) );

                p->setUniformValue( m_sizeId, size );
            }
        }

      private:
        int m_matrixId = -1;
        int m_colorId = -1;
        int m_sizeId = -1;
    };
}

#endif

Material::Material()
{
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
        && ( material->m_pixelAlignment == m_pixelAlignment ) )
    {
        return 0;
    }

    return QSGMaterial::compare( other );
}

class QskBasicLinesNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskBasicLinesNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
    {
        geometry.setDrawingMode( QSGGeometry::DrawLines );
    }

    QSGGeometry geometry;
    Material material;
};

QskBasicLinesNode::QskBasicLinesNode()
    : QSGGeometryNode( *new QskBasicLinesNodePrivate )
{
    Q_D( QskBasicLinesNode );

    setGeometry( &d->geometry );
    setMaterial( &d->material );
}

QskBasicLinesNode::~QskBasicLinesNode()
{
}

void QskBasicLinesNode::setPixelAlignment( Qt::Orientations orientations )
{
    Q_D( QskBasicLinesNode );

    if ( orientations != d->material.m_pixelAlignment )
    {
        d->material.m_pixelAlignment = orientations;
        markDirty( QSGNode::DirtyMaterial );
    }
}

Qt::Orientations QskBasicLinesNode::pixelAlignment() const
{
    return d_func()->material.m_pixelAlignment;
}

void QskBasicLinesNode::setColor( const QColor& color )
{
    Q_D( QskBasicLinesNode );

    const auto a = color.alphaF();

    const QVector4D c( color.redF() * a, color.greenF() * a, color.blueF() * a, a );

    if ( c != d->material.m_color )
    {
        d->material.m_color = c;
        markDirty( QSGNode::DirtyMaterial );
    }
}

void QskBasicLinesNode::setLineWidth( float lineWidth )
{
    Q_D( QskBasicLinesNode );

    lineWidth = std::max( lineWidth, 0.0f );
    if( lineWidth != d->geometry.lineWidth() )
        d->geometry.setLineWidth( lineWidth );
}

float QskBasicLinesNode::lineWidth() const
{
    return d_func()->geometry.lineWidth();
}

