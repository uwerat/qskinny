/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_FILL_NODE_H
#define QSK_BOX_FILL_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QskGradient;
class QskBoxShapeMetrics;
class QskBoxBorderMetrics;

class QskBoxFillNodePrivate;

class QSK_EXPORT QskBoxFillNode : public QSGGeometryNode
{
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
