/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ShadowedBox.h"

#include <QskObjectCounter.h>
#include <QskWindow.h>
#include <QskLinearBox.h>
#include <QskRgbValue.h>

#include <QskGradient.h>
#include <QskShadowMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskBoxBorderColors.h>

#include <SkinnyShortcut.h>
#include <QGuiApplication>

class Box : public ShadowedBox
{
  public:
    Box( QQuickItem* parent = nullptr )
        : ShadowedBox( parent )
    {
        const qreal w = 10;

        QskShadowMetrics shadow;
        //shadow.setOffset( 20.0, 20.0 );
        shadow.setSpreadRadius( w );
        shadow.setBlurRadius( w );

        setShadow( shadow );
        setShadowColor( Qt::black );

        QColor c( Qt::darkRed );
#if 0
        c.setAlpha( 100 );
#endif

        setGradient( c );
        setShape( QskBoxShapeMetrics( 40, 10, 15, 5 ) );

        setBorderWidth( w );
        setBorderColors( Qt::blue );
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

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
