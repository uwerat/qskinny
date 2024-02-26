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
            initValue( 0, 0, 1 );
            initValue( 0, 1, "HELLO" );

            initValue( 1, 0, 2 );
            initValue( 1, 1, "WORLD" );
        }

        void dump() const
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

      private:
        void initValue( int row, int col, const QVariant& value )
        {
            setData( index( row, col ), value, Qt::EditRole );
        }

    };

    class View : public QskLinearBox
    {
      public:
        View( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Vertical, parent )
        {
            setPanel( true );

            auto model = new Model( this );

            auto textInput = new QskTextInput();
            auto spinBox = new QskSpinBox( -100.0, +100.0, 1.0 );

            m_binder = new QskModelObjectBinder( model, this );
            m_binder->bindObject( spinBox, 0 );
            m_binder->bindObject( textInput, 1 );

            auto hBox = new QskLinearBox( Qt::Horizontal );
            hBox->setSection( QskAspect::Header );
            hBox->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );

            {
                auto rowButton = new QskPushButton( "Toggle Row", hBox );
                auto counterButton = new QskPushButton( "Invert Counter", hBox );
                auto submitButton = new QskPushButton( "Submit Changes", hBox );

                connect( rowButton, &QskPushButton::clicked,
                    this, &View::toogleRow );

                connect( counterButton, &QskPushButton::clicked,
                    this, &View::invertCounter );

                connect( submitButton, &QskPushButton::clicked,
                    this, &View::updateModel );

                hBox->addStretch( 1 );
            }

            addItem( hBox );
            addSpacer( 5 );
            addItem( spinBox );
            addItem( textInput );
            addStretch( 1 );
        }

      private:
        void toogleRow()
        {
            m_binder->setCurrentRow( m_binder->currentRow() == 0 ? 1 : 0 );
        }

        void updateModel()
        {
            m_binder->submit();

            const auto model = dynamic_cast< const Model* >( m_binder->model() );
            model->dump();
        }

        void invertCounter()
        {
            auto model = m_binder->model();

            const auto index = model->index( m_binder->currentRow(), 0 );

            auto value = model->data( index, Qt::EditRole );
            model->setData( index, -value.toDouble(), Qt::EditRole );
        }

        QskModelObjectBinder* m_binder;
    };
}

Window::Window()
{
    addItem( new View() );
}
