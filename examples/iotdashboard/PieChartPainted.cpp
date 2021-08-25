/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "PieChartPainted.h"
#include "CircularProgressBar.h"

#include <QskAnimator.h>
#include <QskBox.h>
#include <QskRgbValue.h>
#include <QskSetup.h>
#include <QskSkin.h>
#include <QskTextLabel.h>
#include <QskFunctions.h>

namespace
{
    class ProgressLabel : public QskTextLabel
    {
      public:
        ProgressLabel( QQuickItem* parent )
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

PieChartPainted::PieChartPainted( const QColor& textColor, const QskGradient& gradient,
        int progress, QQuickItem* parent )
    : QskControl( parent )
{
    setAutoLayoutChildren( true );

    auto progressBar = new CircularProgressBar( this );
    progressBar->setGradientHint( CircularProgressBar::Bar, gradient );
    progressBar->setValue( progress );

    auto progressLabel = new ProgressLabel( this );
    progressLabel->setTextColor( textColor );
    progressLabel->setValue( progress );
}

QSizeF PieChartPainted::contentsSizeHint(
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
