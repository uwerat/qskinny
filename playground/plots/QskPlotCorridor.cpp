/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskPlotCorridor.h"
#include "QskPlotCorridorData.h"

#include <QskIntervalF.h>

#include <qcolor.h>
#include <qpointer.h>

QSK_SUBCONTROL( QskPlotCorridor, Border )
QSK_SUBCONTROL( QskPlotCorridor, Corridor )

class QskPlotCorridor::PrivateData
{
  public:
    QPointer< QskPlotCorridorData > corridorData;
};

QskPlotCorridor::QskPlotCorridor( QObject* object )
    : Inherited( object )
    , m_data( new PrivateData )
{
}

QskPlotCorridor::~QskPlotCorridor()
{
}

void QskPlotCorridor::setBorderWidth( qreal lineWidth )
{
    const auto aspect = Border | QskAspect::Size;

    resetMetric( aspect | QskAspect::Lower );
    resetMetric( aspect | QskAspect::Upper );

    lineWidth = qMax( lineWidth, 0.0 );

    if ( setMetric( aspect, lineWidth ) )
    {
        markDirty();
        Q_EMIT borderWidthChanged( lineWidth );
    }
}

qreal QskPlotCorridor::borderWidth() const
{
    return metric( Border | QskAspect::Size );
}

void QskPlotCorridor::setBorderColor( const QColor& color )
{
    resetColor( Border | QskAspect::Lower );
    resetColor( Border | QskAspect::Upper );

    if ( setColor( Border, color ) )
    {
        markDirty();
        Q_EMIT colorChanged( color );
    }
}

QColor QskPlotCorridor::borderColor() const
{
    return color( Border );
}

void QskPlotCorridor::setColor( const QColor& color )
{
    if ( setColor( Corridor, color ) )
    {
        markDirty();
        Q_EMIT colorChanged( color );
    }
}

QColor QskPlotCorridor::color() const
{
    return color( Corridor );
}

void QskPlotCorridor::setSamples( const QVector< QskPlotCorridorSample >& samples )
{
    setData( new QskPlotCorridorSamples( samples, this ) );
}

void QskPlotCorridor::setData( QskPlotCorridorData* corridorData )
{
    if ( corridorData == m_data->corridorData )
        return;

    auto oldData = m_data->corridorData.data();
    m_data->corridorData = corridorData;

    if ( oldData )
    {
        if ( oldData->parent() == this )
        {
            delete oldData;
        }
        else
        {
            disconnect( oldData, &QskPlotCorridorData::changed,
                this, &QskPlotItem::markDirty );
        }
    }

    if ( corridorData )
    {
        if ( corridorData->parent() == nullptr )
            corridorData->setParent( this );

        connect( corridorData, &QskPlotCorridorData::changed,
            this, &QskPlotItem::markDirty );
    }

    markDirty();
}

QskPlotCorridorData* QskPlotCorridor::data() const
{
    return m_data->corridorData;
}

QskPlotCorridorSample QskPlotCorridor::interpolatedSample( qreal value ) const
{
    if ( m_data->corridorData )
        return m_data->corridorData->interpolatedSample( value );

    return QskPlotCorridorSample();
}

void QskPlotCorridor::transformationChanged( ChangeFlags flags )
{
    Inherited::transformationChanged( flags );
}

bool QskPlotCorridor::needsClipping() const
{
    auto data = m_data->corridorData.data();
    if ( data == nullptr || data->count() == 0 )
        return false;

    // The skinlet does basic polygon clipping in x direction

    const auto corridorRect = data->boundingRect();
    const auto plotRect = scaleRect();

    return ( corridorRect.top() < plotRect.top() )
        || ( corridorRect.bottom() > plotRect.bottom() );
}

#include "moc_QskPlotCorridor.cpp"
