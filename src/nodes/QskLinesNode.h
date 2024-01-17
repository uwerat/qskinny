/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_LINES_NODE_H
#define QSK_LINES_NODE_H

#include "QskBasicLinesNode.h"
#include <qvector.h>

class QskIntervalF;
class QskStippleMetrics;
class QTransform;
class QPointF;
class QLineF;
class QPolygonF;

/*
    A node for stippled or solid lines.
    For the moment limited to horizontal/vertical lines: TODO
 */
class QSK_EXPORT QskLinesNode : public QskBasicLinesNode
{
    using Inherited = QskBasicLinesNode;

  public:
    QskLinesNode();
    ~QskLinesNode() override;

    void updateGrid( const QColor&, qreal lineWidth,
        const QskStippleMetrics&, const QTransform&, const QRectF&,
        const QVector< qreal >&, const QVector< qreal >& );

    void updateRect( const QColor&, qreal lineWidth,
        const QskStippleMetrics&, const QTransform&, const QRectF& );

    void updateLine( const QColor&, qreal lineWidth,
        const QPointF&, const QPointF& );

    void updateLine( const QColor&, qreal lineWidth,
        const QskStippleMetrics&, const QTransform&,
        const QPointF&, const QPointF& );

    void updateLines( const QColor&, qreal lineWidth, const QVector< QLineF >& );

    void updateLines( const QColor&, qreal lineWidth,
        const QskStippleMetrics&, const QTransform&, const QVector< QLineF >& );

    void updatePolygon( const QColor&, qreal lineWidth,
        const QTransform&, const QPolygonF& );

  private:
    void updateLines( const QColor&, qreal lineWidth, const QskStippleMetrics&,
        const QTransform&, int count, const QLineF* );

    void updateGeometry( const QskStippleMetrics&, const QTransform&,
        int count, const QLineF* );

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

    QskHashValue m_hash = 0.0;
};

#endif
