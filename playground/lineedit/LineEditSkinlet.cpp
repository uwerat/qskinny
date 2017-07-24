/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "LineEditSkinlet.h"
#include "LineEdit.h"

LineEditSkinlet::LineEditSkinlet( QskSkin* skin ):
    Inherited( skin )
{
    setNodeRoles( { BackgroundRole, ForegroundRole } );
}

LineEditSkinlet::~LineEditSkinlet() = default;

QRectF LineEditSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{      
    const auto lineEdit = static_cast< const LineEdit* >( skinnable );
    
    if ( subControl == LineEdit::Panel )
    {
        return panelRect( lineEdit );
    }
    
    return Inherited::subControlRect( skinnable, subControl );
}   

QRectF LineEditSkinlet::panelRect( const LineEdit* lineEdit ) const
{
    return lineEdit->contentsRect();
}

QSGNode* LineEditSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto lineEdit = static_cast< const LineEdit* >( skinnable );

    switch( nodeRole )
    {
        case BackgroundRole:
            return updateBackgroundNode( lineEdit, node );

        case ForegroundRole:
            return updateForegroundNode( lineEdit, node );
    }
    return nullptr;
}

QSGNode* LineEditSkinlet::updateBackgroundNode(
    const LineEdit* lineEdit, QSGNode* node ) const
{
    return updateBoxNode( lineEdit, node, LineEdit::Panel );
}

QSGNode* LineEditSkinlet::updateForegroundNode(
    const LineEdit* lineEdit, QSGNode* node ) const
{
    auto edit = const_cast< LineEdit* >( lineEdit );
    return edit->updateTextInputNode( node );
}

#include "moc_LineEditSkinlet.cpp"
