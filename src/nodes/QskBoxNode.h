/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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
class QColor;

class QSK_EXPORT QskBoxNode : public QSGNode
{
  public:
    QskBoxNode();
    ~QskBoxNode() override;

    void updateNode( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&,
        const QskShadowMetrics&, const QColor& shadowColor );
};

#endif
