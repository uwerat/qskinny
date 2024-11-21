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

class QskBoundedInput::PrivateData
{
  public:
    qreal stepSize = 0.1;
    uint pageSteps = 1;

    bool snapping = false;
};

QskBoundedInput::QskBoundedInput( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
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

    if ( qFuzzyCompare( m_data->stepSize, stepSize ) )
        return;

    m_data->stepSize = stepSize;
    Q_EMIT stepSizeChanged( stepSize );

    if ( isComponentComplete() )
    {
        if ( m_data->snapping && stepSize )
            alignInput();
    }
}

qreal QskBoundedInput::stepSize() const
{
    return m_data->stepSize;
}

void QskBoundedInput::setPageSteps( uint pageSteps )
{
    if ( m_data->pageSteps == pageSteps )
        return;

    m_data->pageSteps = pageSteps;
    Q_EMIT pageStepsChanged( pageSteps );
}

uint QskBoundedInput::pageSteps() const
{
    return m_data->pageSteps;
}

qreal QskBoundedInput::pageSize() const
{
    return m_data->pageSteps * m_data->stepSize;
}

void QskBoundedInput::stepUp()
{
    increment( m_data->stepSize );
}

void QskBoundedInput::stepDown()
{
    increment( -m_data->stepSize );
}

void QskBoundedInput::pageUp()
{
    increment( pageSize() );
}

void QskBoundedInput::pageDown()
{
    increment( -pageSize() );
}

void QskBoundedInput::setSnapping( bool on )
{
    if ( m_data->snapping == on )
        return;

    m_data->snapping = on;
    Q_EMIT snappingChanged( on );

    if ( isComponentComplete() && m_data->snapping )
        alignInput();
}

bool QskBoundedInput::isSnapping() const
{
    return m_data->snapping;
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
            return m_data->stepSize;

        case Qt::Key_Down:
            return -m_data->stepSize;

        case Qt::Key_PageUp:
            return pageSize();

        case Qt::Key_PageDown:
            return -pageSize();

        default:
        {
            if ( qskIsStandardKeyInput( event, QKeySequence::MoveToNextChar ) )
                return m_data->stepSize;

            if ( qskIsStandardKeyInput( event, QKeySequence::MoveToPreviousChar ) )
                return -m_data->stepSize;
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

    auto offset = qskWheelIncrement( event ) * m_data->stepSize;
    if ( event->modifiers() & ( Qt::ControlModifier | Qt::ShiftModifier ) )
        offset *= m_data->pageSteps;

    increment( offset );
}

#endif

#include "moc_QskBoundedInput.cpp"
