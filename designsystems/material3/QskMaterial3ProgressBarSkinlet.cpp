/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMaterial3ProgressBarSkinlet.h"
#include <QskProgressBar.h>
#include <QskBoxHints.h>
#include <QskMargins.h>
#include <QskClipNode.h>
#include <QskSGNode.h>

using Q = QskProgressBar;

QskMaterial3ProgressBarSkinlet::QskMaterial3ProgressBarSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    appendNodeRoles( { StopIndicatorRole } );
}

QSGNode* QskMaterial3ProgressBarSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    auto progressBar = static_cast< const QskProgressBar* >( skinnable );

    switch( nodeRole )
    {
        case GrooveRole:
        {
            auto clippedNode = QskSGNode::findChildNode( node, GrooveRole );
            clippedNode = Inherited::updateSubNode( skinnable, nodeRole, clippedNode );

           if ( clippedNode == nullptr )
                return nullptr;

            auto clipNode = updateGrooveClipNode( progressBar, node );
            QskSGNode::setNodeRole( clippedNode, nodeRole );
            QskSGNode::setParentNode( clippedNode, clipNode );

            return clipNode;
        }
        case StopIndicatorRole:
        {
            if ( !( progressBar->isIndeterminate() || progressBar->hasOrigin() ) )
                return updateStopIndicatorNode( progressBar, node );

            return nullptr;
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskMaterial3ProgressBarSkinlet::updateStopIndicatorNode(
    const QskProgressBar* progressBar, QSGNode* node ) const
{
    auto rect = progressBar->subControlRect( Q::Groove );
    if ( rect.isEmpty() )
        return nullptr;
        
    if( progressBar->orientation() == Qt::Horizontal )
        rect.setLeft( rect.right() - rect.height() );
    else         
        rect.setBottom( rect.top() + rect.width() );

    QskBoxHints hints;
    hints.shape = progressBar->boxShapeHint( Q::Fill );
    hints.gradient = progressBar->gradientHint( Q::Fill ).endColor();

    return updateBoxNode( progressBar, node, rect, hints );
}

QSGNode* QskMaterial3ProgressBarSkinlet::updateGrooveClipNode(
    const QskProgressBar* progressBar, QSGNode* node ) const
{
    auto rect = progressBar->subControlRect( Q::Fill );
    if ( rect.isEmpty() )
        return nullptr;

    QskMargins margins;
    if ( progressBar->orientation() == Qt::Horizontal )
        margins.setMargins( rect.height(), 0.0 );
    else
        margins.setMargins( 0.0, rect.width() );

    rect = rect.marginsAdded( margins );
    
    auto clipNode = QskSGNode::ensureNode< QskClipNode >( node );
    clipNode->setRegion( progressBar->subControlRect( Q::Groove ), rect );

    return clipNode;
}

#include "moc_QskMaterial3ProgressBarSkinlet.cpp"
