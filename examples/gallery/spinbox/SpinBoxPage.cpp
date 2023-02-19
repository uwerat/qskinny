/******************************************************************************
 * Copyright (C) 2023 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SpinBoxPage.h"
#include <QskGridBox.h>
#include <QskLinearBox.h>
#include <QskSlider.h>
#include <QskSpinBox.h>
#include <QskTextLabel.h>

SpinBoxPage::SpinBoxPage( QQuickItem* parent )
    : Page( Qt::Horizontal, parent )
{
    setMargins( 10 );
    setSpacing( 20 );

    populate();
}

void SpinBoxPage::populate()
{
    const QMap< Qt::Alignment, QString > layouts = { { Qt::AlignLeft,
                                                         QStringLiteral( "Qt::AlignLeft" ) },
        { Qt::AlignHCenter, QStringLiteral( "Qt::AlignHCenter" ) },
        { Qt::AlignRight, QStringLiteral( "Qt::AlignRight" ) },
        { Qt::AlignTop, QStringLiteral( "Qt::AlignTop" ) },
        { Qt::AlignVCenter, QStringLiteral( "Qt::AlignVCenter" ) },
        { Qt::AlignBottom, QStringLiteral( "Qt::AlignBottom" ) },
        { Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral( "Qt::AlignLeft | Qt::AlignVCenter" ) },
        { Qt::AlignRight | Qt::AlignVCenter,
            QStringLiteral( "Qt::AlignRight | Qt::AlignVCenter" ) },
        { Qt::AlignTop | Qt::AlignHCenter, QStringLiteral( "Qt::AlignTop | Qt::AlignHCenter" ) },
        { Qt::AlignBottom | Qt::AlignHCenter,
            QStringLiteral( "Qt::AlignBottom | Qt::AlignHCenter" ) } };

    auto* const grid = new QskGridBox( this );
    constexpr int cols = 5;

    QVector< QskSpinBox* > spinboxes;
    for ( const auto& layout : layouts.keys() )
    {
        const auto x = grid->elementCount() % cols;
        const auto y = grid->elementCount() / cols;
        auto* const column = new QskLinearBox( Qt::Vertical, grid );
        auto* const label = new QskTextLabel( layouts.value( layout ), column );
        auto* const spinbox = new QskSpinBox( column );
        spinbox->setAlignmentHint( QskSpinBox::Layout, layout );
        grid->addItem( column, y, x );
        column->setStretchFactor( label, 1 );
        column->setStretchFactor( spinbox, 99 );
        spinboxes << spinbox;
    }

    const auto strutInc = spinboxes[ 0 ]->strutSizeHint( QskSpinBox::IncrementPanel );
    const auto strutDec = spinboxes[ 0 ]->strutSizeHint( QskSpinBox::DecrementPanel );

    auto* const columnIncW = new QskLinearBox( Qt::Vertical, this );
    auto* const sliderIncW = new QskSlider( Qt::Vertical, columnIncW );
    auto* const labelsIncW = new QskTextLabel( "+W", columnIncW );

    auto* const columnIncH = new QskLinearBox( Qt::Vertical, this );
    auto* const sliderIncH = new QskSlider( Qt::Vertical, columnIncH );
    auto* const labelsIncH = new QskTextLabel( "+H", columnIncH );

    auto* const columnDecW = new QskLinearBox( Qt::Vertical, this );
    auto* const sliderDecW = new QskSlider( Qt::Vertical, columnDecW );
    auto* const labelsDecW = new QskTextLabel( "-W", columnDecW );

    auto* const columnDecH = new QskLinearBox( Qt::Vertical, this );
    auto* const sliderDecH = new QskSlider( Qt::Vertical, columnDecH );
    auto* const labelsDecH = new QskTextLabel( "-H", columnDecH );

    setStretchFactor( columnIncW, 1 );
    setStretchFactor( columnIncH, 1 );
    setStretchFactor( columnDecW, 1 );
    setStretchFactor( columnDecH, 1 );
    setStretchFactor( grid, 99 );

    sliderIncW->setBoundaries( 2, strutInc.width() * 4 );
    sliderIncH->setBoundaries( 2, strutInc.height() * 4 );
    sliderDecW->setBoundaries( 2, strutDec.width() * 4 );
    sliderDecH->setBoundaries( 2, strutDec.height() * 4 );

    sliderIncW->setValue( strutInc.width() );
    sliderIncH->setValue( strutInc.height() );
    sliderDecW->setValue( strutDec.width() );
    sliderDecH->setValue( strutDec.height() );

    auto update = [ spinboxes, sliderIncW, sliderIncH, sliderDecW, sliderDecH ]( qreal v ) {
        const auto incSize = QSizeF{ sliderIncW->value(), sliderIncH->value() };
        const auto decSize = QSizeF{ sliderDecW->value(), sliderDecH->value() };
        for ( auto* spinbox : spinboxes )
        {
            spinbox->setStrutSizeHint( QskSpinBox::IncrementPanel, incSize );
            spinbox->setStrutSizeHint( QskSpinBox::DecrementPanel, decSize );
        }
    };

    connect( sliderIncW, &QskSlider::valueChanged, this, update );
    connect( sliderIncH, &QskSlider::valueChanged, this, update );
    connect( sliderDecW, &QskSlider::valueChanged, this, update );
    connect( sliderDecH, &QskSlider::valueChanged, this, update );
}
