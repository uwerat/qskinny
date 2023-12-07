/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <qsgnode.h>

class QRect;
class QSGTexture;
class BlurredTextureNodePrivate;

class BlurredTextureNode : public QSGGeometryNode
{
    using Inherited = QSGGeometryNode;

  public:
    BlurredTextureNode();
    ~BlurredTextureNode();

    void setTexture( QSGTexture* );
    QSGTexture* texture();

    void setRect( const QRectF& );
    QRectF rect() const;

  private:
    Q_DECLARE_PRIVATE( BlurredTextureNode )
};
