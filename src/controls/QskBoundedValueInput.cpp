/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoundedValueInput.h"
#include "QskFunctions.h"

QskBoundedValueInput::QskBoundedValueInput( QQuickItem* parent )
    : QskBoundedInput( parent )
{
}

QskBoundedValueInput::~QskBoundedValueInput()
{
}

qreal QskBoundedValueInput::alignedValue( qreal value ) const
{
    if ( snap() )
    {
        if ( const auto step = stepSize() )
            value = qRound( value / step ) * step;
    }

    return qBound( minimum(), value, maximum() );
}

void QskBoundedValueInput::alignInput()
{
    const auto newValue = alignedValue( m_value );

    if ( newValue != m_value )
    {
        m_value = newValue;
        Q_EMIT valueChanged( newValue );
    }
}

qreal QskBoundedValueInput::fixupValue( qreal value ) const
{
    return value;
}

void QskBoundedValueInput::setValueAsRatio( qreal ratio )
{
    ratio = qBound( 0.0, ratio, 1.0 );
    setValue( minimum() + ratio * boundaryLength() );
}

qreal QskBoundedValueInput::valueAsRatio() const 
{
    return ( m_value - minimum() ) / boundaryLength();
}

void QskBoundedValueInput::setValue( qreal value )
{
    if ( isComponentComplete() )
    {
        value = alignedValue( value );
    }

    value = fixupValue( value );

    if ( !qskFuzzyCompare( value, m_value ) )
    {
        m_value = value;
        Q_EMIT valueChanged( value );

        update();
    }
}

qreal QskBoundedValueInput::value() const
{
    return m_value;
}

void QskBoundedValueInput::increment( qreal offset )
{
    setValue( m_value + offset );
}

#include "moc_QskBoundedValueInput.cpp"
