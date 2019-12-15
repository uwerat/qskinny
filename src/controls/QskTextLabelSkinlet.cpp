/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextLabelSkinlet.h"
#include "QskTextLabel.h"

#include "QskTextOptions.h"

QskTextLabelSkinlet::QskTextLabelSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, TextRole } );
}

QskTextLabelSkinlet::~QskTextLabelSkinlet() = default;

QRectF QskTextLabelSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto label = static_cast< const QskTextLabel* >( skinnable );

    if ( subControl == QskTextLabel::Panel )
    {
        return contentsRect;
    }
    else if ( subControl == QskTextLabel::Text )
    {
        if ( label->hasPanel() )
        {
            auto r = subControlRect( label, contentsRect, QskTextLabel::Panel );
            return label->innerBox( QskTextLabel::Panel, r );
        }

        return contentsRect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskTextLabelSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto label = static_cast< const QskTextLabel* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            if ( !label->hasPanel() )
                return nullptr;

            return updateBoxNode( label, node, QskTextLabel::Panel );
        }
        case TextRole:
        {
            return updateTextNode( label, node,
                label->text(), label->textOptions(), QskTextLabel::Text );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskTextLabelSkinlet.cpp"
