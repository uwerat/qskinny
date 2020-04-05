/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "AnchorBox.h"

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskControl.h>
#include <QskObjectCounter.h>
#include <QskWindow.h>

#include <QGuiApplication>
#include <QDebug>

class TestRectangle : public QskControl
{
  public:
    TestRectangle( const char* colorName, QQuickItem* parent = nullptr )
        : QskControl( parent)
        , m_colorName( colorName )
    {
        setObjectName( colorName );

        initSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Preferred );

        setMinimumSize( 10, 10 );
        setPreferredSize( 100, 100 );
        setMaximumSize( 1000, 1000 );

        setBackgroundColor( colorName );
    }

  protected:
    void geometryChangeEvent( QskGeometryChangeEvent* event ) override
    {
        QskControl::geometryChangeEvent( event );
#if 0
        qDebug() << m_colorName << size();
#endif
    }

  private:
    QByteArray m_colorName;
};


class MyBox : public AnchorBox
{
  public:
    MyBox( QQuickItem* parent = nullptr )
        : AnchorBox( parent )
    {
        setObjectName( "Box" );
        setup1();
    }

    void setup1();
    void setup2();
    void setup3();

  protected:
    virtual void geometryChangeEvent( QskGeometryChangeEvent* event ) override
    {
        AnchorBox::geometryChangeEvent( event );
    }
};

void MyBox::setup1()
{
    auto a = new TestRectangle( "PaleVioletRed" );
    auto b = new TestRectangle( "DarkSeaGreen" );
    auto c = new TestRectangle( "SkyBlue" );
    auto d = new TestRectangle( "Coral" );
    auto e = new TestRectangle( "NavajoWhite" );
    auto f = new TestRectangle( "Peru" );
    auto g = new TestRectangle( "Olive" );

    addAnchor( a, Qt::AnchorTop, Qt::AnchorTop );
    addAnchor( b, Qt::AnchorTop, Qt::AnchorTop );

    addAnchor( c, Qt::AnchorTop, a, Qt::AnchorBottom );
    addAnchor( c, Qt::AnchorTop, b, Qt::AnchorBottom );
    addAnchor( c, Qt::AnchorBottom, d, Qt::AnchorTop );
    addAnchor( c, Qt::AnchorBottom, e, Qt::AnchorTop );

    addAnchor( d, Qt::AnchorBottom, Qt::AnchorBottom );
    addAnchor( e, Qt::AnchorBottom, Qt::AnchorBottom );

    addAnchor( c, Qt::AnchorTop, f, Qt::AnchorTop );
    addAnchor( c, Qt::AnchorVerticalCenter, f, Qt::AnchorBottom );
    addAnchor( f, Qt::AnchorBottom, g, Qt::AnchorTop );
    addAnchor( c, Qt::AnchorBottom, g, Qt::AnchorBottom );

    // horizontal
    addAnchor( a, Qt::AnchorLeft, Qt::AnchorLeft );
    addAnchor( d, Qt::AnchorLeft, Qt::AnchorLeft );
    addAnchor( a, Qt::AnchorRight, b, Qt::AnchorLeft );

    addAnchor( a, Qt::AnchorRight, c, Qt::AnchorLeft );
    addAnchor( c, Qt::AnchorRight, e, Qt::AnchorLeft );

    addAnchor( b, Qt::AnchorRight, Qt::AnchorRight );
    addAnchor( e, Qt::AnchorRight, Qt::AnchorRight );
    addAnchor( d, Qt::AnchorRight, e, Qt::AnchorLeft );

    addAnchor( f, Qt::AnchorLeft, Qt::AnchorLeft );
    addAnchor( g, Qt::AnchorLeft, Qt::AnchorLeft );
    addAnchor( f, Qt::AnchorRight, g, Qt::AnchorRight );
}

void MyBox::setup2()
{
    auto a = new TestRectangle( "PaleVioletRed" );
    auto b = new TestRectangle( "DarkSeaGreen" );

    addAnchors( a, Qt::TopLeftCorner );
    addAnchors( a, Qt::BottomRightCorner, b, Qt::TopLeftCorner );
    addAnchors( b, Qt::BottomRightCorner );
}

void MyBox::setup3()
{
    auto a = new TestRectangle( "PaleVioletRed" );
    auto b = new TestRectangle( "DarkSeaGreen" );
    auto c = new TestRectangle( "SkyBlue" );
    auto d = new TestRectangle( "Coral" );

    addAnchor( a, Qt::AnchorTop, Qt::AnchorTop );

    addAnchor( a, Qt::AnchorLeft, Qt::AnchorLeft );
    addAnchor( a, Qt::AnchorRight, b, Qt::AnchorLeft );
    addAnchor( b, Qt::AnchorRight, c, Qt::AnchorLeft );
    addAnchor( c, Qt::AnchorRight, d, Qt::AnchorLeft );
    addAnchor( d, Qt::AnchorRight, Qt::AnchorRight );

    auto e = new TestRectangle( "NavajoWhite" );
#if 1
    e->setMinimumWidth( 100 );
#endif
    addAnchor( a, Qt::AnchorBottom, e, Qt::AnchorTop );
    addAnchor( c, Qt::AnchorRight, e, Qt::AnchorRight );
    addAnchor( b, Qt::AnchorLeft, e, Qt::AnchorLeft );
}

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::Quit | SkinnyShortcut::DebugShortcuts );

    auto box = new MyBox();

#if 1
    for ( int i = Qt::MinimumSize; i <= Qt::MaximumSize; i++ )
    {
        const auto which = static_cast< Qt::SizeHint >( i );
        qDebug() << which << box->effectiveSizeHint( which );
    }
#endif

    QskWindow window;
    window.addItem( box );
    window.resize( 600, 600 );
    window.show();

    return app.exec();
}

