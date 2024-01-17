/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_STIPPLED_LINE_RENDERER_H
#define QSK_STIPPLED_LINE_RENDERER_H

#include "QskStippleMetrics.h"

class QLineF;
class QPointF;

/*
    A wrapper for the non public QDashStroker class, tailored for
    splitting lines into dashes/dots. It is faster than QPainterPathStroker
    ( no QPainterPath involved ), but supports simple lines only.
 */
class QskStippledLineRenderer
{
  public:
    QskStippledLineRenderer( const QskStippleMetrics& );
    virtual ~QskStippledLineRenderer();

    qsizetype dashCount( qreal x1, qreal y1, qreal x2, qreal y2 ) const;
    qsizetype dashCount( const QPointF&, const QPointF& ) const;
    qsizetype dashCount( const QLineF& ) const;

    void renderLine( qreal x1, qreal y1, qreal x2, qreal y2 );
    void renderLine( const QPointF&, const QPointF& );
    void renderLine( const QLineF& );

    const QskStippleMetrics& metrics() const;

    // nop: to be overloaded
    virtual void renderDash( qreal x1, qreal y1, qreal x2, qreal y2 );

  private:
    const QskStippleMetrics m_metrics;
};

inline const QskStippleMetrics& QskStippledLineRenderer::metrics() const
{
    return m_metrics;
}

#endif
