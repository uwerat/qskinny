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
}

void QskLayoutChain::expandTo( int index, const Cell& newCell )
{
    auto& cell = m_cells[ index ];

    cell.canGrow |= newCell.canGrow;
    cell.stretch = qMax( cell.stretch, newCell.stretch );
    cell.hint.intersect( newCell.hint );
}

void QskLayoutChain::finish()
{
    qreal minimum = 0.0;
    qreal preferred = 0.0;
    qreal maximum = 0.0;

    m_sumStretches = 0;

    if ( !m_cells.empty() )
    {
        const auto maxMaximum = QskLayoutConstraint::unlimited;

        for ( auto& cell : m_cells )
        {
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
        }

        const qreal spacing = ( m_cells.size() - 1 ) * m_spacing;

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
            extra = padding / m_cells.size();
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
        auto& range = ranges[i];

        offset += fillSpacing;
        fillSpacing = m_spacing;

        range.start = offset;
        range.length = m_cells[i].hint.size( which ) + extra;

        offset += range.length;
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
        const auto& hint = m_cells[i].hint;

        const qreal l = hint.preferred() - hint.minimum();

        factors[i] = l * std::pow( available / desired, l / desired );
        sumFactors += factors[i];
    }

    for ( uint i = 0; i < m_cells.size(); i++ )
    {
        const auto& hint = m_cells[i].hint;

        auto& range = ranges[i];

        offset += fillSpacing;
        fillSpacing = m_spacing;

        range.start = offset;
        range.length = hint.minimum() + available * ( factors[i] / sumFactors );

        offset += range.length;
    }
#else
    const qreal factor = ( size - m_boundingHint.minimum() ) /
        ( m_boundingHint.preferred() - m_boundingHint.minimum() );

    for ( uint i = 0; i < m_cells.size(); i++ )
    {
        const auto& hint = m_cells[i].hint;

        auto& range = ranges[i];

        offset += fillSpacing;
        fillSpacing = m_spacing;

        range.start = offset;
        range.length = hint.minimum() + factor * ( hint.preferred() - hint.minimum() );

        offset += range.length;
    }
#endif

    return ranges;
}

QVector< QskLayoutChain::Range > QskLayoutChain::preferredStretched( qreal size ) const
{
    const int count = m_cells.size();
    auto sumSizes = size - ( count - 1 ) * m_spacing;

    qreal sumFactors = 0.0;
    QVarLengthArray< qreal > factors( count );

    for ( int i = 0; i < count; i++ )
    {
        const auto& hint = m_cells[i].hint;

        if ( hint.preferred() >= hint.maximum() )
        {
            factors[i] = 0.0;
        }
        else
        {
            if ( m_sumStretches == 0 )
                factors[i] = m_cells[i].canGrow ? 1.0 : 0.0;
            else
                factors[i] = m_cells[i].stretch;
        }

        sumFactors += factors[i];
    }

    QVector< Range > ranges( count );

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
        auto& range = ranges[i];
        const auto& factor = factors[i];

        offset += fillSpacing;
        fillSpacing = m_spacing;

        range.start = offset;

        if ( factor >= 0.0 )
        {
            if ( factor > 0.0 )
                range.length = sumSizes * factor / sumFactors;
            else
                range.length = m_cells[i].hint.preferred();
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

    debug << "( " << cell.hint << ", "
        << cell.stretch << ", " << cell.canGrow << " )";
    return debug;
}

#endif

