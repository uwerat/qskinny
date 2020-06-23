#include "TopBar.h"
#include "PieChartPainted.h"

#include <QskSkin.h>
#include <QskTextLabel.h>

#include <QTime>

TopBarItem::TopBarItem(const QString& name, const QColor &color, int progress, int value, QQuickItem* parent ) : QskLinearBox( Qt::Vertical, parent ),
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

    auto* pieChart = new PieChartPainted(color, progress, value, this);
}

TopBar::TopBar(QQuickItem *parent) : QskLinearBox(Qt::Horizontal, parent)
{
    setAutoLayoutChildren(true);
    setAutoAddChildren(true);
    setSizePolicy(QskSizePolicy::Preferred, QskSizePolicy::Fixed);
    setFixedHeight(100);

    QStringList itemStrings = { "Living Room", "Bedroom", "Bathroom", "Kitchen" };
    QColor colors[] = {"#ff3122", "#6776ff", "#f99055", "#6776ff"};
    int progressValues[] = {25, 45, 15, 86};
    int values[] = {175, 205, 115, 289};

    for(int a = 0; a < itemStrings.count(); a++)
    {
        auto* item = new TopBarItem( itemStrings.at(a), colors[a], progressValues[a], values[a], this );
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
