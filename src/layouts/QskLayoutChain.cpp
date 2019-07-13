/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLayoutChain.h"
#include "QskLayoutConstraint.h"

#include <qvarlengtharray.h>
#include <qvector.h>

#ifdef QSK_LAYOUT_COMPAT
#include <cmath>
#endif

QskLayoutChain::QskLayoutChain()
{
}

QskLayoutChain::~QskLayoutChain()
{
}

void QskLayoutChain::invalidate()
{
    m_cells.clear();
    m_constraint = -2;
}

void QskLayoutChain::reset( int count, qreal constraint )
{
    m_cells.assign( count, Cell() );
    m_constraint = constraint;
    m_sumStretches = 0;
    m_validCells = 0;
}

void QskLayoutChain::expandTo( int index, const Cell& newCell )
{
    if ( !newCell.isValid )
        return;

    auto& cell = m_cells[ index ];

    if ( !cell.isValid )
    {
        cell = newCell;
    }
    else
    {
        cell.canGrow |= newCell.canGrow;
        cell.stretch = qMax( cell.stretch, newCell.stretch );
        cell.hint.expandTo( newCell.hint );
    }
}

void QskLayoutChain::finish()
{
    qreal minimum = 0.0;
    qreal preferred = 0.0;
    qreal maximum = 0.0;

    m_sumStretches = 0;
    m_validCells = 0;

    if ( !m_cells.empty() )
    {
        const auto maxMaximum = QskLayoutConstraint::unlimited;

        for ( auto& cell : m_cells )
        {
            if ( !cell.isValid )
                continue;

            minimum += cell.hint.minimum();
            preferred += cell.hint.preferred();

            if ( maximum < maxMaximum )
            {
                if ( cell.stretch == 0 && !cell.canGrow )
                {
                    maximum += cell.hint.preferred();
                }
                else
                {
                    if ( cell.hint.maximum() == maxMaximum )
                        maximum = maxMaximum;
                    else
                        maximum += cell.hint.maximum();
                }
            }

            m_sumStretches += cell.stretch;
            m_validCells++;
        }

        const qreal spacing = ( m_validCells - 1 ) * m_spacing;

        minimum += spacing;
        preferred += spacing;

        if ( maximum < maxMaximum )
            maximum += spacing;
    }

    m_boundingHint.setMinimum( minimum );
    m_boundingHint.setPreferred( preferred );
    m_boundingHint.setMaximum( maximum );
}

bool QskLayoutChain::setSpacing( qreal spacing )
{
    if ( m_spacing != spacing )
    {
        m_spacing = spacing;
        return true;
    }

    return false;
}

QVector< QskLayoutChain::Range > QskLayoutChain::geometries( qreal size ) const
{
    if ( m_validCells == 0 )
        return QVector< Range >();

    QVector< Range > ranges;

    if ( size <= m_boundingHint.minimum() )
    {
        ranges = distributed( Qt::MinimumSize, 0.0, 0.0 );
    }
    else if ( size < m_boundingHint.preferred() )
    {
        ranges = minimumExpanded( size );
    }
    else if ( size <= m_boundingHint.maximum() )
    {
        ranges = preferredStretched( size );
    }
    else
    {
        const qreal padding = size - m_boundingHint.maximum();

        qreal offset = 0.0;
        qreal extra = 0.0;;

        if ( m_extraSpacingAt == Qt::LeftEdge )
        {
            offset = padding;
        }
        else if ( m_extraSpacingAt == Qt::RightEdge )
        {
            offset = 0.0;
        }
        else if ( m_extraSpacingAt == ( Qt::LeftEdge | Qt::RightEdge ) )
        {
            offset = 0.5 * padding;
        }
        else
        {
            extra = padding / m_validCells;
        }

        ranges = distributed( Qt::MaximumSize, offset, extra );
    }

    return ranges;
}

QVector< QskLayoutChain::Range > QskLayoutChain::distributed(
    int which, qreal offset, const qreal extra ) const
{
    qreal fillSpacing = 0.0;

    QVector< Range > ranges( m_cells.size() );

    for ( int i = 0; i < ranges.count(); i++ )
    {
        const auto& cell = m_cells[i];
        auto& range = ranges[i];

        if ( !cell.isValid )
        {
            range.start = offset;
            range.length = 0.0;
        }
        else
        {
            offset += fillSpacing;
            fillSpacing = m_spacing;

            range.start = offset;
            range.length = cell.hint.size( which ) + extra;

            offset += range.length;
        }
    }

    return ranges;
}

