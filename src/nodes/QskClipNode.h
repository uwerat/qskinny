/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_CLIP_NODE_H
#define QSK_CLIP_NODE_H

#include "QskGlobal.h"
#include <QSGClipNode>

class QSK_EXPORT QskClipNode : public QSGClipNode
{
public:
    QskClipNode();
    virtual ~QskClipNode();

    void setRadius( double radius );
    void setRect( const QRectF& rect );

    void update();

private:
    double m_radius;
    bool m_isDirty;

    QSGGeometry m_geometry;
};

#endif
