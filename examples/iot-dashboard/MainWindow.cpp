#include "MainWindow.h"
#include "MenuBar.h"

#include <QskLinearBox.h>

MainWindow::MainWindow() : QskWindow()
{
    setFixedSize( { 1024, 600 } );
    setTitle( "IOT dashboard" );

    m_mainLayout = new QskLinearBox( Qt::Horizontal, contentItem() );

    addMenuBar();
    addMainContent();
}

void MainWindow::addMenuBar()
{
    auto* menuBar = new MenuBar( m_mainLayout );
}

void MainWindow::addMainContent()
{

}
