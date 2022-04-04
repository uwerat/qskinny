/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskCheckBoxSkinlet.h"
#include "QskCheckBox.h"

#include <QSGFlatColorMaterial>
#include <qsgnode.h>

namespace
{
    class Tick : public QSGGeometryNode
    {
      public:
        Tick( const QRectF& rect, const QColor& color )
            : m_target( rect )
            , geometry( QSGGeometry::defaultAttributes_Point2D(), 3 )
        {
            geometry.setDrawingMode( QSGGeometry::DrawLineStrip );
            geometry.setLineWidth( 2 );
            setGeometry( &geometry );

            material.setColor( color );
            setMaterial( &material );

            markDirty( QSGNode::DirtyGeometry );
        }

        void setColor( const QColor& color ) {
            material.setColor( color );
            markDirty( QSGNode::DirtyMaterial );
        }

        void makeTick()
        {
            const auto x = m_target.x();
            const auto y = m_target.y();

            auto vertexData = geometry.vertexDataAsPoint2D();

            vertexData[0].set( x, y + m_target.height() / 2 );
            vertexData[1].set( x + m_target.width() / 3, y + m_target.height() );
            vertexData[2].set( x + m_target.width(), y );

            markDirty( QSGNode::DirtyGeometry );
        }

        void makePartially()
        {
            const auto x = m_target.x();
            const auto y = m_target.y();

            auto vertexData = geometry.vertexDataAsPoint2D();

            vertexData[0].set( x, y + m_target.height() / 2 );
            vertexData[1].set( x, y + m_target.height() / 2 );
            vertexData[2].set( x + m_target.width(), y + m_target.height() / 2 );

            markDirty( QSGNode::DirtyGeometry );
        }

        void makeEmpty()
        {
            const auto x = m_target.x();
            const auto y = m_target.y();

            auto vertexData = geometry.vertexDataAsPoint2D();

            vertexData[0].set( x, y );
            vertexData[1].set( x, y );
            vertexData[2].set( x, y );

            markDirty( QSGNode::DirtyGeometry );
        }

      private:

        const QRectF m_target;

        QSGFlatColorMaterial material;
        QSGGeometry geometry;
    };
}

QskCheckBoxSkinlet::QskCheckBoxSkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { BoxRole, TickRole } );
}

QskCheckBoxSkinlet::~QskCheckBoxSkinlet()
{
}

QRectF QskCheckBoxSkinlet::subControlRect(
    const QskSkinnable*, const QRectF& contentsRect, QskAspect::Subcontrol ) const
{
    return contentsRect;
}

QSGNode* QskCheckBoxSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    switch( nodeRole )
    {
        case BoxRole:
        {
            return updateBoxNode( skinnable, node, QskCheckBox::Box );
        }

        case TickRole:
        {
            auto checkBox = static_cast< const QskCheckBox* >( skinnable );
            return updateTickNode( checkBox, node );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskCheckBoxSkinlet::updateTickNode(
    const QskCheckBox* checkBox, QSGNode* node ) const
{
    using Q = QskCheckBox;

    auto rect = checkBox->subControlRect( Q::Tick );
    rect = rect.marginsRemoved( checkBox->marginHint( Q::Tick ) );

    auto tick = static_cast< Tick* >( node );
    if ( tick == nullptr )
        tick = new Tick( rect, checkBox->color( Q::Tick ) );

    switch ( checkBox->checkState() )
    {
        case Qt::Unchecked:

            tick->setColor( checkBox->color( Q::Tick ) );
            tick->makeEmpty();

            break;

        case Qt::PartiallyChecked:

            tick->setColor( checkBox->color( Q::Tick | Q::PartiallyChecked ) );
            tick->makePartially();

            break;

        case Qt::Checked:

            tick->setColor( checkBox->color( Q::Tick | Q::Checked ) );
            tick->makeTick();

            break;
    }

    return tick;
}

QSizeF QskCheckBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint, const QSizeF& ) const
{
    return skinnable->strutSizeHint( QskCheckBox::Box );
}
