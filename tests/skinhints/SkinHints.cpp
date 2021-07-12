/******************************************************************************
 * QSkinny - Copyright (C) 2021 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include <QskBox.h>
#include <QskBoxBorderColors.h>
#include <QskSkin.h>
#include <QskSkinHintTableEditor.h>

#include <QGuiApplication>
#include <QtTest>

class TestSkinHints : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void setHints();
    void removeHints();

private:
    QskSkin m_skin;
    QskSkinHintTableEditor ed = QskSkinHintTableEditor( &m_skin.hintTable() );
};

void TestSkinHints::setHints()
{
    QskBoxBorderColors colors;
    QVERIFY( colors.colorAt( Qt::TopEdge ) == QColor() );

    ed.setBoxBorderColors( QskBox::Panel, { Qt::red },
                           QskControl::Disabled
                            | QskControl::Hovered
                            | QskControl::Focused
                           );

    colors = ed.boxBorderColors( QskBox::Panel | QskControl::Disabled );
    QVERIFY( colors.colorAt( Qt::TopEdge ) == Qt::red );

    colors = ed.boxBorderColors( QskBox::Panel | QskControl::Hovered );
    QVERIFY( colors.colorAt( Qt::TopEdge ) == Qt::red );

    colors = ed.boxBorderColors( QskBox::Panel | QskControl::Focused );
    QVERIFY( colors.colorAt( Qt::TopEdge ) == Qt::red );

    colors = ed.boxBorderColors( QskBox::Panel | QskControl::Disabled | QskControl::Hovered );
    QVERIFY( colors.colorAt( Qt::TopEdge ) == Qt::red );

    colors = ed.boxBorderColors( QskBox::Panel | QskControl::Disabled | QskControl::Focused );
    QVERIFY( colors.colorAt( Qt::TopEdge ) == Qt::red );

    colors = ed.boxBorderColors( QskBox::Panel | QskControl::Hovered | QskControl::Focused );
    QVERIFY( colors.colorAt( Qt::TopEdge ) == Qt::red );

    colors = ed.boxBorderColors( QskBox::Panel | QskControl::Disabled | QskControl::Hovered | QskControl::Focused );
    QVERIFY( colors.colorAt( Qt::TopEdge ) == Qt::red );
}

void TestSkinHints::removeHints()
{
    bool removed = ed.removeHint( QskBox::Panel | QskAspect::Color,
                                  QskControl::Hovered
                                  | QskControl::Focused
                                  );
    QVERIFY( removed == false );

    ed.setColorHint( QskBox::Panel, QColor( Qt::green ),
                     QskControl::Disabled
                     | QskControl::Hovered
                     | QskControl::Focused
                     );

    removed = ed.removeHint( QskBox::Panel | QskAspect::Color,
                                  QskControl::Disabled
                                  | QskControl::Hovered
                                  | QskControl::Focused
                                  );
    QVERIFY( removed == true );

    removed = ed.removeHint( QskBox::Panel | QskAspect::Color,
                             QskControl::Disabled
                             | QskControl::Hovered
                             | QskControl::Focused
                             );
    QVERIFY( removed == false );

    ed.setColorHint( QskBox::Panel, QColor( Qt::green ),
                     QskControl::Disabled
                     | QskControl::Hovered
                     | QskControl::Focused
                     );

    removed = ed.removeHint( QskBox::Panel | QskAspect::Color,
                             QskControl::Disabled
                             | QskControl::Hovered
                             );
    QVERIFY( removed == true );
}

QTEST_MAIN(TestSkinHints)
#include "SkinHints.moc"
