#include "MainWindow.h"

#include "MainContent.h"
#include "MenuBar.h"

#include <QskLinearBox.h>

#include <QtQuickShapes/private/qquickshape_p.h>
#include <QtQuick/private/qquickpath_p.h>

MainWindow::MainWindow() : QskWindow()
{
    setPreferredSize( { 1024, 600 } );
    setTitle( "IOT dashboard" );

    auto* shape = new QQuickShape( contentItem() );
    shape->setSize( { 500, 500 } );
    connect(this, &QQuickWindow::frameSwapped, [shape]() {
        shape->setAsynchronous(!shape->asynchronous());
        qDebug() << "status:" << shape->rendererType() << shape->status();
    });

    auto* shapePath = new QQuickShapePath( shape );
    shapePath->setFillColor( Qt::red );
    shapePath->setStrokeColor( Qt::blue );
    shapePath->setStrokeWidth( 5 );
    shapePath->setStartX( 5 );
    shapePath->setStartY( 5 );

    auto* arc = new QQuickPathArc( shapePath );
    arc->setX( 50 );
    arc->setY( 50 );
    arc->setRadiusX( 100 );
    arc->setRadiusY( 100 );
    arc->setUseLargeArc( true );

    qDebug() << "visible?" << shape->isVisible() << shape->size();

//    m_mainLayout = new QskLinearBox( Qt::Horizontal, contentItem() );
//    m_menuBar = new MenuBar( m_mainLayout );
//    m_mainContent = new MainContent( m_mainLayout );
}
