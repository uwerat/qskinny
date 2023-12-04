/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcNode.h"
#include "QskArcMetrics.h"
#include "QskMargins.h"
#include "QskGradient.h"
#include "QskShapeNode.h"
#include "QskStrokeNode.h"
#include "QskSGNode.h"

#include <qpen.h>
#include <qpainterpath.h>

#include <QSGSimpleMaterialShader>
#include <QFile>
#include <QDebug>
#include <qmath.h>

namespace
{
    struct QskArcShadowMaterialProperties
    {
        QColor color = Qt::red;
        QRectF rect;
        qreal radius = 1.0; // [0.0,1.0]
        qreal thickness = 0.2;
        qreal startAngle = 0.0; //< degree [0.0,360.0]
        qreal spanAngle = 270.0; //< degree [0.0,360.0]
        qreal extend = 16.0; //< pixel >= 0.0
    };

    class QskArcShadowMaterial final : public QSGSimpleMaterialShader<QskArcShadowMaterialProperties>
    {
        QSG_DECLARE_SIMPLE_SHADER(QskArcShadowMaterial, QskArcShadowMaterialProperties)

        public:

        QskArcShadowMaterial()
        {         
            const QString root( ":/qskinny/shaders/" );            
            setShaderSourceFile( QOpenGLShader::Vertex, root + "arcshadow.vert" );
            setShaderSourceFile( QOpenGLShader::Fragment, root + "arcshadow.frag" );            
        }            

        QList<QByteArray> attributes() const override {
            return QList<QByteArray>() << "vertex";
        }

        void updateState( const QskArcShadowMaterialProperties* newState,
            const QskArcShadowMaterialProperties* oldState ) override
        {
            std::ignore = oldState;

            const auto& color = newState->color;
            const auto& rect = newState->rect;
            const auto& radius = newState->radius;
            const auto& thickness = newState->thickness;
            const auto& startAngle = newState->startAngle;
            const auto& spanAngle = newState->spanAngle;
            const auto& extend = newState->extend;

            auto& p = *program();
            p.setUniformValue("color", color.redF(), color.greenF(), color.blueF(), 1.0f);
            p.setUniformValue("rect", rect.x(), rect.y(), rect.width(), rect.height());
            p.setUniformValue("radius", (float) radius);
            p.setUniformValue("thickness", (float) thickness);
            p.setUniformValue("startAngle", (float) startAngle - 90.0f);
            p.setUniformValue("spanAngle", (float) spanAngle);
            p.setUniformValue("extend", (float) extend);
        }

    };

    class QskArcShadowNode : public QSGGeometryNode
    {
      public:
        QskArcShadowNode() : m_geometry(QSGGeometry::defaultAttributes_Point2D(), 4)
        {
            setGeometry(&m_geometry);
            setMaterial(QskArcShadowMaterial::createMaterial());
            m_geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );            
            material()->setFlag(QSGMaterial::Blending);
        }

        void update(const QRectF& rect, const QskArcMetrics& metrics, const QColor& color, const qreal extend)
        {        
            const auto size = qMin(rect.width(), rect.height());
            auto* const material = static_cast<QSGSimpleMaterial< QskArcShadowMaterialProperties >*>(this->material());
            auto& state = *material->state();
            state.color = color;
            state.rect = rect;
            state.radius = 1.0 - metrics.thickness() / size;
            state.thickness = 2 * metrics.thickness() / size;
            state.startAngle = metrics.startAngle();
            state.spanAngle = metrics.spanAngle();
            state.extend = extend;
            markDirty( QSGNode::DirtyMaterial );
        }

      private:
        QSGGeometry m_geometry;
    };
}


static inline QskGradient qskEffectiveGradient(
    const QskGradient& gradient, const QskArcMetrics& metrics )
{
    if ( !gradient.isMonochrome() )
    {
        if ( gradient.type() == QskGradient::Stops )
        {
            QskGradient g( gradient.stops() );
            g.setConicDirection( 0.5, 0.5, metrics.startAngle(), 360.0 );

            return g;
        }
    }

    return gradient;
}

static inline QskArcMetrics qskEffectiveMetrics(
    const QskArcMetrics& metrics, const QRectF& rect )
{
    if ( metrics.sizeMode() == Qt::RelativeSize )
    {
        const auto rx = 0.5 * rect.width();
        const auto ry = 0.5 * rect.height();

        return metrics.toAbsolute( rx, ry );
    }

    return metrics;
}

