#pragma once

#include <qobject.h>
#include <QtTest/QtTest>

class QskControl;
class TristateCheckBox : public QObject
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
};

QTEST_MAIN(TristateCheckBox)
