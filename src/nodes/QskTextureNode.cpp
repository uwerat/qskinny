#include "QskTextureNode.h"

#include <QSGGeometry>
#include <QSGMaterial>
#include <QSGMaterialShader>
#include <QOpenGLFunctions>

#include <private/qsgnode_p.h>

namespace
{
    class MaterialShader final : public QSGMaterialShader
    {
    public:
        MaterialShader( bool isOpaque );

        virtual char const* const* attributeNames() const override final;
        virtual void updateState( const RenderState&, QSGMaterial*, QSGMaterial* ) override final;

    protected:
        virtual void initialize() override final;

    private:
        int m_matrixId;
        int m_opacityId;

        const bool m_isOpaque : 1;
    };

    class Material final : public QSGMaterial
    {
    public:
        Material( bool isOpaque );

        virtual QSGMaterialType* type() const override;
        virtual QSGMaterialShader* createShader() const override;

        void setTextureId(int );
        int textureId() const;

        virtual int compare(const QSGMaterial* ) const override;

    private:
        int m_textureId;
        const bool m_isOpaque : 1;
    };

    MaterialShader::MaterialShader( bool isOpaque ):
        m_isOpaque( isOpaque )
    {
        setShaderSourceFile(QOpenGLShader::Vertex,
            QStringLiteral(":/qt-project.org/scenegraph/shaders/opaquetexture.vert") );

        setShaderSourceFile(QOpenGLShader::Fragment,
            QStringLiteral(":/qt-project.org/scenegraph/shaders/opaquetexture.frag") );

        if ( !m_isOpaque )
        {
            setShaderSourceFile(QOpenGLShader::Fragment,
                QStringLiteral(":/qt-project.org/scenegraph/shaders/texture.frag") );
        }
    }

    char const* const* MaterialShader::attributeNames() const
    {
        static char const* const attr[] = { "qt_VertexPosition", "qt_VertexTexCoord", 0 };
        return attr;
    }

    void MaterialShader::updateState( const RenderState& state,
        QSGMaterial* newMaterial, QSGMaterial* oldMaterial)
    {
        if ( !m_isOpaque && state.isOpacityDirty() )
            program()->setUniformValue( m_opacityId, state.opacity() );

        auto* materialOld = static_cast< Material* >( oldMaterial );
        auto* materialNew = static_cast< Material* >( newMaterial );

        if ( ( materialOld == nullptr ) || ( materialOld->textureId() != materialNew->textureId() ) )
        {
            auto funcs = QOpenGLContext::currentContext()->functions();
            funcs->glBindTexture( GL_TEXTURE_2D, materialNew->textureId() );
        }

        if ( state.isMatrixDirty() )
            program()->setUniformValue( m_matrixId, state.combinedMatrix() );
    }

    void MaterialShader::initialize()
    {
        m_matrixId = program()->uniformLocation("qt_Matrix");

        if ( !m_isOpaque )
            m_opacityId = program()->uniformLocation("opacity");
    }

    Material::Material( bool isOpaque ) :
        m_textureId(0),
        m_isOpaque( isOpaque )
    {
        setFlag(Blending, true ); // alpha blending
    }

    void Material::setTextureId( int id )
    {
        m_textureId = id;
    }

    int Material::textureId() const
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
        return m_textureId - otherMaterial->m_textureId;
    }
}

class QskTextureNodePrivate : public QSGGeometryNodePrivate
{
public:
    QskTextureNodePrivate():
        geometry( QSGGeometry::defaultAttributes_TexturedPoint2D(), 4 ),
        opaqueMaterial( true ),
        material( false )
    {
    }

    QSGGeometry geometry;

    Material opaqueMaterial;
    Material material;

    QRectF rect;
    Qt::Orientations mirrorOrientations;
};

QskTextureNode::QskTextureNode() :
    QSGGeometryNode( *new QskTextureNodePrivate )
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
        GLuint id = d->material.textureId();

        auto funcs = QOpenGLContext::currentContext()->functions();
        funcs->glDeleteTextures( 1, &id );
    }
}

void QskTextureNode::setRect(const QRectF& r)
{
    Q_D( QskTextureNode );

    if ( d->rect == r)
        return;

    d->rect = r;

    updateTexture();
    markDirty( DirtyGeometry );
}

QRectF QskTextureNode::rect() const
{
    Q_D( const QskTextureNode );
    return d->rect;
}

void QskTextureNode::setTextureId( int textureId )
{
    Q_D( QskTextureNode );

    if ( textureId == d->material.textureId() )
        return;

    if ( d->material.textureId() > 0 )
    {
        GLuint id = d->material.textureId();

        auto funcs = QOpenGLContext::currentContext()->functions();
        funcs->glDeleteTextures( 1, &id );
    }

    d->material.setTextureId( textureId );
    d->opaqueMaterial.setTextureId( textureId );
    updateTexture();

    DirtyState dirty = DirtyMaterial;
#if 0
    // if old/new is in the atlas
    dirty |= DirtyGeometry;
#endif

    markDirty( dirty );
}

int QskTextureNode::textureId() const
{
    Q_D( const QskTextureNode );
    return d->material.textureId();
}

void QskTextureNode::setMirrored( Qt::Orientations orientations )
{
    Q_D( QskTextureNode );

    if ( d->mirrorOrientations == orientations )
        return;

    d->mirrorOrientations = orientations;
    updateTexture();

    markDirty(DirtyMaterial);
}

Qt::Orientations QskTextureNode::mirrored() const
{
    Q_D( const QskTextureNode );
    return d->mirrorOrientations;
}

void QskTextureNode::updateTexture()
{
    Q_D( QskTextureNode );

    QRectF r( 0, 0, 1, 1 );

    if ( d->mirrorOrientations & Qt::Horizontal )
    {
        r.setLeft( 1 );
        r.setRight( 0 );
    }

    if ( d->mirrorOrientations & Qt::Vertical )
    {
        r.setTop( 1 );
        r.setBottom( 0 );
    }

    QSGGeometry::updateTexturedRectGeometry( &d->geometry, d->rect, r );
}
