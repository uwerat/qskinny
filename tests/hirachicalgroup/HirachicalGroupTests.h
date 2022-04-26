#pragma once

#include <qobject.h>
#include <QtTest/QtTest>

class QskControl;
class HirachicalGroupTests : public QObject
{
    Q_OBJECT

    QskControl * root;
  private Q_SLOTS:
    void init();
    void cleanup();

    void higherGroupUpdatesLower();
    void lowerGroupUpdatesHigher();
    void addToGroup();
    void addPartlyToGroup();
    void removeFromGroup();
    void groupMemberGetsDeleted();
    void addTwiceToSameGroup();
};

QTEST_MAIN( HirachicalGroupTests )
