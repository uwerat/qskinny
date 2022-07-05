/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LAYOUT_CHAIN_H
#define QSK_LAYOUT_CHAIN_H

#include <QskLayoutMetrics.h>
#include <qrect.h>
#include <qvector.h>

class QDebug;

class QskLayoutChain
{
  public:
    class Segment
    {
      public:
        inline qreal end() const { return start + length; }

        qreal start = 0.0;
        qreal length = 0.0;
    };

    typedef QVector< Segment > Segments;

    class CellData
    {
      public:
        inline qreal metric( int which ) const
        {
            return metrics.metric( which );
        }

        inline void setMetric( int which, qreal size )
        {
            metrics.setMetric( which, size );
        }

        int stretch = 0;
        bool canGrow = false;
        bool isShrunk = false;
        bool isValid = false;

        QskLayoutMetrics metrics;
    };

    enum FillMode
    {
        Leading = 1 << 0,
        Trailing = 1 << 1
    };

    QskLayoutChain();
    ~QskLayoutChain();

    void invalidate();

    void reset( int count, qreal constraint );
    void expandCell( int index, const CellData& );
    void expandCells( int start, int end, const CellData& );
    void shrinkCell( int index, const CellData& );
    void finish();

    const CellData& cell( int index ) const { return m_cells[ index ]; }

    bool setSpacing( qreal spacing );
    qreal spacing() const { return m_spacing; }

    void setFillMode( int mode ) { m_fillMode = mode; }
    int fillMode() const { return m_fillMode; }

    Segments segments( qreal size ) const;
    QskLayoutMetrics boundingMetrics() const { return m_boundingMetrics; }

    inline qreal constraint() const { return m_constraint; }
    inline int count() const { return m_cells.size(); }

  private:
    Segments distributed( int which, qreal offset, qreal extra ) const;
    Segments minimumExpanded( qreal size ) const;
    Segments preferredStretched( qreal size ) const;

    QskLayoutMetrics m_boundingMetrics;
    qreal m_constraint = -2.0;

    qreal m_spacing = 0;
    int m_fillMode = 0;

    int m_sumStretches = 0;
    int m_validCells = 0;

    QVector< CellData > m_cells;
};

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<( QDebug, const QskLayoutChain::Segment& );
QDebug operator<<( QDebug, const QskLayoutChain::CellData& );

#endif

Q_DECLARE_TYPEINFO( QskLayoutChain::Segment, Q_MOVABLE_TYPE );
Q_DECLARE_TYPEINFO( QskLayoutChain::CellData, Q_MOVABLE_TYPE );

#endif
