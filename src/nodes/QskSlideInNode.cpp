/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSlideInNode.h"
#include "QskSGNode.h"
#include <qtransform.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
QSK_QT_PRIVATE_END

class QskSlideInNodePrivate final : public QSGNodePrivate
{
  public:
    ~QskSlideInNodePrivate()
    {
        delete clipNode;
        delete transformNode;
        delete contentsNode;
    }

    void reparentContentNode( QskSlideInNode* node )
    {
        if ( contentsNode )
        {
            QSGNode* parentNode = transformNode;

            if ( parentNode == nullptr )
                parentNode = clipNode;

            if ( parentNode == nullptr )
                parentNode = node;

            QskSGNode::setParentNode( contentsNode, parentNode );
        }
    }

    QSGClipNode* clipNode = nullptr;
    QSGTransformNode* transformNode = nullptr;
    QSGNode* contentsNode = nullptr;
};

QskSlideInNode::QskSlideInNode()
    : QSGNode( *new QskSlideInNodePrivate, QSGNode::BasicNodeType )
{
}

QskSlideInNode::~QskSlideInNode() = default;

void QskSlideInNode::updateTranslation( const QRectF& rect,
    Qt::Edge edge, qreal progress )
{
    Q_UNUSED( edge ); // TODO ...

    Q_D( QskSlideInNode );

    {
        // clipping

        if ( progress >= 0.0 && progress < 1.0 )
        {
            if ( d->clipNode == nullptr )
            {
                d->clipNode = new QSGClipNode();
                d->clipNode->setFlag( QSGNode::OwnedByParent, false );
                d->clipNode->setIsRectangular( true );
            }

            d->clipNode->setClipRect( rect );

        }
        else
        {
            delete d->clipNode;
            d->clipNode = nullptr;
        }

        if ( d->clipNode )
            QskSGNode::setParentNode( d->clipNode, this );
    }

    {
        // translation

        qreal dx = 0.0;
        qreal dy = ( progress - 1.0 ) * rect.height();

        if ( dx != 0.0 || dy != 0.0 )
        {
            if ( d->transformNode == nullptr )
            {
                d->transformNode = new QSGTransformNode();
                d->transformNode->setFlag( QSGNode::OwnedByParent, false );
            }

            QTransform transform;
            transform.translate( dx, dy );

            d->transformNode->setMatrix( transform );
        }
        else
        {
            delete d->transformNode;
            d->transformNode = nullptr;
        }

        if ( d->transformNode )
        {
            QSGNode* parentNode = d->clipNode;
            if ( parentNode == nullptr )
                parentNode = this;

            QskSGNode::setParentNode( d->transformNode, parentNode );
        }
    }

    d->reparentContentNode( this );
}

void QskSlideInNode::setContentsNode( QSGNode* node )
{
    Q_D( QskSlideInNode );

    if ( d->contentsNode == node )
        return;

    if ( node )
        node->setFlag( QSGNode::OwnedByParent, false );

    delete d->contentsNode;
    d->contentsNode = node;

    d->reparentContentNode( this );
}

QSGNode* QskSlideInNode::contentsNode()
{
    return d_func()->contentsNode;
}

const QSGNode* QskSlideInNode::contentsNode() const
{
    return d_func()->contentsNode;
}
