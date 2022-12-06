/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_RECTANGLE_NODE_H
#define QSK_RECTANGLE_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QskGradient;
class QskRectangleNodePrivate;

class QSK_EXPORT QskRectangleNode : public QSGGeometryNode
{
  public:
    QskRectangleNode();
    ~QskRectangleNode() override;

    void updateNode( const QRectF&, const QskGradient& );

  private:
    Q_DECLARE_PRIVATE( QskRectangleNode )
};

#endif
