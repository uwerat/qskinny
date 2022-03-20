/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLayoutChain.h"

#include <qvarlengtharray.h>
#include <qvector.h>
#include <qdebug.h>

#include <cmath>

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
    m_cells.fill( CellData(), count );
    m_constraint = constraint;
    m_sumStretches = 0;
    m_validCells = 0;
}

void QskLayoutChain::shrinkCell( int index, const CellData& newCell )
{
    if ( !newCell.isValid )
        return;

    auto& cell = m_cells[ index ];

    if ( !cell.isValid )
    {
        cell = newCell;
        cell.stretch = qMax( cell.stretch, 0 );
        m_validCells++;
    }
    else
    {
        cell.canGrow &= newCell.canGrow;
        if ( newCell.stretch >= 0 )
            cell.stretch = qMax( cell.stretch, newCell.stretch );

        if ( !newCell.metrics.isDefault() )
        {
            auto& metrics = cell.metrics;
            auto& newMetrics = newCell.metrics;

            metrics.setMinimum( qMax( metrics.minimum(), newMetrics.minimum() ) );
            metrics.setPreferred( qMax( metrics.preferred(), newMetrics.preferred() ) );

            if ( newMetrics.maximum() < metrics.maximum() )
            {
                metrics.setMaximum( newMetrics.maximum() );
                cell.isShrunk = true;
            }

            cell.metrics.normalize();
        }
    }
}

void QskLayoutChain::expandCell( int index, const CellData& newCell )
{
    if ( !newCell.isValid )
        return;

    auto& cell = m_cells[ index ];

    if ( !cell.isValid )
    {
        cell = newCell;
        m_validCells++;
    }
    else
    {
        cell.canGrow |= newCell.canGrow;
        cell.stretch = qMax( cell.stretch, newCell.stretch );

        cell.metrics.setMetrics(
            qMax( cell.metrics.minimum(), newCell.metrics.minimum() ),
            qMax( cell.metrics.preferred(), newCell.metrics.preferred() ),
            qMax( cell.metrics.maximum(), newCell.metrics.maximum() )
        );
    }
}

