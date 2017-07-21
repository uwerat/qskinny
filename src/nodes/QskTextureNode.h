/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TEXTURE_NODE_H
#define QSK_TEXTURE_NODE_H

#include "QskGlobal.h"

#include <QSGGeometryNode>
#include <Qt>

class QskTextureNodePrivate;

class QSK_EXPORT QskTextureNode : public QSGGeometryNode
{
public:
    QskTextureNode();
    virtual ~QskTextureNode();

    void setRect(const QRectF& rect);
    QRectF rect() const;

    void setTextureId( int id );
    int textureId() const;

    void setMirrored( Qt::Orientations );
    Qt::Orientations mirrored() const;

private:
    void updateTexture();

    Q_DECLARE_PRIVATE( QskTextureNode )
};

#endif
