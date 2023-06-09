/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "DialogPage.h"

#include <QskDialog.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskStandardSymbol.h>
#include <QskBoxShapeMetrics.h>

namespace
{
    class Button : public QskPushButton
    {
        Q_OBJECT

      public:
        enum ButtonType
        {
            Message,
            Information,
            Warning,
            Critical,

            Question,
            Selection,

            TypeCount
        };
        Q_ENUM( ButtonType )

        Button( ButtonType type, QQuickItem* parent = nullptr )
            : QskPushButton( parent )
            , m_type( type )
        {
            setShape( 10 );
            initSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Fixed );

            const int index = metaObject()->indexOfEnumerator( "ButtonType" );
            setText( metaObject()->enumerator( index ).key( m_type ) );

            connect( this, &QskPushButton::clicked, this, &Button::showDialog );
        }

      private:
        void showDialog()
        {
            switch( static_cast< int >( m_type ) )
            {
                case Message:
                    qskDialog->message( text(), text(), QskStandardSymbol::Ok );
                    break;

                case Information:
                    qskDialog->information( text(), text() );
                    break;

                case Warning:
                    qskDialog->warning( text(), text() );
                    break;

                case Critical:
                    qskDialog->critical( text(), text() );
                    break;

                case Question:
                    qskDialog->question( text(), text() );
                    break;

                case Selection:
                    qskDialog->select( text(), text(), { "yes", "no", "maybe" } );
                    break;
            }
        }

        const ButtonType m_type;
    };
}

DialogPage::DialogPage( QQuickItem* parent )
    : Page( Qt::Horizontal, parent )
{
    auto box = new QskLinearBox( Qt::Horizontal, 2, this );
    box->setSpacing( 20 );
    box->setExtraSpacingAt( Qt::BottomEdge );

    for ( int i = 0; i < Button::TypeCount; i++ )
        new Button( static_cast< Button::ButtonType >( i ), box );
}

#include "DialogPage.moc"
