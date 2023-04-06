/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskComboBoxSkinlet.h"
#include "QskComboBox.h"

#include "QskGraphic.h"
#include "QskLabelData.h"

#include "QskSGNode.h"
#include "QskSubcontrolLayoutEngine.h"

namespace
{
    class LayoutEngine : public QskSubcontrolLayoutEngine
    {
      public:
        LayoutEngine( const QskComboBox* box )
            : QskSubcontrolLayoutEngine( Qt::Horizontal )
        {
            setSpacing( box->spacingHint( QskComboBox::Panel ) );

            QSizeF graphicSize;
            QString text;

            if ( box->currentIndex() >= 0 )
            {
                const auto option = box->optionAt( box->currentIndex() );

                graphicSize = option.icon().graphic().defaultSize();
                text = option.text();
            }
            else
            {
                text = box->placeholderText();
            }

            setGraphicTextElements( box,
                QskComboBox::Text, text, QskComboBox::Icon, graphicSize );

            const auto alignment = box->alignmentHint(
                QskComboBox::Panel, Qt::AlignLeft );

            setFixedContent( QskComboBox::Text, Qt::Horizontal, alignment );
        }
    };
}

QskComboBoxSkinlet::QskComboBoxSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, IconRole, TextRole, StatusIndicatorRole } );
}

QskComboBoxSkinlet::~QskComboBoxSkinlet() = default;

QRectF QskComboBoxSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    using Q = QskComboBox;

    const auto* box = static_cast< const QskComboBox* >( skinnable );

    if ( subControl == Q::Panel )
        return contentsRect;

    if ( subControl == Q::Text || subControl == Q::Icon )
    {
        const auto r = box->subControlContentsRect( contentsRect, Q::Panel );

        LayoutEngine layoutEngine( box );
        layoutEngine.setGeometries( r );

        return layoutEngine.subControlRect( subControl );
    }

    if( subControl == Q::StatusIndicator )
    {
        auto rect = box->innerBox( Q::Panel, contentsRect );
        const auto size = box->strutSizeHint( Q::StatusIndicator );
        rect.setLeft( rect.right() - size.width() );
        return rect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskComboBoxSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    using Q = QskComboBox;

    const auto box = static_cast< const QskComboBox* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( box, node, Q::Panel );

        case IconRole:
        {
            const auto option = box->optionAt( box->currentIndex() );
            return updateGraphicNode( box, node,
                option.icon().graphic(), Q::Icon );
        }

        case TextRole:
            return updateTextNode( box, node );

        case StatusIndicatorRole:
            return updateSymbolNode( box, node, Q::StatusIndicator );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskComboBoxSkinlet::updateTextNode(
    const QskComboBox* box, QSGNode* node ) const
{
    using Q = QskComboBox;

    const auto rect = box->subControlRect( Q::Text ).toAlignedRect();

    const auto textHeight = box->effectiveFontHeight( Q::Text );
    if ( !box->clip() && ( rect.height() < textHeight ) )
        return nullptr;

    const auto alignment = box->alignmentHint( Q::Text, Qt::AlignLeft | Qt::AlignVCenter );

    return QskSkinlet::updateTextNode( box, node, rect,
        alignment, box->currentText(), Q::Text );
}

QSizeF QskComboBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    using Q = QskComboBox;

    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto box = static_cast< const QskComboBox* >( skinnable );

    LayoutEngine layoutEngine( box );
    auto size = layoutEngine.sizeHint( which, QSizeF() );

    const auto spacingHint = box->spacingHint( Q::Panel );
    const auto menuGraphicHint = box->strutSizeHint( Q::StatusIndicator );

    size.rwidth() += spacingHint + menuGraphicHint.width();

    size = box->outerBoxSize( Q::Panel, size );
    size = size.expandedTo( box->strutSizeHint( Q::Panel ) );
    size = size.grownBy( skinnable->marginHint( Q::Panel ) );

    return size;
}

#include "moc_QskComboBoxSkinlet.cpp"
