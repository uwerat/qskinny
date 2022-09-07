#include "MainWindow.h"
#include "BlurredBox.h"

#include <QColor>
#include <QskLinearBox.h>
#include <QskStackBox.h>

#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskSegmentedBar.h>
#include <QskShortcutMap.h>
#include <QskSkin.h>
#include <QskSlider.h>
#include <QskTextInput.h>
#include <QskTextLabel.h>

MainWindow::MainWindow()
{
    constexpr QSize size = { 1280, 720 };
    setMinimumSize( size );
    setMaximumSize( size );
    setTitle( tr( "Blurring" ) );
    createBackground();

    // create a centered, blurred and rounded rectangle
    auto* const layout = new QskLinearBox( Qt::Vertical, contentItem() );
    layout->setMargins( 40 );
    auto* const stack = new QskStackBox( layout );
    stack->setAutoLayoutChildren( true );
    auto* const blurred = new BlurredBox( stack );
    blurred->setBlurSize( 20.0 );
    blurred->setBoxShapeHint(BlurredBox::Panel, { 40, 40, 40, 40 });
    auto* const l = new QskLinearBox( Qt::Vertical, layout );
    stack->addItem( l );

    // create controls to change the rectangle
    l->addSpacer( 10, 1 );
    createBlurDirectionsControls( blurred, l );
    createBlurQualityControls( blurred, l );
    createBlurSizeControls( blurred, l );
    createBlurOpacityControls( blurred, l );
    createBlurCornerRadiiControls( blurred, l );
    l->addSpacer( 10, 1 );
    createShortcutNote( l );
    l->addSpacer( 10, 1 );
    return;
}

void MainWindow::createBackground()
{
    // create a brackground image
    auto* const graphic = new QskGraphicLabel( contentItem() );
    graphic->setFillMode( QskGraphicLabel::FillMode::Stretch );
    graphic->setAlignment( Qt::AlignCenter );

    // callback for rotating through the background images
    auto updateBackground = [ this, graphic ]() {
        static unsigned int index = 2;
        index = 1 + ( index + 1 ) % 3;
        const QImage image( QString( ":/backgrounds/background%1.jpg" ).arg( index ) );
        graphic->setGraphic( QskGraphic::fromImage( image ) );
        update();
    };
    updateBackground();

    QKeySequence keys( Qt::CTRL | Qt::Key_Space );
    QskShortcutMap::addShortcut( keys, false, contentItem(), updateBackground );
}

void MainWindow::createBlurDirectionsControls( BlurredBox* blurred, QskLinearBox* layout )
{
    auto* const label = new QskTextLabel( layout );
    label->setTextColor( Qt::white );
    label->setFontRole( QskSkin::MediumFont );
    auto* const slider = new QskSlider( Qt::Horizontal, layout );
    slider->setMinimum( 16.0 );
    slider->setMaximum( 32.0 );
    connect( slider, &QskSlider::valueChanged, slider, [ blurred, label ]( qreal value ) {
        blurred->setBlurDirections( static_cast< float >( value ) );
        label->setText( QString( tr( "Blur Directions" ) + " ( %1 )" ).arg( value ) );
    } );
    slider->setValue( blurred->blurDirections() );
}

void MainWindow::createBlurQualityControls( BlurredBox* blurred, QskLinearBox* layout )
{
    auto* const label = new QskTextLabel( layout );
    label->setTextColor( Qt::white );
    label->setFontRole( QskSkin::MediumFont );
    auto* const slider = new QskSlider( Qt::Horizontal, layout );
    slider->setMinimum( 4.0 );
    slider->setMaximum( 16.0 );
    connect( slider, &QskSlider::valueChanged, slider, [ blurred, label ]( qreal value ) {
        blurred->setBlurQuality( static_cast< float >( value ) );
        label->setText( QString( tr( "Blur Quality" ) + " ( %1 )" ).arg( value ) );
    } );
    slider->setValue( blurred->blurQuality() );
}

