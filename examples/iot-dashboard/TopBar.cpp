#include "TopBar.h"

#include <QskSkin.h>
#include <QskTextLabel.h>

#include <QTime>

TopBarItem::TopBarItem( const QString& name, QQuickItem* parent ) : QskLinearBox( Qt::Vertical, parent ),
    m_name( name )
{
    setAutoLayoutChildren( true );
    setAutoAddChildren( true );
    setFixedSize( {140, 40} );
    setMargins({0, 0, 0, 0});
    setPadding({30, 0, 30, 0});
    setSpacing(6);

    auto* textLabel = new QskTextLabel( name, this );
    textLabel->setFontRole(QskSkin::SmallFont); // ### style
}

TopBar::TopBar(QQuickItem *parent) : QskLinearBox(Qt::Horizontal, parent)
{
    setAutoLayoutChildren(true);
    setAutoAddChildren(true);
    setSizePolicy(QskSizePolicy::Preferred, QskSizePolicy::Fixed);

    QStringList itemStrings = { "Living Room", "Bedroom", "Bathroom", "Kitchen" };

    for( const auto itemString : itemStrings )
    {
        auto* item = new TopBarItem( itemString, this );
        m_entries.append(item);
    }

    auto* timeControl = new QskLinearBox(Qt::Vertical, this);
    auto* timeTitle = new QskTextLabel("Current time", timeControl); // ### make bold or so
    timeTitle->setFontRole(QskSkin::TinyFont);

    auto now = QTime::currentTime();
    auto timeString = now.toString();
    auto* timeDisplay = new QskTextLabel(timeString, timeControl);
    timeDisplay->setFontRole(QskSkin::HugeFont);
    timeDisplay->setTextColor("#6776FF");
}
