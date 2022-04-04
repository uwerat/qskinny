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

void CheckBoxTests::higherGroupUpdatesLower() {
    auto t = new QskCheckBox( root );
    auto t1 = new QskCheckBox( root );
    auto t2 = new QskCheckBox( root );
    auto t3 = new QskCheckBox( root );

    t->addToGroup( t1 );
    t->addToGroup( t2 );
    t->addToGroup( t3 );

    QVERIFY( t->isChecked() == false );
    QVERIFY( t1->isChecked() == false );
    QVERIFY( t2->isChecked() == false );
    QVERIFY( t3->isChecked() == false );

    t->setChecked( true );
    QVERIFY( t->isChecked() );
    QVERIFY( t1->isChecked() );
    QVERIFY( t2->isChecked() );
    QVERIFY( t3->isChecked() );

    t->setChecked( false );
    QVERIFY( t->isChecked() == false );
    QVERIFY( t1->isChecked() == false );
    QVERIFY( t2->isChecked() == false );
    QVERIFY( t3->isChecked() == false );

}
void CheckBoxTests::lowerGroupUpdatesHigher() {
    auto t = new QskCheckBox( root );

    auto t1 = new QskCheckBox( root );
    auto t2 = new QskCheckBox( root );

    t->addToGroup( t1 );
    t->addToGroup( t2 );

    t1->setChecked( true );
    QVERIFY( t->isChecked() );
    QVERIFY( t->isTriState() );
    QVERIFY( t->checkState() == Qt::CheckState::PartiallyChecked );
    QVERIFY( t1->isChecked() == true );
    QVERIFY( t2->isChecked() == false );


    t2->setChecked( true );
    QVERIFY( t->isChecked() );
    QVERIFY( t->isTriState() );
    QVERIFY( t->checkState() == Qt::CheckState::Checked );
    QVERIFY( t1->isChecked() == true );
    QVERIFY( t2->isChecked() == true );

    t1->setChecked( false );
    QVERIFY( t->isChecked() );
    QVERIFY( t->isTriState() );
    QVERIFY( t->checkState() == Qt::CheckState::PartiallyChecked );
    QVERIFY( t1->isChecked() == false );
    QVERIFY( t2->isChecked() == true );

    t2->setChecked( false );
    QVERIFY( t->isChecked() == false );
    QVERIFY( t->isTriState() );
    QVERIFY( t->checkState() == Qt::CheckState::Unchecked );
    QVERIFY( t1->isChecked() == false );
    QVERIFY( t2->isChecked() == false );
}

void CheckBoxTests::addToGroup() {
    auto t = new QskCheckBox( root );

    auto t1 = new QskCheckBox( root );
    auto t2 = new QskCheckBox( root );

    t->addToGroup( t1 );
    t->addToGroup( t2 );

    t->setChecked( true );
    QVERIFY( t->isChecked() );
    QVERIFY( t1->isChecked() );
    QVERIFY( t2->isChecked() );

    auto t3 = new QskCheckBox( root );
    t->addToGroup( t3 );

    QVERIFY( t->checkState() == Qt::CheckState::PartiallyChecked );

    t3->setChecked( true );
    QVERIFY( t->checkState() == Qt::CheckState::Checked );
    auto t4 = new QskCheckBox( root );
    t4->setChecked( true );
    t->addToGroup( t4 );
    QVERIFY( t->checkState() == Qt::CheckState::Checked );
}

void CheckBoxTests::addPartlyToGroup() {
    auto t = new QskCheckBox( root );

    auto t1 = new QskCheckBox( root );
    auto t1a = new QskCheckBox( root );
    auto t1b = new QskCheckBox( root );

    t1->addToGroup( t1a );
    t1->addToGroup( t1b );

    t1a->setChecked( true );

    QVERIFY( t1->checkState() == Qt::CheckState::PartiallyChecked );
    t->addToGroup( t1 );
    QVERIFY( t1->checkState() == Qt::CheckState::PartiallyChecked );
}


void CheckBoxTests::removeFromGroup() {
    auto t = new QskCheckBox( root );

    auto t1 = new QskCheckBox( root );
    auto t2 = new QskCheckBox( root );

    t->addToGroup( t1 );
    t->addToGroup( t2 );

    t2->setChecked( true );
    QVERIFY( t->checkState() == Qt::CheckState::PartiallyChecked );

    t->removeFromGroup( t2 );
    QVERIFY( t->isChecked() == false );
}

void CheckBoxTests::groupMemberGetsDeleted() {
    auto t = new QskCheckBox( root );
    auto t1 = new QskCheckBox( root );
    auto t2 = new QskCheckBox( root );

    t->addToGroup( t1 );
    t->addToGroup( t2 );

    t2->setChecked( true );
    QVERIFY( t->checkState() == Qt::CheckState::PartiallyChecked );

    delete t2;
    QVERIFY( t->isChecked() == false );
}

void CheckBoxTests::addTwiceToSameGroup() {
    auto t = new QskCheckBox( root );

    auto t1 = new QskCheckBox( root );
    auto t2 = new QskCheckBox( root );

    t->addToGroup( t1 );
    t->addToGroup( t1 );
    t->removeFromGroup( t1 );

    t->addToGroup( t2 );

    t2->setChecked( true );

    QVERIFY( t->checkState() == Qt::CheckState::Checked );
}
#include "moc_main.cpp"

