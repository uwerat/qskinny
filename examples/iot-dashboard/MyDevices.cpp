#include "MyDevices.h"
#include "DaytimeSkin.h"
#include "RoundedIcon.h"

#include <QskGridBox.h>
#include <QskTextLabel.h>

namespace {
    class Device : public QskLinearBox
    {
    public:
        Device(const QString& name, const QskGradient& gradient, QQuickItem* parent)
            : QskLinearBox(Qt::Vertical, parent)
            , m_name(name)
        {
            auto fileName = name.toLower();
            fileName.replace(' ', '-');
            auto* icon = new RoundedIcon(fileName, gradient, this);
            icon->setOpacity(0.15);
            auto* textLabel = new QskTextLabel(name, this);
            textLabel->setAlignment(Qt::AlignHCenter);
        }

    private:
        QString m_name;
    };
}

MyDevices::MyDevices(QQuickItem *parent) : QskLinearBox(Qt::Vertical, parent)
{
    setMargins(17);

    auto* title = new QskTextLabel("My Devices", this);
    title->setFontRole(DaytimeSkin::TitleFont);

    auto* content = new QskGridBox(this);

    QskGradient gradient1(QskGradient::Vertical, "#FF3122", "#FF7D34");
    QskGradient gradient2(QskGradient::Vertical, "#6100FF", "#6776FF");

    auto* lamps = new Device("Lamps", gradient1, content);
    content->addItem(lamps, 0, 0);

    auto* musicSystem = new Device("Music System", gradient2, content);
    content->addItem(musicSystem, 0, 1);

    auto* ac = new Device("AC", gradient1, content);
    content->addItem(ac, 1, 0);

    auto* router = new Device("Router", gradient2, content);
    content->addItem(router, 1, 1);
}
