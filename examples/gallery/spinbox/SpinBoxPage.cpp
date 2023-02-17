/******************************************************************************
 * Copyright (C) 2023 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SpinBoxPage.h"
#include <QskSpinBox.h>
#include <QskGridBox.h>
#include <QskTextLabel.h>
#include <QskLinearBox.h>

SpinBoxPage::SpinBoxPage( QQuickItem* parent ) : Page( Qt::Horizontal, parent )
{
    setMargins( 10 );
    setSpacing( 20 );

    populate();
}

void SpinBoxPage::populate()
{
  const QMap<Qt::Alignment, QString> layouts =
  {
    { Qt::AlignLeft, QStringLiteral("Qt::AlignLeft") },
    { Qt::AlignHCenter, QStringLiteral("Qt::AlignHCenter") },
    { Qt::AlignRight, QStringLiteral("Qt::AlignRight") },
    { Qt::AlignTop, QStringLiteral("Qt::AlignTop") },
    { Qt::AlignVCenter, QStringLiteral("Qt::AlignVCenter") },
    { Qt::AlignBottom, QStringLiteral("Qt::AlignBottom") },
    { Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("Qt::AlignLeft | Qt::AlignVCenter") },
    { Qt::AlignRight | Qt::AlignVCenter, QStringLiteral("Qt::AlignRight | Qt::AlignVCenter") },
    { Qt::AlignTop | Qt::AlignHCenter, QStringLiteral("Qt::AlignTop | Qt::AlignHCenter") },
    { Qt::AlignBottom | Qt::AlignHCenter, QStringLiteral("Qt::AlignBottom | Qt::AlignHCenter") }
  };

  auto* const grid = new QskGridBox(this);
  constexpr int cols = 5;
  for(const auto& layout : layouts.keys())
  {
    const auto x = grid->elementCount() % cols;
    const auto y = grid->elementCount() / cols;
    auto* const column = new QskLinearBox(Qt::Vertical, grid);
    auto* const label = new QskTextLabel(layouts.value(layout), column); // TODO put label either on top or on the bottom
    auto* const spinbox = new QskSpinBox( column );
    spinbox->setAlignmentHint(QskSpinBox::Layout, layout);
    grid->addItem(column, y, x);
    column->setStretchFactor(label, 1);
    column->setStretchFactor(spinbox, 99);
  }
}
