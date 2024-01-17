/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_BASIC_LINES_NODE_H
#define QSK_BASIC_LINES_NODE_H

#include "QskGlobal.h"

#include <qsgnode.h>
#include <qnamespace.h>

class QColor;

class QskBasicLinesNodePrivate;

/*
    A node for stippled or solid lines.
    For the moment limited to horizontal/vertical lines: TODO
 */
class QSK_EXPORT QskBasicLinesNode : public QSGGeometryNode
{
    using Inherited = QSGGeometryNode;

  public:
    QskBasicLinesNode();
    ~QskBasicLinesNode() override;

    void setPixelAlignment( Qt::Orientations );
    Qt::Orientations pixelAlignment() const;

    void setColor( const QColor& );
    QColor color() const;

    void setLineWidth( float );
    float lineWidth() const;

  private:
    Q_DECLARE_PRIVATE( QskBasicLinesNode )
};

#endif
