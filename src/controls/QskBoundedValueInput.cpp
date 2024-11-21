/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskBoundedValueInput.h"
#include "QskFunctions.h"

#include <qlocale.h>
#include <cfloat>

static qreal qskAlignedValue( const QskBoundedValueInput* input, qreal value )
{
    value = input->boundedValue( value );

    if ( value > input->minimum() && value < input->maximum() )
    {
        if ( input->isSnapping() && input->stepSize() )
        {
            const auto step = input->stepSize();

            value = qRound( value / step ) * step;
            value = input->boundedValue( value );
        }
    }

    return value;
}

class QskBoundedValueInput::PrivateData
{
  public:
    qreal value = 0.0;
    int decimals = 2;
};

QskBoundedValueInput::QskBoundedValueInput( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
}

QskBoundedValueInput::~QskBoundedValueInput()
{
}

void QskBoundedValueInput::setDecimals( int decimals )
{
    decimals = qBound( 0, decimals, DBL_MAX_10_EXP + DBL_DIG );
    if ( decimals != m_data->decimals )
    {
        m_data->decimals = decimals;

        update();
        resetImplicitSize();
    }
}

int QskBoundedValueInput::decimals() const
{
    return m_data->decimals;
}

void QskBoundedValueInput::alignInput()
{
    auto value = qskAlignedValue( this, m_data->value );
    value = fixupValue( value );

    setValueInternal( value );
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
    return valueAsRatio( m_data->value );
}

void QskBoundedValueInput::setValue( qreal value )
{
    if ( isComponentComplete() )
    {
        value = qskAlignedValue( this, value );
        value = fixupValue( value );
    }

    setValueInternal( value );
}

qreal QskBoundedValueInput::value() const
{
    return m_data->value;
}

void QskBoundedValueInput::increment( qreal offset )
{
    setValue( m_data->value + offset );
}

void QskBoundedValueInput::setValueInternal( qreal value )
{
    if ( !qskFuzzyCompare( value, m_data->value ) )
    {
        m_data->value = value;
        Q_EMIT valueChanged( value );

        update();
    }
}

QString QskBoundedValueInput::valueText() const
{
    return textFromValue( value() );
}

QString QskBoundedValueInput::textFromValue( qreal value ) const
{
    return locale().toString( value, 'f', m_data->decimals );
}

#include "moc_QskBoundedValueInput.cpp"
