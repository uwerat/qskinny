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

#include <QStandardItemModel>

namespace
{
    class Model : public QStandardItemModel
    {
      public:
        Model( QObject* parent = nullptr )
            : QStandardItemModel( 2, 2, parent )
        {
            setValue( 0, 0, 1 );
            setValue( 0, 1, "HELLO" );

            setValue( 1, 0, 2 );
            setValue( 1, 1, "WORLD" );
        }

        void setValue( int row, int col, const QVariant& value )
        {
            setData( index( row, col ), value, Qt::EditRole );
        }

        void dump()
        {
            qDebug() << "Model";
            for ( int row = 0; row < rowCount(); row++ )
            {
                for ( int col = 0; col < columnCount(); col++ )
                {
                    qDebug() << '\t' << row << col
                        << data( index( row, col ), Qt::EditRole );
                }
            }
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
            [ mapper ]() { mapper->setCurrentRow( 0 ); } );

        connect( next, &QskPushButton::clicked,
            [ mapper ]() { mapper->setCurrentRow( 1 ); } );

        vBox->addItem( hBox );
    }

    // update the current record with the data from the SpinBox and TextInput
    auto save = new QskPushButton( "Save Data to Model", vBox );
    connect( save, &QskPushButton::clicked,
        [ = ]() { mapper->submit(); model->dump(); } );

    // trigger the binder and update the spinbox
    auto set0 = new QskPushButton( "Set Model field to 42", vBox );
    connect( set0, &QskPushButton::clicked,
        [ = ]() { model->setValue( mapper->currentRow(), 0, 42 ); } );

    vBox->addSpacer( 0, 100 );
}
