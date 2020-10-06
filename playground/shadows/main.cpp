/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ShadowedBox.h"

#include <QGuiApplication>
#include <QskObjectCounter.h>
#include <QskWindow.h>
#include <QskLinearBox.h>
#include <QskBoxShapeMetrics.h>
#include <QskRgbValue.h>

class Box : public ShadowedBox
{
  public:
    Box( QQuickItem* parent = nullptr )
        : ShadowedBox( parent )
    {
        Shadow shadow;
        shadow.extent = 10;
        shadow.xOffset = 20.0;
        shadow.yOffset = 20.0;

        setShadow( shadow );
        setShadowColor( Qt::black );
        setGradient( Qt::darkRed );
        setShape( QskBoxShapeMetrics( 5, 10, 15, 20 ) );
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    auto layout = new QskLinearBox();
    layout->setPanel( true );
#if 1
    layout->setGradientHint( QskBox::Panel,
        QskGradient( Qt::Vertical, QskRgb::WhiteSmoke, QskRgb::MistyRose ) );
#else
    layout->setGradientHint( QskBox::Panel, Qt::white );
#endif
    layout->setPadding( 60 );
    (void ) new Box( layout );

    QskWindow window;
    window.setColor( QskRgb::PapayaWhip );
    window.addItem( layout );
    window.resize( 600, 600 );
    window.show();

    return app.exec();
}
