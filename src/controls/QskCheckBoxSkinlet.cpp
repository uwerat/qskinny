/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskCheckBoxSkinlet.h"
#include "QskCheckBox.h"
#include "QskSGNode.h"
#include "QskTextOptions.h"
#include "QskFunctions.h"

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

        void update( const QRectF& rect, const QColor& color )
        {
            if ( color != m_material.color() )
            {
                m_material.setColor( color );
                markDirty( QSGNode::DirtyMaterial );
            }

            if ( rect != m_rect )
            {
                m_rect = rect;

                const auto x = rect.x();
                const auto y = rect.y();
                const auto w = rect.width();
                const auto h = rect.height();

                auto points = m_geometry.vertexDataAsPoint2D();

                points[0].set( x, y + h / 2 );
                points[1].set( x + w / 3, y + h );
                points[2].set( x + w, y );

                markDirty( QSGNode::DirtyGeometry );
            }
        }

      private:
        QSGFlatColorMaterial m_material;
        QSGGeometry m_geometry;

        QRectF m_rect;
    };
}

QskCheckBoxSkinlet::QskCheckBoxSkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { BoxRole, IndicatorRole, TextRole } );
}

QskCheckBoxSkinlet::~QskCheckBoxSkinlet()
{
}

QRectF QskCheckBoxSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto checkBox = static_cast< const QskCheckBox* >( skinnable );

    if ( subControl == QskCheckBox::Panel )
    {
        return contentsRect;
    }
    else if ( subControl == QskCheckBox::Box )
    {
        return boxRect( checkBox, contentsRect );
    }
    else if ( subControl == QskCheckBox::Indicator )
    {
        const auto boxRect = subControlRect( skinnable, contentsRect, QskCheckBox::Box );
        return skinnable->innerBox( QskCheckBox::Box, boxRect );

        return skinnable->innerBox( QskCheckBox::Box, contentsRect );
    }
    else if ( subControl == QskCheckBox::Text )
    {
        return textRect( checkBox, contentsRect );
    }

    return contentsRect;
}

QRectF QskCheckBoxSkinlet::textRect(
    const QskCheckBox* checkBox, const QRectF& contentsRect ) const
{
    const auto boxRect = subControlRect( checkBox, contentsRect, QskCheckBox::Box );
    const qreal spacing = checkBox->spacingHint( QskCheckBox::Panel );

    auto r = subControlRect( checkBox, contentsRect, QskCheckBox::Panel );
    r = checkBox->innerBox( QskCheckBox::Panel, r );

    if ( checkBox->layoutMirroring() )
        r.setRight( boxRect.left() - spacing );
    else
        r.setLeft( boxRect.right() + spacing  );

    return r;
}

QRectF QskCheckBoxSkinlet::boxRect(
    const QskCheckBox* checkBox, const QRectF& contentsRect ) const
{
    const auto size = checkBox->strutSizeHint( QskCheckBox::Box );

    auto r = checkBox->innerBox( QskCheckBox::Panel, contentsRect );

    if ( checkBox->layoutMirroring() )
        r.setLeft( r.right() - size.width() );
    else
        r.setWidth( size.width() );

    r.setTop( r.top() + 0.5 * ( r.height() - size.height() ) );
    r.setHeight( size.height() );

    return r;
}

QSGNode* QskCheckBoxSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    auto checkBox = static_cast< const QskCheckBox* >( skinnable );

    switch( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( skinnable, node, QskCheckBox::Panel );

        case BoxRole:
            return updateBoxNode( skinnable, node, QskCheckBox::Box );

        case IndicatorRole:
            return updateIndicatorNode( checkBox, node );

        case TextRole:
            return updateTextNode( checkBox, node );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskCheckBoxSkinlet::updateIndicatorNode(
    const QskCheckBox* checkBox, QSGNode* node ) const
{
    using Q = QskCheckBox;

    if ( !checkBox->isChecked() )
        return nullptr;

    const auto rect = checkBox->subControlRect( Q::Indicator );
    if ( rect.isEmpty() )
        return nullptr;

    auto indicatorNode = QskSGNode::ensureNode< IndicatorNode >( node );
    indicatorNode->update( rect, checkBox->color( Q::Indicator ) );

    return indicatorNode;
}

QSGNode* QskCheckBoxSkinlet::updateTextNode(
    const QskCheckBox* checkBox, QSGNode* node ) const
{
    using Q = QskCheckBox;

    const auto rect = checkBox->subControlRect( Q::Text );
    const auto alignH = checkBox->layoutMirroring() ? Qt::AlignRight : Qt::AlignLeft;

    QskTextOptions textOptions;
    textOptions.setElideMode( Qt::ElideMiddle );

    return QskSkinlet::updateTextNode( checkBox, node, rect, alignH | Qt::AlignVCenter,
        checkBox->text(), textOptions, QskCheckBox::Text );
}

QSizeF QskCheckBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    using Q = QskCheckBox;

    if ( which == Qt::MaximumSize )
        return QSizeF();

    auto checkBox = static_cast< const QskCheckBox* >( skinnable );

    auto size = skinnable->strutSizeHint( QskCheckBox::Box );
    size = skinnable->outerBoxSize( Q::Panel, size );

    auto text = checkBox->text();
    if ( !text.isEmpty() )
    {
        qreal extra = skinnable->spacingHint( Q::Panel );

        if ( which == Qt::MinimumSize )
            text = 'W';

        const auto font = skinnable->effectiveFont( Q::Text );
        extra += qskHorizontalAdvance( font, text );

        size.setWidth( size.width() + extra );
    }

    return size.expandedTo( skinnable->strutSizeHint( Q::Panel ) );
}

#include "moc_QskCheckBoxSkinlet.cpp"
