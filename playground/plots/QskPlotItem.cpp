/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "QskPlotItem.h"
#include "QskPlotView.h"

#include <QskSkinlet.h>
#include <QskIntervalF.h>
#include <qpointer.h>

class QskPlotItem::PrivateData
{
  public:
    QskPlot::Axis xAxis = QskPlot::XBottom;
    QskPlot::Axis yAxis = QskPlot::YLeft;
    qreal z = 0.0;

    QPointer< QskPlotView > view;

    CoordinateType coordinateType = PlotCoordinates;
    bool dirty = true;
};

QskPlotItem::QskPlotItem( QObject* parent )
    : QObject( parent )
    , m_data( new PrivateData )
{
    if ( auto view = qobject_cast< QskPlotView* >( parent ) )
        attach( view );
}

QskPlotItem::~QskPlotItem()
{
    attach( nullptr );
}

void QskPlotItem::attach( QskPlotView* view )
{
    if ( view == m_data->view )
        return;

    if ( m_data->view )
        m_data->view->detachItem( this );

    m_data->view = view;

    if ( m_data->view )
        m_data->view->attachItem( this );
}

void QskPlotItem::setAxes( QskPlot::Axis xAxis, QskPlot::Axis yAxis )
{
    setXAxis( xAxis );
    setYAxis( yAxis );
}

QskPlot::Axis QskPlotItem::xAxis() const
{
    return m_data->xAxis;
}

void QskPlotItem::setXAxis( QskPlot::Axis axis )
{
    if ( m_data->xAxis != axis )
    {
        m_data->xAxis = axis;
        Q_EMIT axisChanged();

        markDirty();
    }
}

QskPlot::Axis QskPlotItem::yAxis() const
{
    return m_data->yAxis;
}

void QskPlotItem::setYAxis( QskPlot::Axis axis )
{
    if ( m_data->yAxis != axis )
    {
        m_data->yAxis = axis;
        Q_EMIT axisChanged();

        markDirty();
    }
}

qreal QskPlotItem::z() const
{
    return m_data->z;
}

bool QskPlotItem::isDirty() const
{
    return m_data->dirty;
}

void QskPlotItem::setZ( qreal z )
{
    if ( m_data->z != z )
    {
        m_data->z = z;
        Q_EMIT zChanged( z );

        if ( auto view = m_data->view )
        {
            view->detachItem( this );
            view->attachItem( this );
        }
    }
}

void QskPlotItem::markDirty()
{
    if ( !m_data->dirty )
    {
        m_data->dirty = true;
        updatePlot();
    }
}

void QskPlotItem::resetDirty()
{
    m_data->dirty = false;
}

void QskPlotItem::updatePlot()
{
    if ( m_data->view )
        m_data->view->update();
}

void QskPlotItem::setCoordinateType( CoordinateType type )
{
    if ( m_data->coordinateType != type )
    {
        m_data->coordinateType = type;
        m_data->dirty = true;
    }
}

QskPlotItem::CoordinateType QskPlotItem::coordinateType() const
{
    return m_data->coordinateType;
}

QTransform QskPlotItem::transformation() const
{
    if ( m_data->view )
        return m_data->view->transformation( m_data->xAxis, m_data->yAxis );

    return QTransform();
}

const QskPlotView* QskPlotItem::view() const
{
    return m_data->view;
}

QQuickItem* QskPlotItem::owningItem() const
{
    return m_data->view;
}

void QskPlotItem::updateNode( QSGNode* node )
{
    if ( auto skinlet = effectiveSkinlet() )
        skinlet->updateNode( this, node );

    resetDirty();
}

bool QskPlotItem::needsClipping() const
{
    return false;
}

void QskPlotItem::transformationChanged( ChangeFlags flags )
{
    if ( flags == CanvasGeometryChanged && coordinateType() == PlotCoordinates )
        return;

    markDirty();
}

QRectF QskPlotItem::scaleRect() const
{
    if ( auto view = m_data->view )
    {
        return QskIntervalF::toRect(
            view->boundaries( m_data->xAxis ),
            view->boundaries( m_data->yAxis ) );
    }

    return QRectF();
}

#include "moc_QskPlotItem.cpp"
