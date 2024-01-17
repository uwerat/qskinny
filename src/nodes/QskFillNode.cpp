/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFillNode.h"
#include "QskGradientMaterial.h"
#include "QskFillNodePrivate.h"
#include "QskSGNode.h"

#include <qsgflatcolormaterial.h>
#include <qsgvertexcolormaterial.h>
#include <qglobalstatic.h>

Q_GLOBAL_STATIC( QSGVertexColorMaterial, qskMaterialColorVertex )

static inline QskGradient::Type qskGradientType( QskFillNode::Coloring coloring )
{
    const auto type = QskGradient::Linear + coloring - QskFillNode::Linear;
    return static_cast< QskGradient::Type >( type );
}

static inline QskFillNode::Coloring qskColoring( QskGradient::Type type )
{
    const auto coloring = QskFillNode::Linear + type - QskGradient::Linear;
    return static_cast< QskFillNode::Coloring >( coloring );
}

QskFillNode::QskFillNode()
    : QskFillNode( *new QskFillNodePrivate )
{
}

QskFillNode::QskFillNode( QskFillNodePrivate& dd )
    : QSGGeometryNode( dd )
{
    setGeometry( &dd.geometry );

    setMaterial( qskMaterialColorVertex );
    setFlag( QSGNode::OwnsMaterial, false );
}

QskFillNode::~QskFillNode()
{
}

void QskFillNode::resetGeometry()
{
    QskSGNode::resetGeometry( this );
}

void QskFillNode::setColoring( Coloring coloring )
{
    Q_D( QskFillNode );

    if ( coloring == d->coloring )
        return;

    d->coloring = coloring;

    switch( coloring )
    {
        case Monochrome:
        {
            setMaterial( new QSGFlatColorMaterial() );
            break;
        }

        case Polychrome:
        {
            setMaterial( qskMaterialColorVertex );
            break;
        }

        /*
            QSGDefaultInternalRectangleNode is using QSGSmoothColorMaterial.
            Can we do something useful with it too ? TODO ...
         */

        default:
        {
            const auto gradientType = qskGradientType( coloring );
            setMaterial( QskGradientMaterial::createMaterial( gradientType ) );
        }
    }

    if ( material() == qskMaterialColorVertex )
    {
        /*
            All color information is stored in the geometry and we can share
            the material regardless of the specific colors.

            As nodes with the same material can be batched it might make sense
            to use this type of coloring for monochrome fillings: memory vs. performance.
         */
        setFlag( QSGNode::OwnsMaterial, false ); // shared: do not delete

        if ( !isGeometryColored() )
        {
            const QSGGeometry g( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 );
            memcpy( ( void* ) &d->geometry, ( void* ) &g, sizeof( QSGGeometry ) );
        }
    }
    else
    {
        setFlag( QSGNode::OwnsMaterial, true );

        if ( isGeometryColored() )
        {
            const QSGGeometry g( QSGGeometry::defaultAttributes_Point2D(), 0 );
            memcpy( ( void* ) &d->geometry, ( void* ) &g, sizeof( QSGGeometry ) );
        }
    }
}

QskFillNode::Coloring QskFillNode::coloring() const
{
    return d_func()->coloring;
}

void QskFillNode::setColoring( const QColor& color )
{
    setColoring( Monochrome );

    auto mat = static_cast< QSGFlatColorMaterial* >( material() );
    if ( mat->color() != color )
    {
        mat->setColor( color );
        markDirty( QSGNode::DirtyMaterial );
    }
}

void QskFillNode::setColoring( const QRectF& rect, const QskGradient& gradient )
{
    if ( gradient.isMonochrome() )
    {
        setColoring( gradient.startColor().toRgb() );
    }
    else
    {
        const auto effectiveGradient = gradient.effectiveGradient();
        setColoring( qskColoring( effectiveGradient.type() ) );

        auto mat = static_cast< QskGradientMaterial* >( material() );
        if ( mat->updateGradient( rect, effectiveGradient ) )
            markDirty( QSGNode::DirtyMaterial );
    }
}

bool QskFillNode::isGeometryColored() const
{
    return d_func()->geometry.attributeCount() != 1;
}
