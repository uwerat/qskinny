/******************************************************************************
 * Copyright (C) 2022 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "StorageMeter.h"
#include "CircularProgressBar.h"
#include <QskSkin.h>
#include <QskTextLabel.h>

QSK_SUBCONTROL( StorageMeter, Status )

namespace
{
    inline QString make_text( const QLocale& locale, const qreal value )
    {
        return locale.toString( static_cast< int >( value ) ) + " " + locale.percent();
    }
}

StorageMeter::StorageMeter( QQuickItem* parent ) noexcept
    : CircularProgressBar( parent )
    , label( new QskTextLabel( this ) )
{
    setAutoLayoutChildren( true );
    setSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Constrained );

    label->setText( make_text( locale(), value() ) );
    label->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
    label->setLayoutAlignmentHint( Qt::AlignCenter );
    label->setFontRole( QskSkin::SmallFont );
}

void StorageMeter::setValue( const qreal value )
{
    const auto gradient = gradientHint( StorageMeter::Status );
    const auto color = gradient.extracted( value / 100.0, value / 100.0 ).startColor();
    setGradientHint( StorageMeter::Bar, { color, color.lighter() } );
    CircularProgressBar::setValue( value );
    label->setTextColor( color );
    label->setText( make_text( locale(), value ) );
}

QSizeF StorageMeter::contentsSizeHint( Qt::SizeHint which, const QSizeF& constraint ) const
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
