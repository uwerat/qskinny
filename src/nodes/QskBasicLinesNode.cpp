/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskBasicLinesNode.h"
#include "QskInternalMacros.h"

#include <qsgmaterial.h>
#include <qsggeometry.h>
#include <QTransform>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
QSK_QT_PRIVATE_END

static inline QVector4D qskColorVector( const QColor& c, qreal opacity)
{
    const auto a = c.alphaF() * opacity;
    return QVector4D( c.redF() * a, c.greenF() * a, c.blueF() * a, a );
}

static inline QVector2D qskOrigin(
    const QRect& rect, Qt::Orientations orientations )
{
    return QVector2D(
        ( orientations & Qt::Horizontal ) ? 0.5 * rect.width() : 0.0,
        ( orientations & Qt::Vertical ) ? 0.5 * rect.height() : 0.0
    );
}

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

        QColor m_color = QColor( 255, 255, 255 );
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

            if ( ( matOld == nullptr ) || ( matNew->m_color != matOld->m_color )
                || state.isOpacityDirty() )
            {
                const auto v4 = qskColorVector( matNew->m_color, state.opacity() );
                memcpy( data + 64, &v4, 16 );
                changed = true;
            }

            if ( state.isMatrixDirty() || ( matOld == nullptr )
                || ( matNew->m_pixelAlignment != matOld->m_pixelAlignment ) )
            {
                /*
                    The shaders work with coordinates in the range[-1,1]. When knowing
                    the device coordinates corresponding to [0.0] we can scale a vertex
                    into device coordinates.

                    coordinates <= 0.0 indicate, that no rounding should be done.
                 */
                const auto origin = qskOrigin(
                    state.viewportRect(), matNew->m_pixelAlignment );

                memcpy( data + 80, &origin, 8 );
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
            m_originId = p->uniformLocation( "origin" );
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

                p->setUniformValue( m_colorId,
                    qskColorVector( material->m_color, state.opacity() ) );

                const auto origin = qskOrigin(
                    state.viewportRect(), material->m_pixelAlignment );;
                p->setUniformValue( m_originId, origin );
            }
        }

      private:
        int m_matrixId = -1;
        int m_colorId = -1;
        int m_originId = -1;
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

    const auto c = color.toRgb();
    if ( c != d->material.m_color )
    {
        d->material.m_color = c;
        markDirty( QSGNode::DirtyMaterial );
    }
}

QColor QskBasicLinesNode::color() const
{
    return d_func()->material.m_color;
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

