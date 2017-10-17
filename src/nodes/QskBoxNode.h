/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_NODE_H
#define QSK_BOX_NODE_H

#include "QskGlobal.h"

class QskBoxOptions;
class QRectF;

#include <QSGNode>

class QSK_EXPORT QskBoxNode : public QSGGeometryNode
{
public:
    QskBoxNode();
    virtual ~QskBoxNode();

    void setBoxData( const QRectF& rect, const QskBoxOptions& );

private:
    void setMonochrome( bool on );

    uint m_metricsHash;
    uint m_colorsHash;
    QRectF m_rect;

    QSGGeometry m_geometry;
};

#endif
