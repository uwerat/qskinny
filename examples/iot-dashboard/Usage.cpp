#include "Usage.h"
#include "DaytimeSkin.h"

#include <QskTextLabel.h>

Usage::Usage( QQuickItem* parent )
    : Box( "Usage", parent )
{
    auto* content = new QskLinearBox( Qt::Vertical, this );

    auto* today = new QskLinearBox( Qt::Horizontal, content );
    auto* todayText = new QskTextLabel( "Usage today", today );
    todayText->setFontRole( QskSkin::SmallFont );
    auto* todaySpacer = new QskTextLabel( "_____", today );
    todaySpacer->setTextColor( "#dddddd" );
    auto* todayValue = new QskTextLabel( "0,5 kwH", today );
    todayValue->setFontRole( QskSkin::SmallFont );

    auto* month = new QskLinearBox( Qt::Horizontal, content );
    auto* monthText = new QskTextLabel( "Usage this month", month );
    monthText->setFontRole( QskSkin::SmallFont );
    auto* monthSpacer = new QskTextLabel( "_____", month );
    monthSpacer->setTextColor( "#dddddd" );
    auto* monthValue = new QskTextLabel( "66 kwH", month );
    monthValue->setFontRole( QskSkin::SmallFont );

    auto* total = new QskLinearBox( Qt::Horizontal, content );
    auto* totalText = new QskTextLabel( "Total working hours", total );
    totalText->setFontRole( QskSkin::SmallFont );
    auto* totalSpacer = new QskTextLabel( "_____", total );
    totalSpacer->setTextColor( "#dddddd" );
    auto* totalValue = new QskTextLabel( "125 hrs", total );
    totalValue->setFontRole( QskSkin::SmallFont );
}
