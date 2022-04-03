#include "main.h"

#include <QskCheckBox.h>

void CheckBoxTests::init() {
    root = new QskControl();
}
void CheckBoxTests::cleanup() {
    delete root;
}

void CheckBoxTests::checkbox() {
    auto t = new QskCheckBox( root );

    QVERIFY( t->isCheckable() );
}

void CheckBoxTests::click() {
    auto t = new QskCheckBox( root );

    QVERIFY( t->isChecked() == false );
    t->click();
    QVERIFY( t->isChecked() );
}

void CheckBoxTests::toggle() {
    auto t = new QskCheckBox( root );

    QVERIFY( t->isChecked() == false );
    t->toggle();
    QVERIFY( t->isChecked() );

    t->toggle();
    QVERIFY( t->isChecked() == false );
}

void CheckBoxTests::triState() {
    auto t = new QskCheckBox( root );

    QVERIFY( t->isChecked() == false );
    QVERIFY( t->isTriState() == false );

    t->setCheckState( Qt::CheckState::PartiallyChecked );

    QVERIFY( t->isChecked() == true );
    QVERIFY( t->isTriState() == true );
}

#include "moc_main.cpp"

