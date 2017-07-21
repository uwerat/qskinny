/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_NODE_H
#define QSK_BOX_NODE_H

#include "QskGlobal.h"
#include "QskBorderGeometry.h"

#define VM_SUPPORT 1

#if VM_SUPPORT
#include "QskBoxMaterialVM.h"
#else
#include "QskBoxMaterial.h"
#endif

#include <QSGGeometryNode>

class QSK_EXPORT QskBoxNode : public QSGGeometryNode
{
public:
    QskBoxNode();
    virtual ~QskBoxNode();

    void setBoxData( const QRectF&, const QskBoxOptions& );

    QRectF rect() const;

private:
    uint m_metricsHash;
    uint m_colorsHash;

    QRectF m_rect;

#if VM_SUPPORT
    QskBoxMaterialVM m_material;
#else
    QskBoxMaterial m_material;
#endif
    QskBorderGeometry m_geometry;
};

#endif
