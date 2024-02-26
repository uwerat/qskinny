/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "Window.h"

#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskTextInput.h>
#include <QskSpinBox.h>

#include <QskModelObjectBinder.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>

namespace
{
    class Model : public QSqlTableModel
    {
      public:
        Model( QObject *parent = nullptr )
            : QSqlTableModel( parent, QSqlDatabase::addDatabase( "QSQLITE" ) )
        {
            auto db = database();

            db.setDatabaseName( ":memory:" );
            db.open();

            QSqlQuery query( db );
            query.exec( "create table test(id integer,value text);" );
            query.exec( "insert into test (id,value) values (1,'HELLO');" );
            query.exec( "insert into test (id,value) values (2,'WORLD');" );

            setTable( "test" );
            select();
        }

        void setValue( int row, const QVariant &value )
        {
            setData( index( row, 0 ), value );
        }
    };
}

Window::Window()
{
    auto model = new Model( this );

    auto textInput = new QskTextInput();
    auto spinBox = new QskSpinBox();

    auto mapper = new QskModelObjectBinder( model, this );

    mapper->bindObject( spinBox, 0 );
    mapper->bindObject( textInput, 1 );

    auto vBox = new QskLinearBox( Qt::Vertical );
    vBox->addSpacer( 0, 100 );
    vBox->addItem( spinBox );
    vBox->addItem( textInput );

    addItem( vBox );

    auto hBox = new QskLinearBox( Qt::Horizontal );

    {
        auto prev = new QskPushButton( "<", hBox );
        auto next = new QskPushButton( ">", hBox);

        connect(prev, &QskPushButton::clicked,
            [ mapper ]() { mapper->setCurrentRow( 0 ); });

        connect( next, &QskPushButton::clicked,
            [ mapper ]() { mapper->setCurrentRow( 1 ); });

        vBox->addItem( hBox );
    }

    // update the current record with the data from the SpinBox and TextInput
    auto save = new QskPushButton( "Save Data to Model", vBox );
    connect( save, &QskPushButton::clicked,
        [=]() { mapper->submit(); qDebug() << model->record( mapper->currentRow() ); });

    // trigger the binder and update the spinbox
    auto set0 = new QskPushButton( "Set Model field to 0", vBox );
    connect( set0, &QskPushButton::clicked,
         [=]() { model->setValue( mapper->currentRow(), 0 ); } );
    vBox->addSpacer( 0,100 );
}
