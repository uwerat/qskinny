/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "DialogPage.h"

#include <QskDialog.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>

#include <QDir>

#if QT_CONFIG(thread)
    /*
        WebAssembly without asyncify support does not allow recursive
        event loops. As we did not implement QskDialog::message and friends
        with callbacks yet ( TODO ) we do have some dummy code here to avoid
        crashes.
     */
    #define QSK_USE_EXEC
#endif

#ifndef QSK_USE_EXEC

#include <QskMessageSubWindow.h>
#include <QskSelectionSubWindow.h>
#include <qquickwindow.h>

namespace
{
    void openMessageSubWindow( QQuickWindow* window, const QString& title,
        const QString& text, QskDialog::Actions actions, QskDialog::Action defaultAction )
    {
        auto subWindow = new QskMessageSubWindow( window->contentItem() );
        subWindow->setPopupFlag( QskPopup::DeleteOnClose );
        subWindow->setModal( true );

        subWindow->setTitle( title );
        subWindow->setDialogActions( actions );
        subWindow->setDefaultDialogAction( defaultAction );

        subWindow->setText( text );
        ( void ) subWindow->open();
    }

    void openSelectSubWindow( QQuickWindow* window, const QString& title,
        QskDialog::Actions actions, QskDialog::Action defaultAction,
        const QStringList& entries, int selectedRow )
    {
        auto subWindow = new QskSelectionSubWindow( window->contentItem() );
        subWindow->setPopupFlag( QskPopup::DeleteOnClose );
        subWindow->setModal( true );

        subWindow->setTitle( title );
        subWindow->setDialogActions( actions );
        subWindow->setDefaultDialogAction( defaultAction );

        subWindow->setEntries( entries );
        subWindow->setSelectedRow( selectedRow );

        ( void ) subWindow->open();
    }
}

#endif

namespace
{
    class Button : public QskPushButton
    {
      public:
        Button( const QString& text, QQuickItem* parent = nullptr )
            : QskPushButton( text, parent )
        {
            setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
        }
    };

    class ButtonBox : public QskLinearBox
    {
      public:
        ButtonBox( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Vertical, parent )
        {
            setObjectName( "ButtonBox" );
            setDefaultAlignment( Qt::AlignCenter );

            setMargins( 10 );
            setSpacing( 20 );

            auto messageButton = new Button( "Message", this );
            connect( messageButton, &Button::clicked, this, &ButtonBox::execMessage );

            auto questionButton = new Button( "Question", this );
            connect( questionButton, &Button::clicked, this, &ButtonBox::execQuestion );

            auto selectButton = new Button( "Selection", this );
            connect( selectButton, &Button::clicked, this, &ButtonBox::execSelection );

            auto fileSelectionButton = new Button( "File selection", this );
            connect( fileSelectionButton, &Button::clicked, this, &ButtonBox::execFileSelection );

            setExtraSpacingAt( Qt::BottomEdge );
        }

      private:
        void execMessage()
        {
            const QString title( "Message" );
            const QString message( "Request vector, over." );
#ifndef QSK_USE_EXEC
            openMessageSubWindow( window(), title, message,
                QskDialog::Ok, QskDialog::Ok );
#else
            qskDialog->information( title, message );
#endif
        }

        void execQuestion()
        {
            const QString title( "Question" );
            const QString question( "Roger, Roger. Do we have a vector, Victor ?" );
#ifndef QSK_USE_EXEC
            openMessageSubWindow( window(), title, question,
                QskDialog::Yes | QskDialog::No, QskDialog::Yes );
#else
            (void )qskDialog->question( title, question );
#endif
        }

        void execSelection()
        {
            const QString title( "The Teens" );

            const QStringList entries =
            {
                "Give Me More",
                "Gimme Gimme Your Love",
                "1-2-3-4 Red Light",
                "New York",
                "If You Walk Away",
                "Eloise",
                "On The Radio",
                "We Are The Teens",
                "Never Gonna Tell No Lie To You",
                "Walking On Sunshine ",
                "Get Out Of My Mind",
                "Cover Girl ",
                "Here I Stand",
                "Gypsy Caravan",
                "It´s Good To Have A Friend",
                "We´ll Have A Party Tonite ´nite",
                "Automatic World",
                "Gimme Gimme Gimme Gimme Gimme Your Love"
            };

#ifndef QSK_USE_EXEC
            openSelectSubWindow( window(), title,
                QskDialog::Ok | QskDialog::Cancel, QskDialog::Ok, entries, 7 );
#else
            (void )qskDialog->select( title, entries, 7 );
#endif
        }

        void execFileSelection()
        {
#ifndef QSK_USE_EXEC
            // not implemented for now (class is not public)
#else
            ( void ) qskDialog->selectFile( "select file", QDir::currentPath() );
#endif
        }
    };
}

DialogPage::DialogPage( QQuickItem* parent )
    : Page( Qt::Horizontal, parent )
{
    ( void ) new ButtonBox( this );
}
