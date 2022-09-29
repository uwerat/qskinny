/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_STROKE_NODE_H
#define QSK_STROKE_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QPen;
class QPainterPath;

class QskStrokeNodePrivate;

class QSK_EXPORT QskStrokeNode : public QSGGeometryNode
{
  public:
    QskStrokeNode();

    void updateNode( const QPainterPath&, const QTransform&, const QPen& );

  private:
    Q_DECLARE_PRIVATE( QskStrokeNode )
};

#endif
