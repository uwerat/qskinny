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
        Tick()
            : geometry( QSGGeometry::defaultAttributes_Point2D(), 3 )
        {
            geometry.setDrawingMode( QSGGeometry::DrawLineStrip );
            geometry.setLineWidth( 2 );
            setGeometry( &geometry );

            setMaterial( &material );
        }

        void setColor( const QColor& color )
        {
            material.setColor( color );
            markDirty( QSGNode::DirtyMaterial );
        }

        void makeTick( const QRectF& rect )
        {
            const auto x = rect.x();
            const auto y = rect.y();

            auto vertexData = geometry.vertexDataAsPoint2D();

            vertexData[0].set( x, y + rect.height() / 2 );
            vertexData[1].set( x + rect.width() / 3, y + rect.height() );
            vertexData[2].set( x + rect.width(), y );

            markDirty( QSGNode::DirtyGeometry );
        }

        void makePartially( const QRectF& rect )
        {
            const auto x = rect.x();
            const auto y = rect.y();

            auto vertexData = geometry.vertexDataAsPoint2D();

            vertexData[0].set( x, y + rect.height() / 2 );
            vertexData[1].set( x, y + rect.height() / 2 );
            vertexData[2].set( x + rect.width(), y + rect.height() / 2 );

            markDirty( QSGNode::DirtyGeometry );
        }

      private:
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

    if ( checkBox->checkState() == Qt::Unchecked )
        return nullptr;

    auto rect = checkBox->subControlRect( Q::Tick );
    rect = rect.marginsRemoved( checkBox->marginHint( Q::Tick ) );

    auto tick = static_cast< Tick* >( node );
    if ( tick == nullptr )
        tick = new Tick();

    if ( checkBox->checkState() == Qt::PartiallyChecked )
    {
        tick->setColor( checkBox->color( Q::Tick | Q::PartiallyChecked ) );
        tick->makePartially( rect );
    }
    else
    {
        tick->setColor( checkBox->color( Q::Tick | Q::Checked ) );
        tick->makeTick( rect );
    }

    return tick;
}

QSizeF QskCheckBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint, const QSizeF& ) const
{
    return skinnable->strutSizeHint( QskCheckBox::Box );
}
