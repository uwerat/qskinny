#include "MyDevices.h"
#include "DaytimeSkin.h"
#include "RoundedIcon.h"

#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskGridBox.h>
#include <QskTextLabel.h>

#include <QImage>

namespace {
    class Device : public QskLinearBox
    {
    public:
        Device(const QString& name, const QskGradient& gradient, QQuickItem* parent)
            : QskLinearBox(Qt::Vertical, parent)
            , m_name(name)
        {
            setAutoAddChildren(false);

            m_icon = new RoundedIcon(QString(), gradient, this);
            m_icon->setOpacity(0.15);
            addItem(m_icon);

            auto* textLabel = new QskTextLabel(name, this);
            textLabel->setAlignment(Qt::AlignHCenter);
            addItem(textLabel);

            auto fileName = name.toLower();
            fileName.replace(' ', '-');
            fileName = ":/images/" + fileName + ".png";
            QImage image( fileName );
            auto graphic = QskGraphic::fromImage( image );
            m_graphicLabel = new QskGraphicLabel( graphic, this );
        }

    protected:
        void updateLayout() override
        {
            QskLinearBox::updateLayout();

            m_graphicLabel->setSize( {36, 36});
            m_graphicLabel->setPosition( { ( m_icon->width() - m_graphicLabel->width() ) / 2,
                                           ( m_icon->height() - m_graphicLabel->height() ) / 2 } );
        }

    private:
        QString m_name;
        RoundedIcon* m_icon;
        QskGraphicLabel* m_graphicLabel;
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
