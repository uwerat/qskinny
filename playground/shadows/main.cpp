/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ShadowedBox.h"

#include <QskObjectCounter.h>
#include <QskWindow.h>
#include <QskGridBox.h>
#include <QskSlider.h>
#include <QskTextLabel.h>
#include <QskRgbValue.h>

#include <SkinnyShortcut.h>
#include <QGuiApplication>
#include <QFontMetrics>

class BoxPanel : public QskBox
{
  public:
    BoxPanel( QQuickItem* parent = nullptr )
        : QskBox( parent )
    {
        setAutoLayoutChildren( true );
        setPadding( 60 );

        setPanel( true );
        setGradientHint( QskBox::Panel, QGradient::SnowAgain );
    }
};

class Slider : public QskSlider
{
  public:
    Slider( qreal min, qreal max, qreal step, qreal value, QQuickItem* parent = nullptr )
        : QskSlider( parent )
    {
        setBoundaries( min, max );
        setStepSize( step );
        setSnap( true );
        setValue( value );
    }
};

class ValueLabel : public QskTextLabel
{
  public:
    ValueLabel( QQuickItem* parent = nullptr )
        : QskTextLabel( parent )
    {
        setFixedWidth( QFontMetrics( font() ).horizontalAdvance( "-100" ) );
        setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    }

    void setValue( qreal value )
    {
        setText( QString::number( value ) );
    }
};

class GridBox : public QskGridBox
{
  public:
    GridBox( QQuickItem* parent = nullptr )
        : QskGridBox( parent )
    {
        setMargins( 5 );
        setColumnStretchFactor( 1, 1 );

        auto sliderX = new Slider( -50, 50, 1, 10 );
        auto sliderY = new Slider( -50, 50, 1, 10 );
        auto sliderSpread = new Slider( 0, 50, 1, 0 );
        auto sliderBlur = new Slider( 0, 50, 1, 10 );
        auto sliderOpacity = new Slider( 0, 1, 0.01, 1 );

        auto panel = new BoxPanel();

        int row = 0;

        addSlider( row++, "Offset X", sliderX );
        addSlider( row++, "Offset Y", sliderY );
        addSlider( row++, "Spread Radius", sliderSpread );
        addSlider( row++, "Blur Radius", sliderBlur );
        addSlider( row++, "Opacity", sliderOpacity );

        addItem( panel, row, 0, -1, -1 );

        auto box = new ShadowedBox( panel );

        box->setOffsetX( sliderX->value() );
        box->setOffsetY( sliderY->value() );
        box->setSpreadRadius( sliderSpread->value() );
        box->setBlurRadius( sliderBlur->value() );
        box->setOpacity( sliderOpacity->value() );

        connect( sliderX, &QskSlider::valueChanged,
            box, &ShadowedBox::setOffsetX );

        connect( sliderY, &QskSlider::valueChanged,
            box, &ShadowedBox::setOffsetY );

        connect( sliderSpread, &QskSlider::valueChanged,
            box, &ShadowedBox::setSpreadRadius );

        connect( sliderBlur, &QskSlider::valueChanged,
            box, &ShadowedBox::setBlurRadius );

        connect( sliderOpacity, &QskSlider::valueChanged,
            box, &ShadowedBox::setOpacity );
    }

  private:
    void addSlider( int row, const QString& text, QskSlider* slider )
    {
        addItem( new QskTextLabel( text ), row, 0 );
        addItem( slider, row, 1 );

        auto label = new ValueLabel();
        label->setValue( slider->value() );

        addItem( label, row, 2 );

        connect( slider, &QskSlider::valueChanged,
            label, [label]( qreal value ) { label->setText( QString::number( value ) ); } ); 
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QskWindow window;
    window.addItem( new GridBox() );
    window.resize( 600, 600 );
    window.show();

    return app.exec();
}
