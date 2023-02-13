/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskGradient.h>
#include <QQuickItem>

class GradientView : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY( QskGradient gradient READ gradient WRITE setGradient )

  public:
    enum NodeType
    {
        Painted,
        BoxFill,
        BoxRectangle,
#ifdef SHAPE_GRADIENT
        Shape,
#endif

        NumNodeTypes
    };
    Q_ENUM( NodeType )

    GradientView( NodeType, QQuickItem* parent = nullptr );

    NodeType nodeType() const;

    void setGradient( const QskGradient& );
    QskGradient gradient() const;

  protected:
    QSGNode* updatePaintNode( QSGNode*, QQuickItem::UpdatePaintNodeData* ) override;
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    void geometryChange( const QRectF&, const QRectF& ) override;
#else
    void geometryChanged( const QRectF&, const QRectF& ) override;
#endif

  private:
    const NodeType m_nodeType;
    QskGradient m_gradient;
};
