/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QGuiApplication>
#include <QFontDatabase>

#include <QskAspect.h>
#include <QskObjectCounter.h>
#include <QskWindow.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskDialog.h>
#include <QskStandardSymbol.h>
#include <QskFocusIndicator.h>

class Button : public QskPushButton
{
public:
    Button( const QString& text, QQuickItem* parent = nullptr ):
        QskPushButton( text, parent )
    {
        setObjectName( text );
        setSizePolicy( QskSizePolicy::MinimumExpanding,
            QskSizePolicy::MinimumExpanding );
    }
};

class ButtonBox : public QskLinearBox
{
public:
    ButtonBox( QQuickItem* parent = nullptr ):
        QskLinearBox( Qt::Horizontal, 2, parent )
    {
        setMargins( 10 );
        setSpacing( 5 );

        Button* messageButton = new Button( "Message", this );
        connect( messageButton, &Button::clicked, this, &ButtonBox::execMessage );

        Button* informationButton = new Button( "Information", this );
        connect( informationButton, &Button::clicked, this, &ButtonBox::execInformation );

        Button* questionButton = new Button( "Question", this );
        connect( questionButton, &Button::clicked, this, &ButtonBox::execQuestion );

        Button* warningButton = new Button( "Warning", this );
        connect( warningButton, &Button::clicked, this, &ButtonBox::execWarning );

        Button* criticalButton = new Button( "Critical", this );
        connect( criticalButton, &Button::clicked, this, &ButtonBox::execCritical );

        Button* selectButton = new Button( "Selection", this );
        connect( selectButton, &Button::clicked, this, &ButtonBox::execSelection );
    }

private:
    void execMessage()
    {
        qDebug() << qskDialog->message( "Message", "Request vector, over.",
            QskStandardSymbol::NoSymbol, QskDialog::Close );
    }

    void execInformation()
    {
        qDebug() << qskDialog->information( "Information",
            "We have clearance, Clarence." );
    }

    void execQuestion()
    {
        qDebug() << qskDialog->question( "Question",
            "Roger, Roger. Do we have a vector, Victor ?" );
    }

    void execWarning()
    {
        qDebug() << qskDialog->warning( "Warning", "We have clearance, Clarence." );
    }

    void execCritical()
    {
        qDebug() << qskDialog->critical( "Critical", "That's Clarence Oveur. Over." );
    }

    void execSelection()
    {
        // of course we all love "The Teens"
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

        qDebug() << qskDialog->select( "Here we go ...", "Hot Hot Hot", entries, 7 );
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    qskDialog->setPolicy( QskDialog::EmbeddedBox );

    ButtonBox* box = new ButtonBox();
    box->itemAtIndex( 0 )->setFocus( true );

    box->setObjectName( "ButtonBox" );

    QskWindow window;
    window.addItem( box );
    window.addItem( new QskFocusIndicator() );

    window.resize( 800, 800 );
    window.show();

    return app.exec();
}
