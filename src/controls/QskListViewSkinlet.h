/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_LIST_VIEW_SKINLET_H
#define QSK_LIST_VIEW_SKINLET_H

#include "QskScrollViewSkinlet.h"

class QskListView;

class QMarginsF;
class QSizeF;
class QRectF;
class QSGTransformNode;

class QSK_EXPORT QskListViewSkinlet : public QskScrollViewSkinlet
{
    Q_GADGET

    using Inherited = QskScrollViewSkinlet;

  public:
    enum NodeRole : quint8
    {
        TextRole = Inherited::RoleCount,
        GraphicRole,

        RoleCount
    };

    Q_INVOKABLE QskListViewSkinlet( QskSkin* = nullptr );
    ~QskListViewSkinlet() override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

    QRectF sampleRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, int index ) const override;

    QskAspect::States sampleStates( const QskSkinnable*,
        QskAspect::Subcontrol, int index ) const override;

  protected:
    QSGNode* updateContentsNode(
        const QskScrollView*, QSGNode* ) const override;

  private:
    void updateForegroundNodes( const QskListView*, QSGNode* ) const;
    void updateBackgroundNodes( const QskListView*, QSGNode* ) const;

    void updateVisibleForegroundNodes(
        const QskListView*, QSGNode*,
        int rowMin, int rowMax, const QMarginsF& margin ) const;

    QSGTransformNode* updateForegroundNode( const QskListView*,
        QSGNode* parentNode, QSGTransformNode* cellNode,
        int row, int col, const QSizeF& ) const;

    QSGNode* updateCellNode( const QskListView*,
        QSGNode*, const QRectF&, int row, int col ) const;
};

#endif
