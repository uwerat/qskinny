/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPopupSkinlet.h"
#include "QskPopup.h"
#include "QskSGNode.h"

#include <qtransform.h>
#include <qsgnode.h>
#include <qquickwindow.h>

namespace
{
    class RootNode : public QSGNode
    {
      public:
        ~RootNode()
        {
            delete m_clipNode;
            delete m_transformNode;
            delete m_contentsNode;
        }

        void setClipRect( const QRectF& rect )
        {
            if ( m_clipNode == nullptr )
            {
                m_clipNode = new QSGClipNode();
                m_clipNode->setFlag( QSGNode::OwnedByParent, false );
                m_clipNode->setIsRectangular( true );
            }

            m_clipNode->setClipRect( rect );
        }

        void resetClip()
        {
            delete m_clipNode;
            m_clipNode = nullptr;
        }

        void setTranslation( qreal dx, qreal dy )
        {
            if ( dx != 0.0 || dy != 0.0 )
            {
                if ( m_transformNode == nullptr )
                {
                    m_transformNode = new QSGTransformNode();
                    m_transformNode->setFlag( QSGNode::OwnedByParent, false );
                }

                QTransform transform;
                transform.translate( dx, dy );

                m_transformNode->setMatrix( transform );
            }
            else
            {
                delete m_transformNode;
                m_transformNode = nullptr;
            }
        }

        void setContentsNode( QSGNode* contentsNode )
        {
            if ( m_contentsNode != contentsNode )
            {
                if ( contentsNode )
                    contentsNode->setFlag( QSGNode::OwnedByParent, false );

                delete m_contentsNode;
                m_contentsNode = contentsNode;
            }
        }

        void rearrangeNodes()
        {
            const std::initializer_list< QSGNode* > nodes =
                { m_clipNode, m_transformNode, m_contentsNode };

            QSGNode* parentNode = this;
            for ( auto node : nodes )
            {
                if ( node )
                {
                    QskSGNode::setParentNode( node, parentNode );
                    parentNode = node;
                }
            }
        }

        inline QSGNode* contentsNode()
        {
            return m_contentsNode;
        }

      private:
        QSGClipNode* m_clipNode = nullptr;
        QSGTransformNode* m_transformNode = nullptr;
        QSGNode* m_contentsNode = nullptr;
    };
}

QskPopupSkinlet::QskPopupSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    appendNodeRoles( { OverlayRole, ContentsRole } );
}

QskPopupSkinlet::~QskPopupSkinlet() = default;

QRectF QskPopupSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto popup = static_cast< const QskPopup* >( skinnable );

    if ( subControl == QskPopup::Overlay )
        return popup->overlayRect();

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskPopupSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto popup = static_cast< const QskPopup* >( skinnable );

    switch ( nodeRole )
    {
        case OverlayRole:
            return updateBoxNode( skinnable, node, QskPopup::Overlay );

        case ContentsRole:
            return updateExtraNode( popup, node );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskPopupSkinlet::updateExtraNode( const QskPopup* popup, QSGNode* node ) const
{
    auto cr = popup->contentsRect();
    if ( cr.isEmpty() )
        return nullptr;

    auto rootNode = QskSGNode::ensureNode< RootNode >( node );

    const auto faderProgress = popup->metric( popup->faderAspect() );
    if ( faderProgress > 0.0 && faderProgress <= 1.0 )
    {
        auto clipRect = QRectF( popup->mapFromScene( QPointF() ), popup->window()->size() );
        clipRect.setTop( cr.top() );

        rootNode->setClipRect( clipRect );
    }
    else
    {
        rootNode->resetClip();
    }

    rootNode->setTranslation( 0.0, -faderProgress * cr.height() );

    auto contentsNode = updateContentsNode( popup, rootNode->contentsNode() );
    rootNode->setContentsNode( contentsNode );

    rootNode->rearrangeNodes();

    return rootNode;
}

QSGNode* QskPopupSkinlet::updateContentsNode( const QskPopup*, QSGNode* ) const
{
    return nullptr;
}

#include "moc_QskPopupSkinlet.cpp"
