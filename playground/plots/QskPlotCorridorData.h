/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskIntervalF.h>

#include <qobject.h>
#include <qvector.h>
#include <qrect.h>

class QskPlotCorridorSample
{
  public:
    qreal value = 0.0;
    QskIntervalF boundary;
};

Q_DECLARE_TYPEINFO( QskPlotCorridorSample, Q_MOVABLE_TYPE );

// Hiding the layout of the data behind an abstract API
class QskPlotCorridorData : public QObject
{
    Q_OBJECT

  public:
    QskPlotCorridorData( QObject* parent = nullptr );
    virtual ~QskPlotCorridorData();

    virtual qsizetype count() const = 0;
    virtual QskPlotCorridorSample sampleAt( qsizetype index ) const = 0;

    virtual QRectF boundingRect() const;

    int upperIndex( qreal value ) const;
    QskPlotCorridorSample interpolatedSample( qreal value ) const;

  Q_SIGNALS:
    void changed();

  protected:
    mutable QRectF m_boundingRect;
};

// A simple implementation using QVector< CorridorSample >
class QskPlotCorridorSamples : public QskPlotCorridorData
{
    Q_OBJECT

    using Inherited = QskPlotCorridorData;

  public:
    QskPlotCorridorSamples( QObject* parent = nullptr );
    QskPlotCorridorSamples(
        const QVector< QskPlotCorridorSample >&, QObject* parent = nullptr );

    void setSamples( const QVector< QskPlotCorridorSample >& );
    QVector< QskPlotCorridorSample > samples() const;

    qsizetype count() const override;
    QskPlotCorridorSample sampleAt( qsizetype index ) const override;

  private:
    QVector< QskPlotCorridorSample > m_samples;
};

inline QVector< QskPlotCorridorSample > QskPlotCorridorSamples::samples() const
{
    return m_samples;
}

inline qsizetype QskPlotCorridorSamples::count() const
{
    return m_samples.count();
}

inline QskPlotCorridorSample QskPlotCorridorSamples::sampleAt( qsizetype index ) const
{
    return m_samples.at( index );
}
