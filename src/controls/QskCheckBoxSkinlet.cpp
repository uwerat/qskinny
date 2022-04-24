/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskCheckBoxSkinlet.h"
#include "QskCheckBox.h"
#include "QskSGNode.h"
#include "QskTextOptions.h"

#include <qfontmetrics.h>
#include <QSGFlatColorMaterial>
#include <qsgnode.h>

#include <algorithm>

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
    setNodeRoles( { PanelRole, TextRole, IndicatorBoxRole, IndicatorRole } );
}

QskCheckBoxSkinlet::~QskCheckBoxSkinlet()
{
}

QRectF QskCheckBoxSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto checkBox = static_cast< const QskCheckBox* >( skinnable );

    if ( subControl == QskCheckBox::Panel)
    {
        return contentsRect;
    }
    else if ( subControl == QskCheckBox::Text)
    {
        return textRect(checkBox, contentsRect );
    }
    else if ( subControl == QskCheckBox::IndicatorTic )
    {
        return indicatorRect(checkBox, contentsRect );
    }
    else if ( subControl == QskCheckBox::IndicatorBox )
    {
        return indicatorBoxRect(checkBox, contentsRect );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskCheckBoxSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    auto checkBox = static_cast< const QskCheckBox* >( skinnable );

    switch( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( skinnable, node, QskCheckBox::Panel );

        case TextRole:
            return updateTextNode( checkBox, node );

        case IndicatorBoxRole:
            return updateBoxNode( skinnable, node, QskCheckBox::IndicatorBox );

        case IndicatorRole:
            return updateIndicatorNode( checkBox, node );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskCheckBoxSkinlet::textRect( const QskCheckBox* checkBox,
    const QRectF& contentsRect ) const {
    using Q = QskCheckBox;

    QSizeF size;
    const QFontMetricsF fm( checkBox->effectiveFont(Q::Text) );
    if ( !checkBox->text().isEmpty() )
    {
        size += fm.size( Qt::TextShowMnemonic, checkBox->text() );
    }

    auto result = contentsRect;

    result.setTopLeft( QPointF(result.right() - size.width() -
        checkBox->marginHint( Q::Text ).right(),
        result.y() + ( result.height() - size.height() ) / 2) );

    return result;
}

QRectF QskCheckBoxSkinlet::indicatorRect( const QskCheckBox* checkBox,
    const QRectF& contentsRect) const {
    using Q = QskCheckBox;

    auto rect = contentsRect;
    auto size =
        checkBox->strutSizeHint( Q::IndicatorTic ).grownBy(checkBox->marginHint( Q::IndicatorTic ) );
    rect.setSize(size);

    return rect;
}

QRectF QskCheckBoxSkinlet::indicatorBoxRect( const QskCheckBox* checkBox,
    const QRectF& contentsRect ) const {
    using Q = QskCheckBox;

    auto rect = contentsRect;
    auto size =
        checkBox->strutSizeHint( Q::IndicatorBox ).grownBy(checkBox->marginHint(Q::IndicatorBox) );
    rect.setSize(size);

    return rect;
}

QSGNode* QskCheckBoxSkinlet::updateTextNode(
    const QskCheckBox* checkBox, QSGNode* node ) const
{
    using Q = QskCheckBox;
    const auto rect = checkBox->subControlRect( Q::Text );

    const auto textHeight = checkBox->effectiveFontHeight( Q::Text );
    if ( !checkBox->clip() && ( rect.height() < textHeight ) )
        return nullptr;

    return QskSkinlet::updateTextNode( checkBox, node,
        checkBox->text(), checkBox->textOptions(), Q::Text );
}

QSGNode* QskCheckBoxSkinlet::updateIndicatorNode(
    const QskCheckBox* checkBox, QSGNode* node ) const
{
    using Q = QskCheckBox;

    if ( checkBox->isChecked() == false )
        return nullptr;

    auto rect = checkBox->subControlRect(Q::IndicatorTic)
        .marginsRemoved(checkBox->marginHint(Q::IndicatorTic) );

    if ( rect.isEmpty() )
        return nullptr;

    auto indicatorNode = QskSGNode::ensureNode< IndicatorNode >( node );
    indicatorNode->update( rect, checkBox->color( Q::IndicatorTic ) );

    return indicatorNode;
}

QSizeF QskCheckBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint, const QSizeF& ) const
{
    using Q = QskCheckBox;
    const auto checkBox = static_cast< const QskCheckBox* >( skinnable );


    const QFontMetricsF fm( checkBox->effectiveFont(Q::Text) );
    QSizeF sizeText;
    if ( !checkBox->text().isEmpty() )
    {
        sizeText += fm.size( Qt::TextShowMnemonic, checkBox->text() )
            .grownBy(checkBox->marginHint(Q::Text) );
    }

    auto sizeIndicator =
        checkBox->strutSizeHint(Q::IndicatorTic).grownBy(checkBox->marginHint(Q::IndicatorTic) );
    auto sizeIndicatorBox =
        checkBox->strutSizeHint(Q::IndicatorBox).grownBy(checkBox->marginHint(Q::IndicatorBox) );

    QSizeF size;
    size.setWidth( sizeText.width() +
        qMax( sizeIndicator.width(), sizeIndicatorBox.width() ) );

    size.setHeight(std::max({ sizeIndicator.height(),
                              sizeIndicatorBox.height(),
                              sizeText.height() }) );
    return size;
}

#include "moc_QskCheckBoxSkinlet.cpp"
