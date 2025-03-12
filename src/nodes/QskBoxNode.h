/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_BOX_NODE_H
#define QSK_BOX_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QskBoxHints;
class QQuickWindow;

class QSK_EXPORT QskBoxNode : public QSGNode
{
  public:
    QskBoxNode();
    ~QskBoxNode() override;

    void updateNode( const QQuickWindow*, const QRectF&, const QskBoxHints& );
};

#endif
