/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "UsageBox.h"
#include "Skin.h"

#include <QskTextLabel.h>

QSK_SUBCONTROL( UsageBox, Separator )

namespace
{
    class SeparatorLabel : public QskTextLabel
    {
      public:

        SeparatorLabel( QQuickItem* parent = nullptr )
            : QskTextLabel( "_____", parent )
        {
            setSubcontrolProxy( QskTextLabel::Text, UsageBox::Separator );
        }
    };
}

UsageBox::UsageBox( QQuickItem* parent )
    : Box( "Usage", parent )
{
    auto* content = new QskLinearBox( Qt::Vertical, this );

    auto* today = new QskLinearBox( Qt::Horizontal, content );
    auto* todayText = new QskTextLabel( "Usage today", today );
    todayText->setFontRole( QskSkin::SmallFont );

    new SeparatorLabel( today );

    auto* todayValue = new QskTextLabel( "0,5 kwH", today );
    todayValue->setFontRole( QskSkin::SmallFont );

    auto* month = new QskLinearBox( Qt::Horizontal, content );
    auto* monthText = new QskTextLabel( "Usage this month", month );
    monthText->setFontRole( QskSkin::SmallFont );

    new SeparatorLabel( month );

    auto* monthValue = new QskTextLabel( "66 kwH", month );
    monthValue->setFontRole( QskSkin::SmallFont );

    auto* total = new QskLinearBox( Qt::Horizontal, content );
    auto* totalText = new QskTextLabel( "Total working hours", total );
    totalText->setFontRole( QskSkin::SmallFont );

    new SeparatorLabel( total );

    auto* totalValue = new QskTextLabel( "125 hrs", total );
    totalValue->setFontRole( QskSkin::SmallFont );
}
