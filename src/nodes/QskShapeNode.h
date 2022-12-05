/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SHAPE_NODE_H
#define QSK_SHAPE_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QskGradient;
class QColor;
class QPainterPath;

class QskShapeNodePrivate;

class QSK_EXPORT QskShapeNode : public QSGGeometryNode
{
  public:
    QskShapeNode();

    void updateNode( const QPainterPath&, const QTransform&,
        const QRectF&, const QskGradient& );

  private:
    Q_DECLARE_PRIVATE( QskShapeNode )
};

#endif
