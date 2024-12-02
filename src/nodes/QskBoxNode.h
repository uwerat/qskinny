/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_BOX_NODE_H
#define QSK_BOX_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QskShadowMetrics;
class QskBoxShapeMetrics;
class QskBoxBorderMetrics;
class QskBoxBorderColors;
class QskGradient;
class QskShadowMetrics;
class QQuickWindow;
class QColor;

class QSK_EXPORT QskBoxNode : public QSGNode
{
  public:
    QskBoxNode();
    ~QskBoxNode() override;

    void updateNode( const QQuickWindow*, const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&,
        const QskShadowMetrics&, const QColor& shadowColor );
};

#endif
