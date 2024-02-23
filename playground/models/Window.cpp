/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "Window.h"

#include "QskSpinBox.h"

#include <QskDialogButtonBox.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskTextInput.h>

#include <QskModelObjectBinder.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <iostream>

Window::Window( )
    
{
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    db.open();
    QSqlQuery q(db);
    q.exec("create table test(id integer,value text);");
    q.exec("insert into test (id,value) values (1,'HELLO');");
    q.exec("insert into test (id,value) values (2,'WORLD');");

    auto table = new QSqlTableModel  (nullptr,db);
    table->setTable( "test" );
    table->select();

    auto txt = new QskTextInput();
    auto spin = new QskSpinBox();

    auto mapper = new QskModelObjectBinder();
    mapper->bindModel(table);
    /*
    Not needed if we set  USER true for the corresponding Q_PROPERTY in QskBoundedInput and QskTextInput
    maybe could be done also for other controls
    mapper->bindControl(spin, 0,"value");
    mapper->bindControl(txt, 1, "text");
    */
    mapper->bindObject(spin, 0); // needs USER=true for value in QskBoundedInput
    mapper->bindObject(txt, 1); // needs USER=true for text in QskTextInput

    mapper->selectRow(0);

    auto v = new QskLinearBox(Qt::Vertical);
    v->addSpacer( 0,100 );
    v->addItem( spin );
    v->addItem( txt );
    addItem(v);
    auto h = new QskLinearBox(Qt::Horizontal);
    auto prev = new QskPushButton("<",h);
    auto next = new QskPushButton(">",h);
    connect(prev,&QskPushButton::clicked,[=]() { mapper->selectRow( 0 ); });
    connect(next,&QskPushButton::clicked,[=]() { mapper->selectRow( 1 ); });
    v->addItem(h);

    auto save = new QskPushButton("Save",v);
    connect(save,&QskPushButton::clicked,[=]() {
        mapper->updateModel();
        auto r = table->record(mapper->currentRow());
        qDebug() << r;
    });
    v->addSpacer( 0,100 );
}


#include "moc_Window.cpp"
