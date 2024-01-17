/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskBoundedInput.h"
#include "QskFunctions.h"
#include "QskIntervalF.h"
#include "QskEvent.h"

#include <cmath>

QSK_SYSTEM_STATE( QskBoundedInput, ReadOnly, ( QskAspect::FirstSystemState << 1 ) )

QskBoundedInput::QskBoundedInput( QQuickItem* parent )
    : Inherited( parent )
    , m_stepSize( 0.1 )
    , m_pageSize( 1 )
    , m_snap( false )
{
    setFocusPolicy( Qt::StrongFocus );
    setAcceptedMouseButtons( Qt::LeftButton );
    setWheelEnabled( true );

    if ( isComponentComplete() )
    {
        connect( this, &QskBoundedControl::boundariesChanged,
            this, &QskBoundedInput::alignInput );
    }
}

QskBoundedInput::~QskBoundedInput()
{
}

void QskBoundedInput::setStepSize( qreal stepSize )
{
    if ( qFuzzyIsNull( stepSize ) )
        stepSize = 0.0;

    if ( qFuzzyCompare( m_stepSize, stepSize ) )
        return;

    m_stepSize = stepSize;
    Q_EMIT stepSizeChanged( stepSize );

    if ( isComponentComplete() )
    {
        if ( m_snap && stepSize )
            alignInput();
    }
}

qreal QskBoundedInput::stepSize() const
{
    return m_stepSize;
}

void QskBoundedInput::setPageSize( int pageSize )
{
    if ( m_pageSize == pageSize )
        return;

    m_pageSize = pageSize;
    Q_EMIT pageSizeChanged( pageSize );
}

int QskBoundedInput::pageSize() const
{
    return m_pageSize;
}

void QskBoundedInput::stepUp()
{
    increment( m_stepSize );
}

void QskBoundedInput::stepDown()
{
    increment( -m_stepSize );
}

void QskBoundedInput::pageUp()
{
    increment( m_pageSize * m_stepSize );
}

void QskBoundedInput::pageDown()
{
    increment( -m_pageSize * m_stepSize );
}

void QskBoundedInput::setSnap( bool snap )
{
    if ( m_snap == snap )
        return;

    m_snap = snap;
    Q_EMIT snapChanged( snap );

    if ( isComponentComplete() && snap )
        alignInput();
}

bool QskBoundedInput::snap() const
{
    return m_snap;
}

void QskBoundedInput::componentComplete()
{
    if ( isComponentComplete() )
    {
        connect( this, &QskBoundedControl::boundariesChanged,
            this, &QskBoundedInput::alignInput, Qt::UniqueConnection );
    }

    Inherited::componentComplete();
}

void QskBoundedInput::alignInput()
{
}

qreal QskBoundedInput::alignedValue( qreal value ) const
{
    value = boundedValue( value );

    if ( value > minimum() && value < maximum() )
    {
        if ( m_snap && m_stepSize )
        {
            value = qRound( value / m_stepSize ) * m_stepSize;
            value = boundedValue( value );
        }
    }

    return value;
}

QskIntervalF QskBoundedInput::alignedInterval( const QskIntervalF& interval ) const
{
    if ( m_snap )
    {
        if ( const auto step = m_stepSize )
        {
            const qreal lower = std::floor( interval.lowerBound() / step ) * step;
            const qreal upper = std::ceil( interval.upperBound() / step ) * step;

            return QskIntervalF( lower, upper );
        }
    }

    return interval;
}

void QskBoundedInput::setReadOnly( bool readOnly )
{
    if ( readOnly == isReadOnly() )
        return;

    setSkinStateFlag( ReadOnly, readOnly );

    // we are killing user settings here !!
    setFocusPolicy( readOnly ? Qt::NoFocus : Qt::StrongFocus );
    setAcceptedMouseButtons( readOnly ? Qt::NoButton : Qt::LeftButton );
    setWheelEnabled( !readOnly );

    Q_EMIT readOnlyChanged( readOnly );

    QEvent event( QEvent::ReadOnlyChange );
    QCoreApplication::sendEvent( this, &event );
}

bool QskBoundedInput::isReadOnly() const
{
    return hasSkinState( ReadOnly );
}

qreal QskBoundedInput::incrementForKey( const QKeyEvent* event ) const
{
    switch( event->key() )
    {
        case Qt::Key_Up:
            return m_stepSize;

        case Qt::Key_Down:
            return -m_stepSize;

        case Qt::Key_PageUp:
            return m_pageSize;

        case Qt::Key_PageDown:
            return -m_pageSize;

        default:
        {
            if ( qskIsStandardKeyInput( event, QKeySequence::MoveToNextChar ) )
                return m_stepSize;

            if ( qskIsStandardKeyInput( event, QKeySequence::MoveToPreviousChar ) )
                return -m_stepSize;
        }
    }

    return 0.0;
}

void QskBoundedInput::keyPressEvent( QKeyEvent* event )
{
    if ( !isReadOnly() )
    {
        if ( const auto offset = incrementForKey( event ) )
        {
            increment( offset );
            return;
        }
    }

    Inherited::keyPressEvent( event );
}

#ifndef QT_NO_WHEELEVENT

void QskBoundedInput::wheelEvent( QWheelEvent* event )
{
    if ( isReadOnly() )
    {
        Inherited::wheelEvent( event );
        return;
    }

    auto offset = qskWheelIncrement( event ) * m_stepSize;
    if ( event->modifiers() & ( Qt::ControlModifier | Qt::ShiftModifier ) )
        offset *= m_pageSize;

    increment( offset );
}

#endif

#include "moc_QskBoundedInput.cpp"
