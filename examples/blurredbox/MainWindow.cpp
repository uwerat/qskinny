#include "MainWindow.h"
#include "BlurredBox.h"
#include "BlurredBoxTextureProvider.h"
#include "BlurredBoxSkinlet.h"

#include <QColor>
#include <QskLinearBox.h>
#include <QskStackBox.h>

#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskGridBox.h>
#include <QskSegmentedBar.h>
#include <QskShortcutMap.h>
#include <QskSkin.h>
#include <QskSlider.h>
#include <QskTextInput.h>
#include <QskTextLabel.h>
#include <QQuickItemGrabResult>
#include <QSGTextureProvider>
#include <QskAnimator.h>
#include <QskBoxShapeMetrics.h>
#include <QskPushButton.h>

#include <memory>

class BlurredBoxAnimator : public QskAnimator{
public:
    BlurredBoxAnimator(BlurredBox& blurred): m_blurred(blurred) {

    }
    void advance(qreal value) override
    {        
        m_blurred.setOpacity(value);
    }

    void done() override {
        delete this;
    }
private:
    BlurredBox& m_blurred;
};

enum Layers : int { BACKGROUND = 1, BLURRED = 2, CONTROLS = 3 };

MainWindow::MainWindow() : m_stack(new QskStackBox(contentItem()))
{
    constexpr auto width = 1280;
    constexpr auto height = 720;
    constexpr QSize size = { width, height };
    setMinimumSize( size );
    setMaximumSize( size );
    setTitle( tr( "Blurring" ) );

    m_stack->setAutoLayoutChildren(true);
    createBackground();

    QskShortcutMap::addShortcut( Qt::CTRL | Qt::Key_O, false, contentItem(), [this](){
        createOverlay();
    });
}

void MainWindow::createBackground()
{
    // create a brackground image
    auto* const graphic = new QskGraphicLabel( m_stack );
    graphic->setZ(BACKGROUND);
    graphic->setFillMode( QskGraphicLabel::FillMode::Stretch );
    graphic->setAlignment( Qt::AlignCenter );

    static int index = 0;
    const QImage image( QString( ":/backgrounds/background%1.jpg" ).arg( 1 + index++  ) );
    graphic->setGraphic( QskGraphic::fromImage( image ) );
    m_stack->addItem(graphic);
    update();
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
    slider->setMinimum( 0.0 );
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

void MainWindow::createOverlay()
{
    auto* const skinlet = new BlurredBoxSkinlet(std::make_shared<BlurredBoxTextureProvider>(this));
    skinlet->setOwnedBySkinnable(true);

    auto* blurredLayout = new QskLinearBox(m_stack);
    blurredLayout->setZ(BLURRED);

    auto* const blurred = new BlurredBox(blurredLayout);
    blurred->setBoxShapeHint(BlurredBox::Panel, { 40, 40, 40, 40 });
    blurred->setBlurQuality(20);
    blurred->setBlurDirections(32);
    blurred->setBlurSize(16);
    blurred->setSkinlet(skinlet);
    blurred->setMargins(80);

    auto* const animator = new BlurredBoxAnimator(*blurred);
    animator->setDuration(200);
    animator->setWindow(this);
    animator->start();

    auto* const controlsLayout = new QskLinearBox( Qt::Vertical, m_stack );
    controlsLayout->setZ(CONTROLS);
    controlsLayout->setMargins(80);

    // create controls to change the rectangle
    controlsLayout->addSpacer( 10, 1 );
    createBlurDirectionsControls( blurred, controlsLayout );
    createBlurQualityControls( blurred, controlsLayout );
    createBlurSizeControls( blurred, controlsLayout );
    createBlurOpacityControls( blurred, controlsLayout );
    createBlurCornerRadiiControls( blurred, controlsLayout );
    controlsLayout->addSpacer( 10, 1 );
    createShortcutNote( controlsLayout );
    controlsLayout->addSpacer( 10, 1 );

    update();
}

#include "moc_MainWindow.cpp"
