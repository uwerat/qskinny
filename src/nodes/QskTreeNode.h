/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TREE_NODE_H
#define QSK_TREE_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

/*
   Used as paintNode in all QskControls ( see QskControl::updateItemPaintNode )
 */
class QSK_EXPORT QskTreeNode final : public QSGNode
{
  public:
    QskTreeNode();
    virtual ~QskTreeNode();

    void setSubtreeBlocked( bool on, bool notify = true );
    bool isSubtreeBlocked() const override;

  protected:
    QskTreeNode( QSGNodePrivate& );

  private:
    bool m_isBlocked = false;;
};

QSK_EXPORT QskTreeNode* qskTreeNodeCast( QSGNode* );
QSK_EXPORT const QskTreeNode* qskTreeNodeCast( const QSGNode* );

/*
    Used by all QskQuickItem as root node ( = itemNode ) of its subtree
    ( see qskItemNode in QskQuick.h )
 */
class QSK_EXPORT QskItemNode final : public QSGTransformNode
{
  public:
    QskItemNode();
    virtual ~QskItemNode();

    void setSubtreeBlocked( bool on, bool notify = true );
    bool isSubtreeBlocked() const override;

  private:
    bool m_isBlocked = false;;
};

QSK_EXPORT QskItemNode* qskItemNodeCast( QSGNode* );
QSK_EXPORT const QskItemNode* qskItemNodeCast( const QSGNode* );

QSK_EXPORT bool qskIsBlockableNode( const QSGNode* );
QSK_EXPORT bool qskTryBlockNode( QSGNode*, bool on, bool notify = true );

QSK_EXPORT void qskTryBlockTree( QSGNode*, bool on, bool notify = true );

// un/block a node and all its successors in the rendering pipeline
QSK_EXPORT void qskTryBlockTrailingNodes(
    QSGNode*, const QSGNode* ancestorNode, bool on, bool notify = true );

#endif
