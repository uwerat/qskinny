/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "DevicesPage.h"

#include <QskTextLabel.h>
#include <QskFontRole.h>

QSK_SUBCONTROL( DevicesPage, Panel )

DevicesPage::DevicesPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    auto* const textLabel = new QskTextLabel( "devices page", this );
    textLabel->setAlignmentHint( QskTextLabel::Text, Qt::AlignCenter );
    textLabel->setFontRole( QskFontRole::Display );
}

#include "moc_DevicesPage.cpp"
