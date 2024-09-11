/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "MembersPage.h"

#include <QskSkin.h>
#include <QskTextLabel.h>
#include <QskFontRole.h>

QSK_SUBCONTROL( MembersPage, Panel )

MembersPage::MembersPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    auto* const textLabel = new QskTextLabel( "members page", this );
    textLabel->setAlignmentHint( QskTextLabel::Text, Qt::AlignCenter );
    textLabel->setFontRole( QskFontRole::Display );
}

#include "moc_MembersPage.cpp"
