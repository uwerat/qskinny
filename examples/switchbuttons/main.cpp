#include <QGuiApplication>

#include <QskLinearBox.h>
#include <QskSwitchButton.h>
#include <QskTextLabel.h>
#include <QskWindow.h>

#include <SkinnyShortcut.h>

int main(int argc, char* argv[])
{
    QGuiApplication app( argc, argv );
    QskWindow window;

    SkinnyShortcut::enable( SkinnyShortcut::Quit |
        SkinnyShortcut::DebugShortcuts );

    auto vbox = new QskLinearBox(Qt::Vertical);
    auto hbox = new QskLinearBox(vbox);
    new QskTextLabel("Disable the boxes: ", hbox);
    auto disabler = new QskSwitchButton(hbox);

    auto targets = new QskLinearBox(Qt::Horizontal, vbox);

    auto target1 = new QskSwitchButton(targets);
    target1->setOrientation(Qt::Vertical);

    auto target2 = new QskSwitchButton(targets);
    target2->setOrientation(Qt::Horizontal);

    auto target3 = new QskSwitchButton(targets);
    target3->setChecked(true);
    target3->setOrientation(Qt::Vertical);

    auto target4 = new QskSwitchButton(targets);
    target4->setChecked(true);
    target4->setOrientation(Qt::Horizontal);

    QObject::connect(disabler, &QskSwitchButton::checkedChanged,
        targets, [target1, target2, target3, target4](bool c){
        target1->setEnabled(!c);
        target2->setEnabled(!c);
        target3->setEnabled(!c);
        target4->setEnabled(!c);
    });

    targets->setExtraSpacingAt(Qt::RightEdge);
    vbox->setExtraSpacingAt(Qt::BottomEdge);

    window.addItem( vbox );
    window.show();

    return app.exec();
}
