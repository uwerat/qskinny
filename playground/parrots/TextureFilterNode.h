/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <qsgnode.h>

class QRect;
class QSGTexture;

class TextureFilterNodePrivate;

class TextureFilterNode : public QSGGeometryNode
{
    using Inherited = QSGGeometryNode;

  public:
    TextureFilterNode();
    ~TextureFilterNode();

    void setTexture( QSGTexture* );
    QSGTexture* texture();

#if 0
    // deriving from QSGImageNode ???
    void setOwnsTexture( bool );
    bool ownsTexture() const;
#endif
    void setRect( const QRectF& );

  private:
    Q_DECLARE_PRIVATE( TextureFilterNode )
};
