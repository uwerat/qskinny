#include "TopBar.h"
#include "PieChartPainted.h"

#include <QskSkin.h>
#include <QskTextLabel.h>

#include <QTime>

TopBarItem::TopBarItem(const QString& name, const QColor &textColor, const QGradient& gradient, int progress, int value, QQuickItem* parent ) : QskLinearBox( Qt::Vertical, parent ),
    m_name( name )
{
    setAutoLayoutChildren( true );
    setAutoAddChildren( true );

    auto* textLabel = new QskTextLabel( name, this );
    textLabel->setFontRole(QskSkin::SmallFont); // ### style

    auto* pieChartAndDisplay = new QskLinearBox( Qt::Horizontal, this );
    pieChartAndDisplay->setSpacing(10);
    auto* pieChart = new PieChartPainted(textColor, gradient, progress, value, pieChartAndDisplay);
    auto* display = new QskLinearBox( Qt::Vertical, pieChartAndDisplay );
    display->setSpacing(0);
    display->addSpacer(0, 1);
    auto* displayValue = new QskTextLabel(QString::number(value), display);
    displayValue->setFontRole(QskSkin::MediumFont);
    auto* displayUnit = new QskTextLabel("kwH", display);
    displayUnit->setFontRole(QskSkin::SmallFont);
    display->addSpacer(0, 1);

}

TopBar::TopBar(QQuickItem *parent) : QskLinearBox(Qt::Horizontal, parent)
{
    setAutoLayoutChildren(true);
    setAutoAddChildren(true);
    setSizePolicy(QskSizePolicy::Preferred, QskSizePolicy::Fixed);
    setMargins({25, 30, 25, 0});

    QStringList itemStrings = { "Living Room", "Bedroom", "Bathroom", "Kitchen" };
    QColor textColors[] = {"#ff3122", "#6776ff", "#f99055", "#6776ff"};
    QColor gradientColors[] = {"#FF5C00", "#FF3122",
                               "#6776FF", "#6100FF",
                               "#FFCE50", "#FF3122",
//                               "#00ff00", "#ffffff", // ### remove
                               "#6776FF", "#6100FF"};

    int progressValues[] = {25, 45, 15, 86};
    int values[] = {175, 205, 115, 289};

    for(int a = 0; a < itemStrings.count(); a++)
    {
        QLinearGradient gradient(0, 0, 30, 0); // ### do this properly and dependent on the size
        QGradientStop stop1(0.0, gradientColors[2 * a]);
        QGradientStop stop2(1.0, gradientColors[2 * a + 1]);
        gradient.setStops({stop1, stop2});

        auto* item = new TopBarItem( itemStrings.at(a), textColors[a], gradient, progressValues[a], values[a], this );
        m_entries.append(item);
    }

    auto* timeControl = new QskLinearBox(Qt::Vertical, this);
    timeControl->setMargins({0, 0, 50, 0});
    auto* timeTitle = new QskTextLabel("Current time", timeControl); // ### make bold or so
    timeTitle->setFontRole(QskSkin::TinyFont);

    auto now = QTime::currentTime();
    auto timeString = now.toString();
    auto* timeDisplay = new QskTextLabel(timeString, timeControl);
    timeDisplay->setFontRole(QskSkin::HugeFont);
    timeDisplay->setTextColor("#6776FF");
}
