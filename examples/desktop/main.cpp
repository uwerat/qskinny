/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyShapeProvider.h>
#include <SkinnyShortcut.h>

#include <QskAspect.h>
#include <QskGradient.h>
#include <QskGraphicLabel.h>
#include <QskObjectCounter.h>
#include <QskShortcutMap.h>
#include <QskSubWindow.h>
#include <QskSubWindowArea.h>
#include <QskWindow.h>

#include <QDebug>
#include <QGuiApplication>
#include <QKeySequence>

class SubWindow : public QskSubWindow
{
  public:
    SubWindow( const QString& iconSource, QQuickItem* parent = nullptr )
        : QskSubWindow( parent )
    {
        setObjectName( iconSource );

        const QUrl url( iconSource );

        setWindowTitle( url.fileName() );
        setWindowIconSource( url );

        auto label = new QskGraphicLabel( this );
        label->setSource( iconSource );
        label->setAlignment( Qt::AlignCenter );

        setSizePolicy( QskSizePolicy::MinimumExpanding,
            QskSizePolicy::MinimumExpanding );

        QskShortcutMap::addShortcut( this, QKeySequence( Qt::Key_P ), true,
            [ iconSource ] { qDebug() << iconSource; } );
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
    area->setGradientHint( QskSubWindowArea::Panel,
        QskGradient( QskGradient::Diagonal, "DarkSlateGray", "LightSlateGray" ) );

    QRectF windowRect( 0, 0, 250, 250 );

    const QStringList shapes = { "ellipse", "ring", "star", "hexagon" };

    const QString url = QString( "image://%1/" ).arg( providerId );

    for ( int i = 0; i < shapes.count(); i++ )
    {
        const qreal off = ( i + 1 ) * 100;

        auto subWindow = new SubWindow( url + shapes[ i ], area );
        subWindow->setGeometry( windowRect.translated( off, off ) );
        subWindow->open();
    }

    QskWindow window;

    window.addItem( area );
    window.resize( 800, 800 );
    window.show();

    return app.exec();
}
