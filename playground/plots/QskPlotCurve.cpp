/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "QskPlotCurve.h"
#include "QskPlotCurveData.h"

#include <qpointer.h>
#include <qcolor.h>

QSK_SUBCONTROL( QskPlotCurve, Line )

class QskPlotCurve::PrivateData
{
  public:
    QPointer< QskPlotCurveData > curveData;
};

QskPlotCurve::QskPlotCurve( QObject* object )
    : Inherited( object )
    , m_data( new PrivateData )
{
}

QskPlotCurve::~QskPlotCurve()
{
}

void QskPlotCurve::setColor( const QColor& color )
{
    if ( setColor( Line, color ) )
    {
        markDirty();
        Q_EMIT colorChanged( color );
    }
}

QColor QskPlotCurve::color() const
{
    return color( Line );
}

void QskPlotCurve::setLineWidth( qreal lineWidth )
{
    lineWidth = qMax( lineWidth, 0.0 );

    if ( setMetric( Line | QskAspect::Size, lineWidth ) )
    {
        markDirty();
        Q_EMIT lineWidthChanged( lineWidth );
    }
}

qreal QskPlotCurve::lineWidth() const
{
    return metric( Line | QskAspect::Size );
}

void QskPlotCurve::setPoints( const QVector< QPointF >& points )
{
    setData( new QskPlotCurvePoints( points, this ) );
}

void QskPlotCurve::setData( QskPlotCurveData* curveData )
{
    if ( curveData == m_data->curveData )
        return;

    auto oldData = m_data->curveData.data();
    m_data->curveData = curveData;

    if ( oldData )
    {
        if ( oldData->parent() == this )
            delete oldData;
        else
            disconnect( oldData, &QskPlotCurveData::changed, this, &QskPlotItem::markDirty );
    }

    if ( curveData )
    {
        if ( curveData->parent() == nullptr )
            curveData->setParent( this );

        connect( curveData, &QskPlotCurveData::changed, this, &QskPlotItem::markDirty );
    }

    markDirty();
}

QskPlotCurveData* QskPlotCurve::data() const
{
    return m_data->curveData;
}

QPointF QskPlotCurve::interpolatedPoint(
    Qt::Orientation orientation, qreal value ) const
{
    if ( m_data->curveData )
        return m_data->curveData->interpolatedPoint( orientation, value );

    return QPointF();
}

void QskPlotCurve::transformationChanged( ChangeFlags flags )
{
    if ( flags & ( XBoundariesChanged | YBoundariesChanged ) )
    {
        /*
            We could skip updates, when the curve is inside
            of old and new boundaries TODO ...
         */
    }

    Inherited::transformationChanged( flags );
}

bool QskPlotCurve::needsClipping() const
{
    auto data = m_data->curveData.data();
    if ( data == nullptr || data->count() == 0 )
        return false;

    // The skinlet does basic polygon clipping for monotonic data.

    using D = QskPlotCurveData;

    const auto hints = data->hints();
    if ( ( hints & D::MonotonicX ) && ( hints & D::MonotonicY ) )
        return false;

    if ( data->hints() & QskPlotCurveData::BoundingRectangle )
    {
        const auto curveRect = data->boundingRect();
        const auto plotRect = scaleRect();

        if ( hints & D::MonotonicX )
        {
            return ( curveRect.top() < plotRect.top() )
                || ( curveRect.bottom() > plotRect.bottom() );
        }

        if ( hints & D::MonotonicY )
        {
            return ( curveRect.left() < plotRect.left() )
                && ( curveRect.right() > plotRect.right() );
        }

        return !plotRect.contains( curveRect );
    }

    return true;
}

#include "moc_QskPlotCurve.cpp"
