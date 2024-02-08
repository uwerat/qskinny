/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "UsageBox.h"

#include <QskSkin.h>
#include <QskTextLabel.h>

QSK_SUBCONTROL( UsageBox, Separator )

namespace
{
    class UsageLine : public QskLinearBox
    {
      public:
        UsageLine( const QString& info, const QString& value, QQuickItem* parent )
            : QskLinearBox( Qt::Horizontal, parent )
        {
            auto infoLabel = new QskTextLabel( info, this );
            infoLabel->setFontRole( QskSkin::SmallFont );

            auto separator = new QskTextLabel( "_____", this );
            separator->setSubcontrolProxy( QskTextLabel::Text, UsageBox::Separator );

            auto valueLabel = new QskTextLabel( value, this );
            valueLabel->setFontRole( QskSkin::SmallFont );
        }
    };
}

UsageBox::UsageBox( QQuickItem* parent )
    : Box( "Usage", parent )
{
    ( void ) new UsageLine( "Usage today", "0,5 kwH", this );
    ( void ) new UsageLine( "Usage this month", "66 kwH", this );
    ( void ) new UsageLine( "Total working hours", "125 hrs", this );
}
