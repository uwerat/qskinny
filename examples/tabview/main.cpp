/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskAspect.h>
#include <QskBoxBorderColors.h>
#include <QskFocusIndicator.h>
#include <QskLinearBox.h>
#include <QskObjectCounter.h>
#include <QskPushButton.h>
#include <QskSkin.h>
#include <QskTabButton.h>
#include <QskTabBar.h>
#include <QskTabView.h>
#include <QskTextLabel.h>
#include <QskWindow.h>
#include <QskShortcutMap.h>

#include <QGuiApplication>

class Label : public QskTextLabel
{
  public:
    Label( const QString& text, QQuickItem* parent = nullptr )
        : QskTextLabel( text, parent )
    {
        setTextColor( Qt::darkRed );
        setFontRole( QskSkin::LargeFont );
        setAlignment( Qt::AlignCenter );
    }
};

class TabView : public QskTabView
{
  public:
    TabView( QQuickItem* parent = nullptr )
        : QskTabView( parent )
    {
        for ( int i = 0; i < 10; i++ )
        {
            QString text;
            if ( i == 4 )
                text = QString( "Another Tab" );
            else
                text = QString( "Tab %1" ).arg( i + 1 );

            addTab( text, new Label( text ) );
        }

        buttonAt( 2 )->setEnabled( false );
        setCurrentIndex( 4 );
    }

    void appendTab()
    {
        const auto text = QString( "Tab %1" ).arg( count() + 1 );
        addTab( text, new Label( text ) );
    }

    void removeLastTab()
    {
        if ( count() > 0 )
            removeTab( count() - 1 );
    }

    void rotate()
    {
        const Qsk::Position pos[] = { Qsk::Top, Qsk::Right, Qsk::Bottom, Qsk::Left };

        for ( int i = 0; i < 4; i++ )
        {
            if ( tabPosition() == pos[i] )
            {
                setTabPosition( pos[ ( i + 1 ) % 4 ] );
                break;
            }
        }
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

    auto tabView = new TabView();

    auto rotateButton = new QskPushButton( "Rotate" );
    rotateButton->setFocus( true );
    QObject::connect( rotateButton, &QskPushButton::clicked,
        tabView, &TabView::rotate );

    auto autoFitButton = new QskPushButton( "Fit Tabs" );
    autoFitButton->setCheckable( true );
    QObject::connect( autoFitButton, &QskPushButton::toggled,
        tabView, &QskTabView::setAutoFitTabs );

    auto addButton = new QskPushButton( "Add" );
    QObject::connect( addButton, &QskPushButton::clicked,
        tabView, &TabView::appendTab );

    auto removeButton = new QskPushButton( "Remove" );
    QObject::connect( removeButton, &QskPushButton::clicked,
        tabView, &TabView::removeLastTab );

    auto buttonBox = new QskLinearBox( Qt::Horizontal );
    buttonBox->addItem( rotateButton );
    buttonBox->addItem( autoFitButton );
    buttonBox->addItem( addButton );
    buttonBox->addItem( removeButton );
    buttonBox->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    auto layoutBox = new QskLinearBox( Qt::Vertical );
    layoutBox->setDefaultAlignment( Qt::AlignLeft );
    layoutBox->setMargins( 20 );
    layoutBox->setSpacing( 10 );
    layoutBox->addItem( buttonBox );
    layoutBox->addItem( tabView );

    auto focusIndicator = new QskFocusIndicator();
    focusIndicator->setObjectName( "FocusIndicator" );
    focusIndicator->setBoxBorderColorsHint( QskFocusIndicator::Panel, Qt::red );

    QskWindow window;
    window.resize( 800, 600 );
    window.addItem( layoutBox );
    window.addItem( focusIndicator );

    window.show();

    for ( int i = 0; i < 10; i++ )
    {
        QskShortcutMap::addShortcut( Qt::Key_F1 + i, false,
            [tabView, i] { tabView->removeTab( i ); } );
    }

    return app.exec();
}
