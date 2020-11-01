#include "QskTextureNode.h"
#include "QskTextureRenderer.h"

#include <qopenglfunctions.h>
#include <qsggeometry.h>
#include <qsgmaterial.h>
#include <qquickwindow.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
QSK_QT_PRIVATE_END


namespace
{
    class MaterialShader final : public QSGMaterialShader
    {
      public:
        MaterialShader( bool isOpaque );

        char const* const* attributeNames() const override;
        void updateState( const RenderState&, QSGMaterial*, QSGMaterial* ) override;

      protected:
        void initialize() override;

      private:
        int m_matrixId = -1;
        int m_opacityId = -1;

        const bool m_isOpaque : 1;
    };

    class Material final : public QSGMaterial
    {
      public:
        Material( bool isOpaque );

        QSGMaterialType* type() const override;
        QSGMaterialShader* createShader() const override;

        void setTextureId( uint );
        uint textureId() const;

        int compare( const QSGMaterial* ) const override;

      private:
        uint m_textureId = 0;
        const bool m_isOpaque : 1;
    };

    MaterialShader::MaterialShader( bool isOpaque )
        : m_isOpaque( isOpaque )
    {
        setShaderSourceFile( QOpenGLShader::Vertex,
            QStringLiteral( ":/qt-project.org/scenegraph/shaders/opaquetexture.vert" ) );

        const auto fragmentShaderFile = m_isOpaque
            ? QStringLiteral( ":/qt-project.org/scenegraph/shaders/opaquetexture.frag" )
            : QStringLiteral( ":/qt-project.org/scenegraph/shaders/texture.frag" );

        setShaderSourceFile( QOpenGLShader::Fragment, fragmentShaderFile );
    }

    char const* const* MaterialShader::attributeNames() const
    {
        static char const* const attr[] = { "qt_VertexPosition", "qt_VertexTexCoord", 0 };
        return attr;
    }

    void MaterialShader::updateState(
        const RenderState& state, QSGMaterial* newMaterial, QSGMaterial* oldMaterial )
    {
        if ( !m_isOpaque && state.isOpacityDirty() )
            program()->setUniformValue( m_opacityId, state.opacity() );

        auto* materialOld = static_cast< Material* >( oldMaterial );
        auto* materialNew = static_cast< Material* >( newMaterial );

        if ( ( materialOld == nullptr ) ||
            ( materialOld->textureId() != materialNew->textureId() ) )
        {
            auto funcs = QOpenGLContext::currentContext()->functions();
            funcs->glBindTexture( GL_TEXTURE_2D, materialNew->textureId() );
        }

        if ( state.isMatrixDirty() )
            program()->setUniformValue( m_matrixId, state.combinedMatrix() );
    }

    void MaterialShader::initialize()
    {
        m_matrixId = program()->uniformLocation( "qt_Matrix" );

        if ( !m_isOpaque )
            m_opacityId = program()->uniformLocation( "opacity" );
    }

    Material::Material( bool isOpaque )
        : m_isOpaque( isOpaque )
    {
        setFlag( Blending, true ); // alpha blending
    }

    void Material::setTextureId( uint id )
    {
        m_textureId = id;
    }

    uint Material::textureId() const
    {
        return m_textureId;
    }

    QSGMaterialType* Material::type() const
    {
        if ( m_isOpaque )
        {
            static QSGMaterialType typeOpaque;
            return &typeOpaque;
        }
        else
        {
            static QSGMaterialType type;
            return &type;
        }
    }

    QSGMaterialShader* Material::createShader() const
    {
        return new MaterialShader( m_isOpaque );
    }

    int Material::compare( const QSGMaterial* other ) const
    {
        const auto otherMaterial = static_cast< const Material* >( other );

        if ( m_textureId == otherMaterial->m_textureId )
            return 0;

        return ( m_textureId > otherMaterial->m_textureId ) ? 1 : -1;
    }
}

class QskTextureNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskTextureNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_TexturedPoint2D(), 4 )
        , opaqueMaterial( true )
        , material( false )
    {
    }

    void setTextureId( QQuickWindow*, uint id );

    void updateTextureGeometry( const QQuickWindow* window )
    {
        QRectF r( 0, 0, 1, 1 );

        if ( this->mirrored & Qt::Horizontal )
        {
            r.setLeft( 1 );
            r.setRight( 0 );
        }

        if ( mirrored & Qt::Vertical )
        {
            r.setTop( 1 );
            r.setBottom( 0 );
        }

        const qreal ratio = window->effectiveDevicePixelRatio();

        const QRectF scaledRect( rect.x(), rect.y(),
            rect.width() / ratio, rect.height() / ratio );

        QSGGeometry::updateTexturedRectGeometry( &geometry, scaledRect, r );
    }

    QSGGeometry geometry;

    Material opaqueMaterial;
    Material material;

    QRectF rect;
    Qt::Orientations mirrored;
};

QskTextureNode::QskTextureNode()
    : QSGGeometryNode( *new QskTextureNodePrivate )
{
    Q_D( QskTextureNode );

    setGeometry( &d->geometry );

    setMaterial( &d->material );
    setOpaqueMaterial( &d->opaqueMaterial );
}

QskTextureNode::~QskTextureNode()
{
    Q_D( QskTextureNode );

    if ( d->material.textureId() > 0 )
    {
        /*
            In certain environments we have the effect, that at
            program termination the context is already gone
         */
        if ( auto context = QOpenGLContext::currentContext() )
        {
            GLuint id = d->material.textureId();

            auto funcs = context->functions();
            funcs->glDeleteTextures( 1, &id );
        }
    }
}

void QskTextureNode::setTexture( QQuickWindow* window,
    const QRectF& rect, uint textureId,
    Qt::Orientations mirrored )
{
    Q_D( QskTextureNode );

    if ( d->rect != rect || d->mirrored != mirrored )
    {
        d->rect = rect;
        d->mirrored = mirrored;

        d->updateTextureGeometry( window );

        markDirty( DirtyGeometry );
    }

    if ( textureId != this->textureId() )
    {
        d->setTextureId( window, textureId );
        markDirty( DirtyMaterial );
    }
}

void QskTextureNodePrivate::setTextureId( QQuickWindow*, uint textureId )
{
    if ( this->material.textureId() > 0 )
    {
        GLuint id = this->material.textureId();

        auto funcs = QOpenGLContext::currentContext()->functions();
        funcs->glDeleteTextures( 1, &id );
    }

    this->material.setTextureId( textureId );
    this->opaqueMaterial.setTextureId( textureId );
}

uint QskTextureNode::textureId() const
{
    Q_D( const QskTextureNode );
    return d->material.textureId();
}

bool QskTextureNode::isNull() const
{
    return textureId() == 0;
}

QRectF QskTextureNode::rect() const
{
    Q_D( const QskTextureNode );
    return d->rect;
}

Qt::Orientations QskTextureNode::mirrored() const
{
    Q_D( const QskTextureNode );
    return d->mirrored;
}
