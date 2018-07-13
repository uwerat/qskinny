#include "QskTextureNode.h"

#include <QSGGeometry>
#include <QSGMaterial>
#include <QSGMaterialShader>
#include <QOpenGLFunctions>

#include <private/qsgnode_p.h>

#if 1

#include <QSurface>
#include <QWindow>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QScreen>

static inline qreal qskDevicePixelRatio()
{
    qreal ratio = 1.0;

    const auto context = QOpenGLContext::currentContext();

    if ( context->surface()->surfaceClass() == QSurface::Window )
    {
        auto *window = static_cast< QWindow* >( context->surface() );

        if ( auto* quickWindow = qobject_cast< QQuickWindow *>( window ) )
            ratio = quickWindow->effectiveDevicePixelRatio();
        else
            ratio = window->devicePixelRatio();
    }
    else
    {
        if ( context->screen() )
            ratio = context->screen()->devicePixelRatio();
        else
            ratio = qGuiApp->devicePixelRatio();
    }

    return ratio;
}

#endif

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

        void setTextureId( uint );
        uint textureId() const;

        virtual int compare(const QSGMaterial* ) const override;

    private:
        uint m_textureId;
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

        if ( ( materialOld == nullptr )
            || ( materialOld->textureId() != materialNew->textureId() ) )
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

void QskTextureNode::setTextureId( uint textureId )
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

uint QskTextureNode::textureId() const
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

#if 1
    const qreal ratio = qskDevicePixelRatio();
    const QRect rect( d->rect.x(), d->rect.y(),
        d->rect.width() / ratio, d->rect.height() / ratio );
#endif

    QSGGeometry::updateTexturedRectGeometry( &d->geometry, rect, r );
}
