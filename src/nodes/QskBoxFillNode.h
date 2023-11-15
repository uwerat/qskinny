/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_BOX_FILL_NODE_H
#define QSK_BOX_FILL_NODE_H

#include "QskGlobal.h"
#include "QskFillNode.h"

class QskGradient;
class QskBoxShapeMetrics;
class QskBoxBorderMetrics;

class QskBoxFillNodePrivate;

class QSK_EXPORT QskBoxFillNode : public QskFillNode
{
    using Inherited = QskFillNode;

  public:
    QskBoxFillNode();

    void updateNode( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskGradient& );

    void updateNode( const QRectF&, const QskGradient& );

  private:
    Q_DECLARE_PRIVATE( QskBoxFillNode )
};

#endif
