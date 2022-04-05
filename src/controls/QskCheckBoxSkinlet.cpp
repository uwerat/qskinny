/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskCheckBoxSkinlet.h"
#include "QskCheckBox.h"
#include "QskSGNode.h"

#include <QSGFlatColorMaterial>
#include <qsgnode.h>

namespace
{
    class IndicatorNode : public QSGGeometryNode
    {
      public:
        IndicatorNode()
            : m_geometry( QSGGeometry::defaultAttributes_Point2D(), 3 )
        {
            m_geometry.setDrawingMode( QSGGeometry::DrawLineStrip );
            m_geometry.setLineWidth( 2 );
            setGeometry( &m_geometry );

            setMaterial( &m_material );
        }

        void update( bool isPartially, const QRectF& rect, const QColor& color )
        {
            if ( color != m_material.color() )
            {
                m_material.setColor( color );
                markDirty( QSGNode::DirtyMaterial );
            }

            if ( rect != m_rect || isPartially != m_isPartially )
            {
                m_rect = rect;
                m_isPartially = isPartially;

                const auto x = rect.x();
                const auto y = rect.y();
                const auto w = rect.width();
                const auto h = rect.height();

                auto points = m_geometry.vertexDataAsPoint2D();

                if ( isPartially )
                {
                    points[0].set( x, y + h / 2 );
                    points[1] = points[0];
                    points[2].set( x + w, y + h / 2 );
                }
                else
                {
                    points[0].set( x, y + h / 2 );
                    points[1].set( x + w / 3, y + h );
                    points[2].set( x + w, y );
                }
                
                markDirty( QSGNode::DirtyGeometry );
            }
        }

      private:
        QSGFlatColorMaterial m_material;
        QSGGeometry m_geometry;

        QRectF m_rect;
        bool m_isPartially;
    };
}

QskCheckBoxSkinlet::QskCheckBoxSkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { PanelRole, IndicatorRole } );
}

QskCheckBoxSkinlet::~QskCheckBoxSkinlet()
{
}

QRectF QskCheckBoxSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskCheckBox::Indicator )
        return skinnable->innerBox( QskCheckBox::Panel, contentsRect );

    return contentsRect;
}

QSGNode* QskCheckBoxSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    auto checkBox = static_cast< const QskCheckBox* >( skinnable );

    switch( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( skinnable, node, QskCheckBox::Panel );

        case IndicatorRole:
            return updateIndicatorNode( checkBox, node );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskCheckBoxSkinlet::updateIndicatorNode(
    const QskCheckBox* checkBox, QSGNode* node ) const
{
    using Q = QskCheckBox;

    const auto state = checkBox->checkState();
    if ( state == Qt::Unchecked )
        return nullptr;

    const auto rect = checkBox->subControlRect( Q::Indicator );
    if ( rect.isEmpty() )
        return nullptr;

    auto indicatorNode = QskSGNode::ensureNode< IndicatorNode >( node );
    indicatorNode->update( state != Qt::Checked, rect, checkBox->color( Q::Indicator ) );

    return indicatorNode;
}

QSizeF QskCheckBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint, const QSizeF& ) const
{
    return skinnable->strutSizeHint( QskCheckBox::Panel );
}
