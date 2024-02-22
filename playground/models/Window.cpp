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

#include <QskDataControlMapper.h>

#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>

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

    auto mapper = new QskDataControlMapper();
    mapper->setModel(table);
    /*
    Not needed if we set  USER true for the corresponding Q_PROPERTY in QskBoundedInput and QskTextInput
    maybe could be done also for other controls
    mapper->addMapping(spin, 0,"value");
    mapper->addMapping(txt, 1, "text");
    */
    mapper->addMapping(spin, 0); // needs USER=true for value in QskBoundedInput
    mapper->addMapping(txt, 1); // needs USER=true for text in QskTextInput

    mapper->setCurrentIndex(0);

    auto v = new QskLinearBox(Qt::Vertical);
    v->addSpacer( 0,100 );
    v->addItem( spin );
    v->addItem( txt );
    addItem(v);
    auto h = new QskLinearBox(Qt::Horizontal);
    auto prev = new QskPushButton("<",h);
    auto next = new QskPushButton(">",h);
    connect(prev,&QskPushButton::clicked,[=]() { mapper->setCurrentIndex( 0 ); });
    connect(next,&QskPushButton::clicked,[=]() { mapper->setCurrentIndex( 1 ); });
    v->addItem(h);
    v->addSpacer( 0,100 );
}


#include "moc_Window.cpp"
