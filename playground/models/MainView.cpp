/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "MainView.h"

#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskTextLabel.h>
#include <QskTextField.h>
#include <QskSpinBox.h>
#include <QskSeparator.h>
#include <QskFontRole.h>
#include <QskModelObjectBinder.h>

#include <QStandardItemModel>

namespace
{
    class TitleLabel : public QskTextLabel
    {
      public:
        TitleLabel( const QString& text, QQuickItem* parent = nullptr )
            : QskTextLabel( text, parent )
        {
            setFontRole( QskFontRole::Title );
        }
    };

    class SpinBox : public QskSpinBox
    {
      public:
        SpinBox( QQuickItem* parent = nullptr )
            : QskSpinBox( -100.0, 100.0, 1.0, parent )
        {
            initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
        }
    };

    class Header : public QskLinearBox
    {
        Q_OBJECT

      public:
        Header( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, parent )
        {
            initSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::Fixed );

            setPaddingHint( QskBox::Panel, 5 );

            setPanel( true );

            auto rowButton = new QskPushButton( "Toggle Row" );
            auto submitButton = new QskPushButton( "Submit Changes" );

            connect( rowButton, &QskPushButton::clicked,
                this, &Header::rowClicked );

            connect( submitButton, &QskPushButton::clicked,
                this, &Header::submitClicked );

            addItem( rowButton );
            addItem( submitButton );
            addStretch( 1 );
        }

      Q_SIGNALS:
        void rowClicked();
        void submitClicked();
    };

    class Model : public QStandardItemModel
    {
      public:
        Model( QObject* parent = nullptr )
            : QStandardItemModel( 2, 2, parent )
        {
            initValue( 0, 0, 1.0 );
            initValue( 0, 1, "HELLO" );

            initValue( 1, 0, 2.0 );
            initValue( 1, 1, "WORLD" );
        }

      private:
        void initValue( int row, int col, const QVariant& value )
        {
            setData( index( row, col ), value, Qt::EditRole );
        }
    };

    class DisplayBox : public QskLinearBox
    {
      public:
        DisplayBox( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, parent )
        {
            setMargins( 10, 0, 10, 0 );
            initSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::Fixed );

            addItem( new SpinBox() );
            addItem( new QskTextField() );
        }
    };

    class ModelBox : public QskLinearBox
    {
      public:
        ModelBox( QAbstractItemModel* model )
            : QskLinearBox( Qt::Horizontal, model->columnCount() )
            , m_model( model )
        {
            setMargins( 10, 0, 10, 0 );
            setExtraSpacingAt( Qt::BottomEdge );

            for ( int row = 0; row < model->rowCount(); row++ )
            {
                for ( int col = 0; col < model->columnCount(); col++ )
                {
                    const auto value = model->data(
                        model->index( row, col ), Qt::EditRole );

                    if ( value.userType() == QVariant::Double )
                    {
                        auto spinBox = new SpinBox( this );
                        connect( spinBox, &QskSpinBox::valueChanged,
                            this, &ModelBox::updateModel );
                    }
                    else
                    {
                        auto textField = new QskTextField( this );
                        connect( textField, &QskTextField::textChanged,
                            this, &ModelBox::updateModel );
                    }
                }
            }

            updateDisplay();

            connect( m_model, &Model::dataChanged, this, &ModelBox::updateDisplay );
        }

      private:
        void updateModel()
        {
            if ( auto item = qobject_cast< const QQuickItem* >( sender() ) )
            {
                const int index = indexOf( item );

                const auto modelIndex = m_model->index(
                    index / dimension(), index % dimension() );

                const auto property = item->metaObject()->userProperty();
                m_model->setData( modelIndex, property.read( item ), Qt::EditRole );
            }
        }

        void updateDisplay() const
        {
            for ( int row = 0; row < m_model->rowCount(); row++ )
            {
                for ( int col = 0; col < m_model->columnCount(); col++ )
                {
                    const auto index = m_model->index( row, col );

                    if ( auto item = itemAtIndex( row * dimension() + col ) )
                    {
                        const auto property = item->metaObject()->userProperty();
                        property.write( item, m_model->data( index, Qt::EditRole ) );
                    }
                }
            }
        }

        QPointer< QAbstractItemModel > m_model;
    };
}

MainView::MainView( QQuickItem* parent )
    : QskMainView( parent )
{
    m_binder = new QskModelObjectBinder( new Model( this ), this );

    auto header = new Header();

    auto displayBox = new DisplayBox();
    for ( int i = 0; i < displayBox->elementCount(); i++ )
        m_binder->bindObject( displayBox->itemAtIndex( i ), i );

    auto box = new QskLinearBox( Qt::Vertical );

    box->addItem( new TitleLabel( "Editor:" ) );
    box->addItem( displayBox );
    box->addItem( new QskSeparator() );
    box->addItem( new TitleLabel( "Model:" ) );
    box->addItem( new ModelBox( m_binder->model() ) );

    setHeader( header );
    setBody( box );

    connect( header, &Header::rowClicked,
        this, &MainView::toogleRow );

    connect( header, &Header::submitClicked,
        m_binder, &QskModelObjectBinder::submit );
}

void MainView::toogleRow()
{
    m_binder->setCurrentRow( m_binder->currentRow() == 0 ? 1 : 0 );
}

#include "MainView.moc"
