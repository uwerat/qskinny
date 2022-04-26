#include "HirachicalGroupTests.h"

#include <QskCheckBox.h>
#include <QskTristateCheckBox.h>
#include <QskHirachicalGroup.h>

void HirachicalGroupTests::init()
{
    root = new QskControl();
}

void HirachicalGroupTests::cleanup()
{
    delete root;
}

void HirachicalGroupTests::higherGroupUpdatesLower()
{
    auto t = new QskTristateCheckBox( root );
    auto group = new QskHirachicalGroup( t );
    auto c1 = new QskCheckBox( root );
    auto c2 = new QskCheckBox( root );
    auto c3 = new QskCheckBox( root );

    group->add( c1 );
    group->add( c2 );
    group->add( c3 );

    QVERIFY( t->isChecked() == false );
    QVERIFY( c1->isChecked() == false );
    QVERIFY( c2->isChecked() == false );
    QVERIFY( c3->isChecked() == false );

    t->setChecked( true );
    QVERIFY( t->isChecked() );
    QVERIFY( c1->isChecked() );
    QVERIFY( c2->isChecked() );
    QVERIFY( c3->isChecked() );

    t->setChecked( false );
    QVERIFY( t->isChecked() == false );
    QVERIFY( c1->isChecked() == false );
    QVERIFY( c2->isChecked() == false );
    QVERIFY( c3->isChecked() == false );

}

void HirachicalGroupTests::lowerGroupUpdatesHigher()
{
    auto t = new QskTristateCheckBox( root );
    auto group = new QskHirachicalGroup( t );

    auto c1 = new QskCheckBox( root );
    auto c2 = new QskCheckBox( root );

    group->add( c1 );
    group->add( c2 );

    c1->setChecked( true );
    QVERIFY( t->isChecked() );
    QVERIFY( t->checkState() == Qt::CheckState::PartiallyChecked );
    QVERIFY( c1->isChecked() == true );
    QVERIFY( c2->isChecked() == false );

    c2->setChecked( true );
    QVERIFY( t->isChecked() );
    QVERIFY( t->checkState() == Qt::CheckState::Checked );
    QVERIFY( c1->isChecked() == true );
    QVERIFY( c2->isChecked() == true );

    c1->setChecked( false );
    QVERIFY( t->isChecked() );
    QVERIFY( t->checkState() == Qt::CheckState::PartiallyChecked );
    QVERIFY( c1->isChecked() == false );
    QVERIFY( c2->isChecked() == true );

    c2->setChecked( false );

    QVERIFY( t->isChecked() == false );
    QVERIFY( t->checkState() == Qt::CheckState::Unchecked );
    QVERIFY( c1->isChecked() == false );
    QVERIFY( c2->isChecked() == false );
}

void HirachicalGroupTests::addToGroup()
{
    auto t = new QskTristateCheckBox( root );
    auto group = new QskHirachicalGroup( t );

    auto c1 = new QskCheckBox( root );
    auto c2 = new QskCheckBox( root );

    group->add( c1 );
    group->add( c2 );

    t->setChecked( true );
    QVERIFY( t->isChecked() );
    QVERIFY( c1->isChecked() );
    QVERIFY( c2->isChecked() );

    auto c3 = new QskCheckBox( root );
    group->add( c3 );

    ( t->checkState() );


    c3->setChecked( true );
    QVERIFY( t->checkState() == Qt::CheckState::Checked );
    auto t4 = new QskCheckBox( root );
    t4->setChecked( true );
    group->add( t4 );
    QVERIFY( t->checkState() == Qt::CheckState::Checked );
}

void HirachicalGroupTests::addPartlyToGroup() {
    auto t1 = new QskTristateCheckBox( root );
    auto t2 = new QskTristateCheckBox( root );
    auto group1 = new QskHirachicalGroup( t1 );
    auto group2 = new QskHirachicalGroup( t2 );

    auto c1 = new QskCheckBox( root );
    auto c2a = new QskCheckBox( root );
    auto c2b = new QskCheckBox( root );

    group1->add( c1 );
    group1->add( t2 );

    group2->add( c2a );
    group2->add( c2b );

    c2a->setChecked( true );

    QVERIFY( t1->checkState() == Qt::CheckState::PartiallyChecked );
    QVERIFY( t2->checkState() == Qt::CheckState::PartiallyChecked );
}

void HirachicalGroupTests::removeFromGroup()
{
    auto t = new QskTristateCheckBox( root );
    auto group = new QskHirachicalGroup( t );

    auto c1 = new QskCheckBox( root );
    auto c2 = new QskCheckBox( root );

    group->add( c1 );
    group->add( c2 );

    c2->setChecked( true );
    QVERIFY( t->checkState() == Qt::CheckState::PartiallyChecked );

    group->remove( c2 );
    QVERIFY( t->isChecked() == false );
}

void HirachicalGroupTests::groupMemberGetsDeleted()
{
    auto t = new QskTristateCheckBox( root );
    auto group = new QskHirachicalGroup( t );
    auto c1 = new QskCheckBox( root );
    auto c2 = new QskCheckBox( root );

    group->add( c1 );
    group->add( c2 );

    c2->setChecked( true );
    QVERIFY( t->checkState() == Qt::CheckState::PartiallyChecked );

    delete c2;
    QVERIFY( t->isChecked() == false );
}

void HirachicalGroupTests::addTwiceToSameGroup()
{
    auto t = new QskTristateCheckBox( root );
    auto group = new QskHirachicalGroup( t );
    auto c1 = new QskCheckBox( root );
    auto c2 = new QskCheckBox( root );

    group->add( c1 );
    group->add( c1 );
    group->remove( c1 );

    group->add( c2 );

    c2->setChecked( true );

    QVERIFY( t->checkState() == Qt::CheckState::Checked );
}

#include "moc_HirachicalGroupTests.cpp"

