/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "StatisticsPage.h"

#include <QskSkin.h>
#include <QskTextLabel.h>

QSK_SUBCONTROL( StatisticsPage, Panel )

StatisticsPage::StatisticsPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    auto* const textLabel = new QskTextLabel( "statistics page", this );
    textLabel->setAlignmentHint( QskTextLabel::Text, Qt::AlignCenter );
    textLabel->setFontRole( QskSkin::HugeFont );
}

#include "moc_StatisticsPage.cpp"
