/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FILL_NODE_H
#define QSK_FILL_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>
#include <qcolor.h>

class QskFillNodePrivate;
class QskGradient;

class QSK_EXPORT QskFillNode : public QSGGeometryNode
{
    Q_GADGET

    using Inherited = QSGGeometryNode;

  public:
    enum Coloring
    {
        Monochrome,
        Polychrome,

        Linear,
        Radial,
        Conic
    };

    enum Hint
    {
        /*
            Colors might be defined in the material ( QskGradientMaterial,
            QSGFlatColorMaterial ) or attached to each point ( QSGVertexColorMaterial ).

            The main advantage of using colored points is, that the material becomes
            independent of the coloring and the scene graph is able to batch the nodes
            ( https://doc.qt.io/qt-6/qtquick-visualcanvas-scenegraph.html ).

            However adding the color information for each point increases the memory
            footprint.

            The default setting is to use colored points where possible. Note, that
            this is what is also done in the Qt/Quick code.
         */
        PreferColoredGeometry = 1
    };

    Q_ENUM( Hint )
    Q_DECLARE_FLAGS( Hints, Hint )

    QskFillNode();
    ~QskFillNode() override;

    void resetGeometry();

    void setColoring( Coloring );
    Coloring coloring() const;

    void setColoring( QRgb );
    void setColoring( const QColor& );
    void setColoring( Qt::GlobalColor );

    void setColoring( const QRectF&, const QskGradient& );

    bool isGeometryColored() const;

    void setHints( Hints );
    Hints hints() const;

    void setHint( Hint, bool on = true );
    bool hasHint( Hint ) const;

  protected:
    QskFillNode( QskFillNodePrivate& );

  private:
    Q_DECLARE_PRIVATE( QskFillNode )
};

inline void QskFillNode::setColoring( QRgb rgb )
{
    setColoring( QColor::fromRgba( rgb ) );
}

inline void QskFillNode::setColoring( Qt::GlobalColor color )
{
    setColoring( QColor( color ) );
}

#endif