QVector< QskLayoutChain::Range > QskLayoutChain::minimumExpanded( qreal size ) const
{
    QVector< Range > ranges( m_cells.size() );

    qreal fillSpacing = 0.0;
    qreal offset = 0.0;

    /*
        We have different options how to distribute the availabe space

        - according to the preferred sizes

        - items with a larger preferred size are stretchier: this is
          what QSK_LAYOUT_COMPAT does ( compatible with QGridLayoutEngine )

        - somehow using the stretch factors
     */

#ifdef QSK_LAYOUT_COMPAT

    /*
         Code does not make much sense, but this is what QGridLayoutEngine does.
         The implementation is intended to help during the migration, but is supposed
         to be removed then TODO ...
     */
    qreal sumFactors = 0.0;
    QVarLengthArray< qreal > factors( m_cells.size() );

    const qreal desired = m_boundingHint.preferred() - m_boundingHint.minimum();
    const qreal available = size - m_boundingHint.minimum();

    for ( uint i = 0; i < m_cells.size(); i++ )
    {
        const auto& cell = m_cells[i];
        if ( !cell.isValid )
        {
            factors[i] = 0.0;
        }
        else
        {
            const qreal l = cell.hint.preferred() - cell.hint.minimum();

            factors[i] = l * std::pow( available / desired, l / desired );
            sumFactors += factors[i];
        }
    }

    for ( uint i = 0; i < m_cells.size(); i++ )
    {
        const auto& cell = m_cells[i];
        auto& range = ranges[i];

        if ( !cell.isValid )
        {
            range.start = offset;
            range.length = 0.0;
        }
        else
        {
            offset += fillSpacing;
            fillSpacing = m_spacing;

            range.start = offset;
            range.length = cell.hint.minimum()
                + available * ( factors[i] / sumFactors );

            offset += range.length;
        }
    }
#else
    const qreal factor = ( size - m_boundingHint.minimum() ) /
        ( m_boundingHint.preferred() - m_boundingHint.minimum() );

    for ( uint i = 0; i < m_cells.size(); i++ )
    {
        const auto& cell = m_cells[i];
        auto& range = ranges[i];

        if ( !cell.isValid )
        {
            range.start = offset;
            range.length = 0.0;
        }
        else
        {
            offset += fillSpacing;
            fillSpacing = m_spacing;

            range.start = offset;
            range.length = cell.hint.minimum()
                + factor * ( cell.hint.preferred() - cell.hint.minimum() );

            offset += range.length;
        }
    }
#endif

    return ranges;
}

QVector< QskLayoutChain::Range > QskLayoutChain::preferredStretched( qreal size ) const
{
    const int count = m_cells.size();

    qreal sumFactors = 0.0;

    QVarLengthArray< qreal > factors( count );
    QVector< Range > ranges( count );

    for ( int i = 0; i < count; i++ )
    {
        const auto& cell = m_cells[i];

        if ( !cell.isValid )
        {
            ranges[i].length = 0.0;
            factors[i] = -1.0;
            continue;
        }

        if ( cell.hint.preferred() >= cell.hint.maximum() )
        {
            factors[i] = 0.0;
        }
        else
        {
            if ( m_sumStretches == 0 )
                factors[i] = cell.canGrow ? 1.0 : 0.0;
            else
                factors[i] = cell.stretch;

        }

        sumFactors += factors[i];
    }

    auto sumSizes = size - ( m_validCells - 1 ) * m_spacing;

    Q_FOREVER
    {
        bool done = true;

        for ( int i = 0; i < count; i++ )
        {
            if ( factors[i] < 0.0 )
                continue;

            const auto size = sumSizes * factors[i] / sumFactors;

            const auto& hint = m_cells[i].hint;
            const auto boundedSize =
                qBound( hint.preferred(), size, hint.maximum() );

            if ( boundedSize != size )
            {
                ranges[i].length = boundedSize;
                sumSizes -= boundedSize;
                sumFactors -= factors[i];
                factors[i] = -1.0;

                done = false;
            }
        }

        if ( done )
            break;
    }

    qreal offset = 0;
    qreal fillSpacing = 0.0;

    for ( int i = 0; i < count; i++ )
    {
        const auto& cell = m_cells[i];
        auto& range = ranges[i];

        const auto& factor = factors[i];

        if ( cell.isValid )
        {
            offset += fillSpacing;
            fillSpacing = m_spacing;
        }

        range.start = offset;

        if ( factor >= 0.0 )
        {
            if ( factor > 0.0 )
                range.length = sumSizes * factor / sumFactors;
            else
                range.length = cell.hint.preferred();
        }

        offset += range.length;
    }

    return ranges;
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskLayoutChain::Range& range )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "( " << range.start << ", " << range.end() << " )";

    return debug;
}

QDebug operator<<( QDebug debug, const QskLayoutChain::Cell& cell )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    if ( !cell.isValid )
    {
        debug << "( " << "Invalid " << " )";
    }
    else
    {
        debug << "( " << cell.hint << ", "
            << cell.stretch << ", " << cell.canGrow << " )";
    }

    return debug;
}

#endif