void QskLayoutChain::expandCells(
    int index, int count, const CellData& multiCell )
{
    QskLayoutChain chain;
    chain.setSpacing( m_spacing );
    chain.reset( count, -1 );

    for ( int i = 0; i < count; i++ )
    {
        auto& cell = chain.m_cells[ i ];
        cell = m_cells[ index + i ];

        if ( !cell.isValid )
        {
            cell.isValid = true;
            cell.canGrow = multiCell.canGrow;
            cell.stretch = multiCell.stretch;
        }
    }
    chain.finish();

    QskLayoutChain::Segments minimum;
    QskLayoutChain::Segments preferred;
    QskLayoutChain::Segments maximum;

    const auto chainMetrics = chain.boundingMetrics();

    if ( multiCell.metrics.minimum() > chainMetrics.minimum() )
        minimum = chain.segments( multiCell.metrics.minimum() );

    if ( multiCell.metrics.preferred() > chainMetrics.preferred() )
        preferred = chain.segments( multiCell.metrics.preferred() );

    if ( chainMetrics.maximum() == QskLayoutMetrics::unlimited )
    {
        if ( multiCell.metrics.maximum() < QskLayoutMetrics::unlimited )
            maximum = chain.segments( multiCell.metrics.maximum() );
    }

    for ( int i = 0; i < count; i++ )
    {
        auto& cell = m_cells[ index + i ];

        cell.canGrow |= multiCell.canGrow;
        cell.stretch = qMax( cell.stretch, multiCell.stretch );

        if ( !minimum.isEmpty() )
            cell.metrics.expandMinimum( minimum[i].length );

        if ( !preferred.isEmpty() )
            cell.metrics.expandPreferred( preferred[i].length );

        if ( !maximum.isEmpty() && !cell.isValid )
            cell.metrics.setMaximum( maximum[i].length );

        if ( !cell.isValid )
        {
            cell.isValid = true;
            m_validCells++;
        }
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
        const auto maxMaximum = QskLayoutMetrics::unlimited;

        for ( auto& cell : m_cells )
        {
            if ( !cell.isValid )
                continue;

            minimum += cell.metrics.minimum();
            preferred += cell.metrics.preferred();

            if ( maximum < maxMaximum )
            {
                if ( cell.stretch == 0 && !cell.canGrow )
                {
                    maximum += cell.metrics.preferred();
                }
                else
                {
                    if ( cell.metrics.maximum() == maxMaximum )
                        maximum = maxMaximum;
                    else
                        maximum += cell.metrics.maximum();
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

    m_boundingMetrics.setMinimum( minimum );
    m_boundingMetrics.setPreferred( preferred );
    m_boundingMetrics.setMaximum( maximum );
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

QskLayoutChain::Segments QskLayoutChain::segments( qreal size ) const
{
    if ( m_validCells == 0 )
        return Segments();

    Segments segments;

    if ( size <= m_boundingMetrics.minimum() )
    {
        segments = distributed( Qt::MinimumSize, 0.0, 0.0 );
    }
    else if ( size < m_boundingMetrics.preferred() )
    {
        segments = minimumExpanded( size );
    }
    else if ( size <= m_boundingMetrics.maximum() )
    {
        segments = preferredStretched( size );
    }
    else
    {
        const qreal padding = size - m_boundingMetrics.maximum();

        qreal offset = 0.0;
        qreal extra = 0.0;;

        switch( m_fillMode )
        {
            case Leading:
                offset = padding;
                break;

            case Trailing:
                break;

            case Leading | Trailing:
                offset = 0.5 * padding;
                break;

            default:
                extra = padding / m_validCells;
        }

        segments = distributed( Qt::MaximumSize, offset, extra );
    }

    return segments;
}

QskLayoutChain::Segments QskLayoutChain::distributed(
    int which, qreal offset, const qreal extra ) const
{
    qreal fillSpacing = 0.0;

    Segments segments( m_cells.size() );

    for ( int i = 0; i < segments.count(); i++ )
    {
        const auto& cell = m_cells[i];
        auto& segment = segments[i];

        if ( !cell.isValid )
        {
            segment.start = offset;
            segment.length = 0.0;
        }
        else
        {
            offset += fillSpacing;
            fillSpacing = m_spacing;

            segment.start = offset;

            qreal size = cell.metrics.metric( which );

#if 1
            if ( which == Qt::MaximumSize && size == QskLayoutMetrics::unlimited )
            {
                /*
                    We have some special handling in QskLayoutChain::finish,
                    that adds the preferred instead of the maximum
                    size of a cell to the bounding maximum size.
                    No good way to have this here, TODO ...
                 */
                size = cell.metrics.metric( Qt::PreferredSize );
            }
#endif

            if ( which == Qt::MaximumSize && cell.isShrunk )
            {
                segment.length = size;
                offset += segment.length + extra;
            }
            else
            {
                segment.length = size + extra;
                offset += segment.length;
            }
        }
    }

    return segments;
}

QskLayoutChain::Segments QskLayoutChain::minimumExpanded( qreal size ) const
{
    Segments segments( m_cells.size() );

    qreal fillSpacing = 0.0;
    qreal offset = 0.0;

    /*
        We have different options how to distribute the available space

        - according to the preferred sizes

        - items with a larger preferred size are stretchier: this is
          what QGridLayoutEngine does

        - somehow using the stretch factors
     */

    const qreal factor = ( size - m_boundingMetrics.minimum() ) /
        ( m_boundingMetrics.preferred() - m_boundingMetrics.minimum() );

    for ( int i = 0; i < m_cells.count(); i++ )
    {
        const auto& cell = m_cells[i];
        auto& segment = segments[i];

        if ( !cell.isValid )
        {
            segment.start = offset;
            segment.length = 0.0;
        }
        else
        {
            offset += fillSpacing;
            fillSpacing = m_spacing;

            segment.start = offset;
            segment.length = cell.metrics.minimum()
                + factor * ( cell.metrics.preferred() - cell.metrics.minimum() );

            offset += segment.length;
        }
    }

    return segments;
}

QskLayoutChain::Segments QskLayoutChain::preferredStretched( qreal size ) const
{
    const int count = m_cells.size();

    qreal sumFactors = 0.0;

    QVarLengthArray< qreal > factors( count );
    Segments segments( count );

    for ( int i = 0; i < count; i++ )
    {
        const auto& cell = m_cells[i];

        if ( !cell.isValid )
        {
            segments[i].length = 0.0;
            factors[i] = -1.0;
            continue;
        }

        if ( cell.metrics.preferred() >= cell.metrics.maximum() )
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

            const auto& hint = m_cells[i].metrics;
            const auto boundedSize =
                qBound( hint.preferred(), size, hint.maximum() );

            if ( boundedSize != size )
            {
                segments[i].length = boundedSize;
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
        auto& segment = segments[i];

        const auto& factor = factors[i];

        if ( cell.isValid )
        {
            offset += fillSpacing;
            fillSpacing = m_spacing;
        }

        segment.start = offset;

        if ( factor >= 0.0 )
        {
            if ( factor > 0.0 )
                segment.length = sumSizes * factor / sumFactors;
            else
                segment.length = cell.metrics.preferred();
        }

        offset += segment.length;
    }

    return segments;
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskLayoutChain::Segment& segment )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "( " << segment.start << ", " << segment.end() << " )";

    return debug;
}

QDebug operator<<( QDebug debug, const QskLayoutChain::CellData& cell )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    if ( !cell.isValid )
    {
        debug << "( " << "Invalid " << " )";
    }
    else
    {
        debug << "( " << cell.metrics << ", "
            << cell.stretch << ", " << cell.canGrow << " )";
    }

    return debug;
}

#endif

