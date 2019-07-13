/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LAYOUT_CHAIN_H
#define QSK_LAYOUT_CHAIN_H

#include <QskLayoutHint.h>
#include <qglobal.h>
#include <qvector.h>
#include <vector>

class QDebug;

class QskLayoutChain
{
  public:
    class Range
    {
      public:
        inline qreal end() const { return start + length; }

        qreal start = 0.0;
        qreal length = 0.0;
    };

    class Cell
    {
      public:
        Cell()
        {
        }

        Cell( QskLayoutHint hint, int stretch )
            : hint( hint )
            , stretch( stretch )
            , isValid( true )
        {
        }

        inline bool operator==( const Cell& other ) const
        {
            return ( isValid == other.isValid )
                && ( canGrow == other.canGrow )
                && ( stretch == other.stretch )
                && ( hint == other.hint );
        }

        inline bool operator!=( const Cell& other ) const
        {
            return !( *this == other );
        }

        QskLayoutHint hint;
        int stretch = 0;
        bool canGrow = false;
        bool isValid = false;
    };

    QskLayoutChain();
    ~QskLayoutChain();

    void invalidate();

    void reset( int count, qreal constraint );
    void expandTo( int index, const Cell& );
    void finish();

    const Cell& cell( int index ) const { return m_cells[ index ]; }

    bool setSpacing( qreal spacing );
    qreal spacing() const { return m_spacing; }

    void setExtraSpacingAt( Qt::Edges edges ) { m_extraSpacingAt = edges; }

    QVector< Range > geometries( qreal size ) const;
    QskLayoutHint boundingHint() const { return m_boundingHint; }

    inline qreal constraint() const { return m_constraint; }
    inline int count() const { return m_cells.size(); }

  private:
    Q_DISABLE_COPY( QskLayoutChain )

    QVector< Range > distributed( int which, qreal offset, qreal extra ) const;
    QVector< Range > minimumExpanded( qreal size ) const;
    QVector< Range > preferredStretched( qreal size ) const;

    QskLayoutHint m_boundingHint;
    qreal m_constraint = -2.0;

    qreal m_spacing = 0;
    Qt::Edges m_extraSpacingAt;

    int m_sumStretches = 0;
    int m_validCells = 0;
    std::vector< Cell > m_cells;
};

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<( QDebug, const QskLayoutChain::Range& );
QDebug operator<<( QDebug, const QskLayoutChain::Cell& );

#endif

Q_DECLARE_TYPEINFO( QskLayoutChain::Range, Q_MOVABLE_TYPE );
Q_DECLARE_TYPEINFO( QskLayoutChain::Cell, Q_MOVABLE_TYPE );

#endif
