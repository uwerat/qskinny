#pragma once

#include <qobject.h>
#include <QtTest/QtTest>

class QskControl;
class CheckBoxTests : public QObject
{
    Q_OBJECT

    QskControl * root;
  private Q_SLOTS:
    void init();
    void cleanup();

    void checkbox();
    void click();
    void toggle();
    void tristate();
    void higherGroupUpdatesLower();
    void lowerGroupUpdatesHigher();
    void addToGroup();
    void addPartlyToGroup();
    void removeFromGroup();
    void groupMemberGetsDeleted();
    void addTwiceToSameGroup();
};

QTEST_MAIN(CheckBoxTests)