static inline QRectF qskEffectiveRect(
    const QRectF& rect, const qreal borderWidth )
{
    if ( borderWidth <= 0.0 )
        return rect;

    return qskValidOrEmptyInnerRect( rect, QskMargins( 0.5 * borderWidth ) );
}

QskArcNode::QskArcNode()
{
}

QskArcNode::~QskArcNode()
{
}

void QskArcNode::setArcData( const QRectF& rect,
    const QskArcMetrics& arcMetrics, const QskGradient& fillGradient )
{
    setArcData( rect, arcMetrics, 0.0, QColor(), fillGradient );
}

void QskArcNode::setArcData( const QRectF& rect, const QskArcMetrics& arcMetrics,
    qreal borderWidth, const QColor& borderColor, const QskGradient& fillGradient )
{
    enum NodeRole
    {
        ShadowRole,
        FillRole,
        BorderRole
    };

    const auto metrics = qskEffectiveMetrics( arcMetrics, rect );
    const auto gradient = qskEffectiveGradient( fillGradient, metrics );

    auto shadowNode = static_cast< QskArcShadowNode* >(
        QskSGNode::findChildNode( this, ShadowRole ) );

    auto fillNode = static_cast< QskShapeNode* >(
        QskSGNode::findChildNode( this, FillRole ) );

    auto borderNode = static_cast< QskStrokeNode* >(
        QskSGNode::findChildNode( this, BorderRole ) );

    const auto arcRect = qskEffectiveRect( rect, borderWidth );
    if ( arcRect.isEmpty() )
    {
        delete shadowNode;
        delete fillNode;
        delete borderNode;
        return;
    }

    const auto path = metrics.painterPath( arcRect );

    if ( true /* TODO */ )
    {
        if ( shadowNode == nullptr ) 
        {
            shadowNode = new QskArcShadowNode;
            QskSGNode::setNodeRole( shadowNode, ShadowRole );            
        }

        const auto extend = 16.0;
        const auto e = extend / 4;
        auto* const vertices = shadowNode->geometry()->vertexDataAsPoint2D();
        const auto shadowRect = arcRect.adjusted( -e, +e, e, -e );
        vertices[0].set(shadowRect.left(), shadowRect.top());
        vertices[1].set(shadowRect.left(), shadowRect.bottom());
        vertices[2].set(shadowRect.right(), shadowRect.top());
        vertices[3].set(shadowRect.right(), shadowRect.bottom());
        shadowNode->update(shadowRect, metrics, Qt::black, extend);
        qDebug() << shadowRect << arcRect;
    }
    else 
    {
        delete shadowNode;
        shadowNode = nullptr;
    }

    if ( gradient.isVisible() && !metrics.isNull() )
    {
        if ( fillNode == nullptr )
        {
            fillNode = new QskShapeNode;
            QskSGNode::setNodeRole( fillNode, FillRole );
        }

        fillNode->updateNode( path, QTransform(), arcRect, gradient );
    }
    else
    {
        delete fillNode;
        fillNode = nullptr;
    }

    if ( borderWidth > 0.0 && borderColor.alpha() > 0 )
    {
        if ( borderNode == nullptr )
        {
            borderNode = new QskStrokeNode;
            QskSGNode::setNodeRole( borderNode, BorderRole );
        }

        QPen pen( borderColor, borderWidth );
        pen.setCapStyle( Qt::FlatCap );
        
        borderNode->updateNode( path, QTransform(), pen );
    }
    else
    {
        delete borderNode;
        borderNode = nullptr;
    }
    
    const auto oldChildCount = this->childCount();    
    const auto newChildCount =
        ( shadowNode ? 1 : 0 ) + ( fillNode ? 1 : 0 ) + ( borderNode ? 1 : 0 );
     
    if ( oldChildCount != newChildCount ) 
    {
        removeAllChildNodes();
     
        for ( QSGNode* node : { ( QSGNode* ) shadowNode, ( QSGNode* ) fillNode, ( QSGNode* ) borderNode })
        {
            if ( node != nullptr ) 
            {
                appendChildNode( node );
            }
        }
    }
}
