/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_AXIS_SCALE_NODE_H
#define QSK_AXIS_SCALE_NODE_H

#include "QskGlobal.h"
#include "QskBasicLinesNode.h"

#include <qnamespace.h>

class QRectF;
class QskIntervalF;
class QskTickmarks;

class QskAxisScaleNodePrivate;

class QSK_EXPORT QskAxisScaleNode : public QskBasicLinesNode
{
    using Inherited = QskBasicLinesNode;

  public:
    enum Alignment
    {
        Leading,
        Centered,
        Trailing
    };

    QskAxisScaleNode();
    ~QskAxisScaleNode() override;

    void setAxis( Qt::Orientation, qreal pos, const QTransform& );
    void setTickGeometry( Alignment, qreal tickLength, qreal tickWidth );

    void update( const QskTickmarks&, const QskIntervalF& );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
