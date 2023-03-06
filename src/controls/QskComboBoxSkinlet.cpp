/******************************************************************************
 * QSkinny - Copyright (C) 2023 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskComboBoxSkinlet.h"
#include "QskComboBox.h"

#include "QskGraphic.h"
#include "QskSGNode.h"
#include "QskSubcontrolLayoutEngine.h"

namespace
{
#if 1 // unify with the implementation from QskMenu
    template< class T >
    static inline QVariant qskSampleAt( const QskComboBox* box )
    {
        if( std::is_same< T, QString >() )
        {
            return box->currentText();
        }

        const int index = box->currentIndex();

        if( index < 0 )
            return QVariant::fromValue( T() );

        const auto list = box->optionAt( index );
        for ( const auto& value : list )
        {
            if ( value.canConvert< T >() )
                return value;
        }

        return QVariant();
    }

    template< class T >
    static inline T qskValueAt( const QskComboBox* box )
    {
        const auto sample = qskSampleAt< T >( box );
        return sample.template value< T >();
    }
#endif

    class LayoutEngine : public QskSubcontrolLayoutEngine
    {
      public:
        LayoutEngine( const QskComboBox* box )
            : QskSubcontrolLayoutEngine( Qt::Horizontal )
        {
            setSpacing( box->spacingHint( QskComboBox::Panel ) );

            setGraphicTextElements( box,
                QskComboBox::Text, qskValueAt< QString >( box ),
                QskComboBox::Graphic, qskValueAt< QskGraphic >( box ).defaultSize() );

            const auto alignment = box->alignmentHint(
                QskComboBox::Panel, Qt::AlignLeft );
            setFixedContent( QskComboBox::Text, Qt::Horizontal, alignment );
        }
    };
}

QskComboBoxSkinlet::QskComboBoxSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, GraphicRole, TextRole, PopupIndicatorRole } );
}

QskComboBoxSkinlet::~QskComboBoxSkinlet() = default;

QRectF QskComboBoxSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    using Q = QskComboBox;

    const auto* box = static_cast< const QskComboBox* >( skinnable );

    if ( subControl == Q::Panel )
        return contentsRect;

    if ( subControl == Q::Text || subControl == Q::Graphic )
    {
        const auto r = box->subControlContentsRect( contentsRect, Q::Panel );

        LayoutEngine layoutEngine( box );
        layoutEngine.setGeometries( r );

        return layoutEngine.subControlRect( subControl );
    }

    if( subControl == Q::PopupIndicator )
    {
        auto rect = box->innerBox( Q::Panel, contentsRect );
        const auto size = box->strutSizeHint( Q::PopupIndicator );
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

        case GraphicRole:
            return updateGraphicNode( box, node, box->graphic(), Q::Graphic );

        case TextRole:
            return updateTextNode( box, node );

        case PopupIndicatorRole:
            return updateSymbolNode( box, node, Q::PopupIndicator );
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
    const auto menuGraphicHint = box->strutSizeHint( Q::PopupIndicator );

    size.rwidth() += spacingHint + menuGraphicHint.width();

    size = box->outerBoxSize( Q::Panel, size );
    size = size.expandedTo( box->strutSizeHint( Q::Panel ) );
    size = size.grownBy( skinnable->marginHint( Q::Panel ) );

    return size;
}

#include "moc_QskComboBoxSkinlet.cpp"
