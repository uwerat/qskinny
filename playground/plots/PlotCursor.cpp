/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "PlotCursor.h"
#include <qtransform.h>

QSK_SUBCONTROL( PlotCursor, Line )
QSK_SUBCONTROL( PlotCursor, LabelPanel )
QSK_SUBCONTROL( PlotCursor, LabelText )

class PlotCursor::PrivateData
{
  public:
    qreal position = 0.0;
    Qt::Orientation orientation = Qt::Horizontal;
};

PlotCursor::PlotCursor( QObject* object )
    : Inherited( object )
    , m_data( new PrivateData )
{
    setCoordinateType( CanvasCoordinates );
}

PlotCursor::~PlotCursor()
{
}

void PlotCursor::setOrientation( Qt::Orientation orientation )
{
    if ( m_data->orientation != orientation )
    {
        m_data->orientation = orientation;
        markDirty();
    }
}

Qt::Orientation PlotCursor::orientation() const
{
    return m_data->orientation;
}

void PlotCursor::setCanvasPosition( qreal position )
{
    const auto t = transformation().inverted();

    if ( m_data->orientation == Qt::Horizontal )
        position = t.map( QPointF( position, 0.0 ) ).x();
    else
        position = t.map( QPointF( 0.0, position ) ).y();

    setPosition( position );
}

void PlotCursor::setPosition( qreal position )
{
    if ( m_data->position != position )
    {
        m_data->position = position;
        markDirty();
    }
}

qreal PlotCursor::position() const
{
    return m_data->position;
}

void PlotCursor::transformationChanged( ChangeFlags flags )
{
    Inherited::transformationChanged( flags );
}

bool PlotCursor::needsClipping() const
{
    return false;
}

#include "moc_PlotCursor.cpp"
