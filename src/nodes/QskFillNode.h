/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FILL_NODE_H
#define QSK_FILL_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QskFillNodePrivate;
class QskGradient;

class QSK_EXPORT QskFillNode : public QSGGeometryNode
{
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

    QskFillNode();
    ~QskFillNode() override;

    void resetGeometry();

    void setColoring( Coloring );
    Coloring coloring() const;

    void setColoring( const QColor& );
    void setColoring( const QRectF&, const QskGradient& );

    bool isGeometryColored() const;

  protected:
    QskFillNode( QskFillNodePrivate& );

  private:
    Q_DECLARE_PRIVATE( QskFillNode )
};

#endif
