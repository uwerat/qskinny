#include "TristateCheckBox.h"

#include <QskTristateCheckBox.h>

void TristateCheckBox::init()
{
    root = new QskControl();
}

void TristateCheckBox::cleanup()
{
    delete root;
}

void TristateCheckBox::checkbox()
{
    auto t = new QskTristateCheckBox( root );

    QVERIFY( t->isCheckable() );
}

void TristateCheckBox::click()
{
    auto t = new QskTristateCheckBox( root );

    QVERIFY( t->isChecked() == false );
    t->click();
    QVERIFY( t->isChecked() );
}

void TristateCheckBox::toggle()
{
    auto t = new QskTristateCheckBox( root );

    QVERIFY( t->isChecked() == false );
    t->toggle();
    QVERIFY( t->isChecked() );

    t->toggle();
    QVERIFY( t->isChecked() == false );
}

void TristateCheckBox::tristate()
{
    auto t = new QskTristateCheckBox( root );

    QVERIFY( t->isChecked() == false );

    t->setCheckState( Qt::CheckState::PartiallyChecked );

    QVERIFY( t->isChecked() == true );
}

#include "moc_TristateCheckBox.cpp"

