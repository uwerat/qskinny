/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLineEditSkinlet.h"
#include "QskLineEdit.h"

QskLineEditSkinlet::QskLineEditSkinlet( QskSkin* skin ):
    Inherited( skin )
{
    setNodeRoles( { BackgroundRole, ForegroundRole } );
}

QskLineEditSkinlet::~QskLineEditSkinlet() = default;

QRectF QskLineEditSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{      
    const auto lineEdit = static_cast< const QskLineEdit* >( skinnable );
    
    if ( subControl == QskLineEdit::Panel )
    {
        return panelRect( lineEdit );
    }
    
    return Inherited::subControlRect( skinnable, subControl );
}   

QRectF QskLineEditSkinlet::panelRect( const QskLineEdit* lineEdit ) const
{
    return lineEdit->contentsRect();
}

QSGNode* QskLineEditSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto lineEdit = static_cast< const QskLineEdit* >( skinnable );

    switch( nodeRole )
    {
        case BackgroundRole:
            return updateBackgroundNode( lineEdit, node );

        case ForegroundRole:
            return updateForegroundNode( lineEdit, node );
    }
    return nullptr;
}

QSGNode* QskLineEditSkinlet::updateBackgroundNode(
    const QskLineEdit* lineEdit, QSGNode* node ) const
{
    return updateBoxNode( lineEdit, node, QskLineEdit::Panel );
}

QSGNode* QskLineEditSkinlet::updateForegroundNode(
    const QskLineEdit* lineEdit, QSGNode* node ) const
{
    auto edit = const_cast< QskLineEdit* >( lineEdit );
    return edit->updateTextInputNode( node );
}

#include "moc_QskLineEditSkinlet.cpp"
