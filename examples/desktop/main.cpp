/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyShortcut.h>
#include <SkinnyShapeProvider.h>

#include <QskObjectCounter.h>
#include <QskWindow.h>
#include <QskSubWindow.h>
#include <QskSubWindowArea.h>
#include <QskGraphicLabel.h>
#include <QskGradient.h>
#include <QskAspect.h>

#include <QGuiApplication>

class SubWindow : public QskSubWindow
{
public:
    SubWindow( const QString& graphicSource, QQuickItem* parent = nullptr ):
        QskSubWindow( parent )
    {
        setObjectName( graphicSource );

        setColor( QskSubWindow::Panel | QskAspect::Background, "Wheat" );

        auto label = new QskGraphicLabel( this );
        label->setSource( graphicSource );
        label->setAlignment( Qt::AlignCenter );

        setSizePolicy( QskSizePolicy::MinimumExpanding,
            QskSizePolicy::MinimumExpanding );
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    const char providerId[] = "shapes";
    Qsk::addGraphicProvider( providerId, new SkinnyShapeProvider() );

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QskSubWindowArea* area = new QskSubWindowArea();
    area->setGradient( QskGradient( QskGradient::Diagonal,
        "DarkSlateGray", "LightSlateGray" ) );

    QRectF windowRect( 0, 0, 250, 250 );

    const QStringList shapes = { "ellipse", "ring", "star", "hexagon" };

    const QString url = QString( "image://%1/" ).arg( providerId );

    for ( int i = 0; i < shapes.count(); i++ )
    {
        const qreal off = ( i + 1 ) * 100;

        SubWindow* subWindow = new SubWindow( url + shapes[i], area );
        subWindow->setGeometry( windowRect.translated( off, off ) );
    }

    QskWindow window;

    window.addItem( area );
    window.resize( 800, 800 );
    window.show();

    return app.exec();
}
