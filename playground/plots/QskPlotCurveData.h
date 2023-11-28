/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <qobject.h>
#include <qvector.h>
#include <qrect.h>
#include <qnamespace.h>

// Hiding the layout of the data behind an abstract API
class QskPlotCurveData : public QObject
{
    Q_OBJECT

  public:
    enum Hint
    {
        /*
            The points are monotonic in/decreasing. F.e a faster algos
            can be implemented with this information ( f.e polygon clipping )
         */
        MonotonicX = 1 << 0,
        MonotonicY = 1 << 1,

        /*
            The data offers a bounding rectangle, that can f.e be used for
            clipping or autoscaling purposes
         */
        BoundingRectangle = 1 << 2
    };
    Q_ENUM( Hint );

    Q_DECLARE_FLAGS( Hints, Hint )

    QskPlotCurveData( QObject* parent = nullptr );
    virtual ~QskPlotCurveData();

    void setHints( Hints );
    Hints hints() const;

    void setHint( Hint, bool on = true );

    virtual qsizetype count() const = 0;
    virtual QPointF pointAt( qsizetype index ) const = 0;

    virtual QRectF boundingRect() const;

    int upperIndex( Qt::Orientation, qreal value ) const;
    QPointF interpolatedPoint( Qt::Orientation, qreal value ) const;

  Q_SIGNALS:
    void changed();

  protected:
    mutable QRectF m_boundingRect;

  private:
    Hints m_hints = BoundingRectangle;
};

inline QskPlotCurveData::Hints QskPlotCurveData::hints() const
{
    return m_hints;
}

Q_DECLARE_OPERATORS_FOR_FLAGS( QskPlotCurveData::Hints )

// A simple implementation using QVector< QPointF >
class QskPlotCurvePoints : public QskPlotCurveData
{
    Q_OBJECT

    using Inherited = QskPlotCurveData;

  public:
    QskPlotCurvePoints( QObject* parent = nullptr );
    QskPlotCurvePoints( const QVector< QPointF >&, QObject* parent = nullptr );

    void setPoints( const QVector< QPointF >& );
    QVector< QPointF > points() const;

    qsizetype count() const override;
    QPointF pointAt( qsizetype index ) const override;

  private:
    QVector< QPointF > m_points;
};

inline QVector< QPointF > QskPlotCurvePoints::points() const
{
    return m_points;
}

inline qsizetype QskPlotCurvePoints::count() const
{
    return m_points.count();
}

inline QPointF QskPlotCurvePoints::pointAt( qsizetype index ) const
{
    return m_points.at( index );
}
