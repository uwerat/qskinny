/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRADIENT_NODE_H
#define QSK_GRADIENT_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QskGradient;
class QskGradientNodePrivate;

class QSK_EXPORT QskGradientNode : public QSGGeometryNode
{
  public:
    QskGradientNode();
    ~QskGradientNode() override;

    void updateNode( const QRectF&, const QskGradient& );

  private:
    Q_DECLARE_PRIVATE( QskGradientNode )
};

#endif
