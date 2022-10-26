/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "EnergyMeter.h"
#include "CircularProgressBar.h"

#include <QskTextLabel.h>
#include <QskSkin.h>

namespace
{
    class ValueLabel : public QskTextLabel
    {
      public:
        ValueLabel( QQuickItem* parent )
            : QskTextLabel( parent )
        {
            initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
            setLayoutAlignmentHint( Qt::AlignCenter );
            setFontRole( QskSkin::SmallFont );
        }

        void setValue( int value )
        {
            setText( locale().toString( value ) + " " + locale().percent() );
        }
    };
}

EnergyMeter::EnergyMeter( const QColor& textColor,
        const QskGradient& gradient, int value, QQuickItem* parent )
    : QskControl( parent )
{
    setAutoLayoutChildren( true );

    auto valueBar = new CircularProgressBar( this );
    valueBar->setGradientHint( CircularProgressBar::Bar, gradient );
    valueBar->setValue( value );

    auto valueLabel = new ValueLabel( this );
    valueLabel->setTextColor( textColor );
    valueLabel->setValue( value );
}

QSizeF EnergyMeter::contentsSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    qreal size;

    if ( constraint.width() > 0 )
    {
        size = constraint.width();
    }
    else if ( constraint.height() > 0 )
    {
        size = constraint.height();
    }
    else
    {
        size = 57;
    }

    return QSizeF( size, size );
}
