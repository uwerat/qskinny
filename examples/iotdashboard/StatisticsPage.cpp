/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "StatisticsPage.h"

#include <QskTextLabel.h>
#include <QskFontRole.h>

QSK_SUBCONTROL( StatisticsPage, Panel )

StatisticsPage::StatisticsPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    auto* const textLabel = new QskTextLabel( "statistics page", this );
    textLabel->setAlignmentHint( QskTextLabel::Text, Qt::AlignCenter );
    textLabel->setFontRole( QskFontRole::Display );
}

#include "moc_StatisticsPage.cpp"