void MainWindow::createBlurSizeControls( BlurredBox* blurred, QskLinearBox* layout )
{
    auto* const label = new QskTextLabel( layout );
    label->setTextColor( Qt::white );
    label->setFontRole( QskSkin::MediumFont );
    auto* const slider = new QskSlider( Qt::Horizontal, layout );
    slider->setMinimum( 4.0 );
    slider->setMaximum( 32.0 );
    connect( slider, &QskSlider::valueChanged, slider, [ blurred, label ]( qreal value ) {
        blurred->setBlurSize( static_cast< float >( value ) );
        label->setText( QString( tr( "Blur Size" ) + " ( %1 )" ).arg( value ) );
    } );
    slider->setValue( blurred->blurSize() );
}

void MainWindow::createBlurOpacityControls( BlurredBox* blurred, QskLinearBox* layout )
{
    auto* const label = new QskTextLabel( layout );
    label->setTextColor( Qt::white );
    label->setFontRole( QskSkin::MediumFont );
    auto* const slider = new QskSlider( Qt::Horizontal, layout );
    slider->setMinimum( 0.0 );
    slider->setMaximum( 1.0 );
    connect( slider, &QskSlider::valueChanged, slider, [ blurred, label ]( qreal value ) {
        blurred->setOpacity( value );
        label->setText( QString( tr( "Blur Opacity" ) + " ( %1 )" ).arg( value ) );
    } );
    slider->setValue( blurred->opacity() );
}

void MainWindow::createBlurCornerRadiiControls( BlurredBox* blurred, QskLinearBox* layout )
{
    auto* const label = new QskTextLabel( layout );
    label->setTextColor( Qt::white );
    label->setFontRole( QskSkin::MediumFont );
    auto* const bar = new QskSegmentedBar( Qt::Horizontal, layout );
    enum
    {
        TL = 0,
        TR = 1,
        BR = 2,
        BL = 3
    };
    bar->addText( "TL" );
    bar->addText( "TR" );
    bar->addText( "BR" );
    bar->addText( "BL" );
    auto* const slider = new QskSlider( Qt::Horizontal, layout );
    slider->setMinimum( 0.0 );
    slider->setMaximum( 80.0 );
    connect( slider, &QskSlider::valueChanged, slider, [ blurred, bar, label ]( qreal value ) {
        auto shape = blurred->boxShapeHint(BlurredBox::Panel);
        const auto format = tr( "Corner Radius" ) + " ( %1 )";

        switch ( bar->currentIndex() )
        {
            case TL:
                shape.setTopLeft( { value, value } );
                break;
            case TR:
                shape.setTopRight( { value, value } );
                break;
            case BR:
                shape.setBottomRight( { value, value } );
                break;
            case BL:
                shape.setBottomLeft( { value, value } );
                break;
        }

        label->setText( format.arg( value ) );
        blurred->setBoxShapeHint(BlurredBox::Panel, shape );
        blurred->update();
    } );
    connect( bar, &QskSegmentedBar::currentIndexChanged, slider, [ blurred, slider ]( int index ) {
        const auto shape = blurred->boxShapeHint(BlurredBox::Panel);

        switch ( index )
        {
            case TL:
                slider->setValue( shape.topLeft().width() );
                break;
            case TR:
                slider->setValue( shape.topRight().width() );
                break;
            case BR:
                slider->setValue( shape.bottomRight().width() );
                break;
            case BL:
                slider->setValue( shape.bottomLeft().width() );
                break;
        }
    } );
    slider->setValue( blurred->boxShapeHint(BlurredBox::Panel).topLeft().width() );
    bar->setSelectedIndex( TL );
    bar->setCurrentIndex( TL );
}

void MainWindow::createShortcutNote( QskLinearBox* layout )
{
    const auto text = tr( "Press 'Ctrl + Space' to change the background" );
    auto* const label = new QskTextLabel( text, layout );
    label->setTextColor( Qt::white );
    label->setFontRole( QskSkin::LargeFont );
}

#include "moc_MainWindow.cpp"
