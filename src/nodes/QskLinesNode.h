/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_LINES_NODE_H
#define QSK_LINES_NODE_H

#include "QskGlobal.h"

#include <qsgnode.h>
#include <qvector.h>

class QskIntervalF;
class QskStippleMetrics;
class QTransform;
class QPointF;
class QQuickItem;

class QskLinesNodePrivate;

/*
    A node for stippled or solid lines.
    For the moment limited to horizontal/vertical lines: TODO
 */
class QSK_EXPORT QskLinesNode : public QSGGeometryNode
{
  public:
    QskLinesNode();
    ~QskLinesNode() override;

    void setGlobalPosition( const QPointF&, qreal devicePixelRatio );
    void setGlobalPosition( const QQuickItem* );

    void updateLines( const QColor&, qreal lineWidth,
        const QskStippleMetrics&, const QTransform&, const QRectF&,
        const QVector< qreal >&, const QVector< qreal >& );

    void updateLines( const QColor&, qreal lineWidth,
        const QskStippleMetrics&, const QTransform&, const QRectF& );

  private:
    void updateGeometry(
        const QskStippleMetrics&, const QTransform&, const QRectF&,
        const QVector< qreal >&, const QVector< qreal >& );

    QSGGeometry::Point2D* setSolidLines(
        Qt::Orientation, qreal, qreal,
        const QTransform&, int count, const qreal* values,
        QSGGeometry::Point2D* ) const;

    QSGGeometry::Point2D* setStippledLines(
        Qt::Orientation, qreal, qreal,
        const QTransform&, int count, const qreal* values,
        const QskStippleMetrics&, QSGGeometry::Point2D* ) const;

    Q_DECLARE_PRIVATE( QskLinesNode )
};

#endif
