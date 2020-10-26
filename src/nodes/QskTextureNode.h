/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TEXTURE_NODE_H
#define QSK_TEXTURE_NODE_H

#include "QskGlobal.h"

#include <qnamespace.h>
#include <qsgnode.h>

class QskTextureNodePrivate;

class QSK_EXPORT QskTextureNode : public QSGGeometryNode
{
  public:
    QskTextureNode();
    ~QskTextureNode() override;

    void setRect( const QRectF& );
    QRectF rect() const;

    void setTextureId( uint id );
    uint textureId() const;

    void setMirrored( Qt::Orientations );
    Qt::Orientations mirrored() const;

  private:
    Q_DECLARE_PRIVATE( QskTextureNode )
};

#endif
