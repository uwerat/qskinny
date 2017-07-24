#include "SoundControl.h"

#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskGraphicProvider.h>
#include <QskGridBox.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskSlider.h>
#include <QskTabBar.h>
#include <QskTextLabel.h>

#include <QImage>

QSK_SUBCONTROL( FilledRectangle, Panel )

QskAspect::Subcontrol FilledRectangle::effectiveSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskPushButton::Panel )
        return FilledRectangle::Panel;

    return subControl;
}

QSK_SUBCONTROL( BalanceFadeBox, Panel )

QskAspect::Subcontrol BalanceFadeBox::effectiveSubcontrol(
        QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskPushButton::Panel )
        return BalanceFadeBox::Panel;

    return subControl;
}

class StackedControl : public QskControl {
public:
    StackedControl( QQuickItem* parent = nullptr ) : QskControl( parent ),
        m_offset( 0.0, 0.0 )
    {
    }

    QPointF offset() const
    {
        return m_offset;
    }

    void setOffset( const QPointF& offset )
    {
        m_offset = offset;
    }

protected:
    virtual void updateLayout() override final
    {
        const QRectF cr = contentsRect();

        for ( int a = 0; a < children().count(); a++ )
        {
            QskControl* control = static_cast< QskControl* >( children().at( a ) );

            qreal xCenter = ( cr.width() + margins().top() + margins().bottom() ) / 2;
            qreal yCenter = ( cr.height() + margins().left() + margins().right() ) / 2;

            if ( control->objectName() == "verticalBar" )
            {
                control->setPosition( QPointF( xCenter, margins().top() ) );
                control->setSize( QSizeF( 3, cr.height() ) );
                control->setZ( 1 );
            }
            else if ( control->objectName() == "horizontalBar" )
            {
                control->setPosition( QPointF( margins().left(), yCenter ) );
                control->setSize( QSizeF( cr.width(), 3 ) );
                control->setZ( 1 );
            }
            else if ( control->objectName() == "box" )
            {
                qreal size = 30;
                control->setPosition( QPointF( xCenter - ( size / 2 ) + 1 + m_offset.x(),
                                               yCenter - ( size / 2 ) + 1 + m_offset.y() ) );
                control->setSize( QSizeF( size, size ) );
                control->setZ( 2 );
            }
            else
            {
                control->setPosition( cr.topLeft() );
                control->setSize( cr.size() );
            }
        }
    }

private:
    QPointF m_offset;
};

SoundControl::SoundControl( QQuickItem *parent ) : QskControl( parent )
{
    setMargins( QMarginsF( 40, 20, 40, 20 ) );
    setAutoLayoutChildren( true );

    QskGridBox* outterLayout = new QskGridBox( this );
    outterLayout->setVerticalSpacing( 40 );
    outterLayout->setHorizontalSpacing( 60 );
    outterLayout->setColumnStretchFactor( 0, 1 );
    outterLayout->setColumnStretchFactor( 1, 2 );

    QskLinearBox* toneBox = new QskLinearBox( Qt::Horizontal, outterLayout );
    toneBox->setSpacing( 20 );
    toneBox->setAutoAddChildren( true );
    toneBox->setAutoLayoutChildren( true );
    toneBox->addSpacer( 0, 1 );
    FilledRectangle* toneRectangle = new FilledRectangle( toneBox );
    QskTextLabel* toneLabel = new QskTextLabel( "Tone", toneBox );
    toneLabel->setAlignment( Qt::AlignLeft );
    toneRectangle->setFixedHeight(
        QFontMetricsF( toneLabel->effectiveFont( QskTextLabel::Text ) ).height() );
    toneBox->addSpacer( 0, 1 );
    outterLayout->addItem( toneBox, 0, 0 );


    QskLinearBox* balanceBox = new QskLinearBox( Qt::Horizontal, outterLayout );
    balanceBox->setSpacing( 20 );
    balanceBox->setAutoAddChildren( true );
    balanceBox->setAutoLayoutChildren( true );
    balanceBox->addSpacer( 0, 1 );
    FilledRectangle* balanceRectangle = new FilledRectangle( balanceBox );
    QskTextLabel* balanceLabel = new QskTextLabel( "Balance / Fade", balanceBox );
    balanceLabel->setAlignment( Qt::AlignLeft );
    balanceRectangle->setFixedHeight(
        QFontMetricsF( balanceLabel->effectiveFont( QskTextLabel::Text ) ).height() );
    balanceBox->addSpacer( 0, 1 );
    outterLayout->addItem( balanceBox, 0, 1 );


    QskGridBox* toneGridBox = new QskGridBox( outterLayout );
    QskTextLabel* bassLabel = new QskTextLabel( "Bass", toneGridBox );
    toneGridBox->addItem( bassLabel, 0, 0 );
    QskTextLabel* trebleLabel = new QskTextLabel( "Treble", toneGridBox );
    toneGridBox->addItem( trebleLabel, 0, 1 );
    QskTextLabel* subLabel = new QskTextLabel( "Sub", toneGridBox );
    toneGridBox->addItem( subLabel, 0, 2 );

    QskTextLabel* bassNumberLabel = new QskTextLabel( "0", toneGridBox );
    toneGridBox->addItem( bassNumberLabel, 1, 0 );
    QskTextLabel* trebleNumberLabel = new QskTextLabel( "0", toneGridBox );
    toneGridBox->addItem( trebleNumberLabel, 1, 1 );
    QskTextLabel* subNumberLabel = new QskTextLabel( "0", toneGridBox );
    toneGridBox->addItem( subNumberLabel, 1, 2 );

    QskPushButton* bassPlusButton = new QskPushButton( "+", toneGridBox );
    bassPlusButton->setFixedSize( 35, 35 );
    toneGridBox->addItem( bassPlusButton, 2, 0 );
    toneGridBox->setAlignment( bassPlusButton, Qt::AlignCenter );
    QskPushButton* treblePlusButton = new QskPushButton( "+", toneGridBox );
    treblePlusButton->setFixedSize( 35, 35 );
    toneGridBox->addItem( treblePlusButton, 2, 1 );
    toneGridBox->setAlignment( treblePlusButton, Qt::AlignCenter );
    QskPushButton* subPlusButton = new QskPushButton( "+", toneGridBox );
    subPlusButton->setFixedSize( 35, 35 );
    toneGridBox->addItem( subPlusButton, 2, 2 );
    toneGridBox->setAlignment( subPlusButton, Qt::AlignCenter );

    QskSlider* bassSlider = new QskSlider( Qt::Vertical, toneGridBox );
    bassSlider->setMinimum( 0 );
    bassSlider->setMaximum( 40 );
    bassSlider->setValue( 30 );
    toneGridBox->addItem( bassSlider, 3, 0 );
    toneGridBox->setAlignment( bassSlider, Qt::AlignCenter );
    QskSlider* trebleSlider = new QskSlider( Qt::Vertical, toneGridBox );
    trebleSlider->setMinimum( 0 );
    trebleSlider->setMaximum( 40 );
    trebleSlider->setValue( 11 );
    toneGridBox->addItem( trebleSlider, 3, 1 );
    toneGridBox->setAlignment( trebleSlider, Qt::AlignCenter );
    QskSlider* subSlider = new QskSlider( Qt::Vertical, toneGridBox );
    subSlider->setMinimum( 0 );
    subSlider->setMaximum( 40 );
    subSlider->setValue( 18 );
    toneGridBox->addItem( subSlider, 3, 2 );
    toneGridBox->setAlignment( subSlider, Qt::AlignCenter );

    QskPushButton* bassMinusButton = new QskPushButton( "-", toneGridBox );
    bassMinusButton->setFixedSize( 35, 35 );
    toneGridBox->addItem( bassMinusButton, 4, 0 );
    toneGridBox->setAlignment( bassMinusButton, Qt::AlignCenter );
    QskPushButton* trebleMinusButton = new QskPushButton( "-", toneGridBox );
    trebleMinusButton->setFixedSize( 35, 35 );
    toneGridBox->addItem( trebleMinusButton, 4, 1 );
    toneGridBox->setAlignment( trebleMinusButton, Qt::AlignCenter );
    QskPushButton* subMinusButton = new QskPushButton( "-", toneGridBox );
    subMinusButton->setFixedSize( 35, 35 );
    toneGridBox->addItem( subMinusButton, 4, 2 );
    toneGridBox->setAlignment( subMinusButton, Qt::AlignCenter );

    connect( bassPlusButton, &QskPushButton::pressed, [ bassSlider ]() {
        bassSlider->setValue( bassSlider->value() + 1 );
    } );
    connect( treblePlusButton, &QskPushButton::pressed, [ trebleSlider ]() {
        trebleSlider->setValue( trebleSlider->value() + 1 );
    } );
    connect( subPlusButton, &QskPushButton::pressed, [ subSlider ]() {
        subSlider->setValue( subSlider->value() + 1 );
    } );
    connect( bassMinusButton, &QskPushButton::pressed, [ bassSlider ]() {
        bassSlider->setValue( bassSlider->value() - 1 );
    } );
    connect( trebleMinusButton, &QskPushButton::pressed, [ trebleSlider ]() {
        trebleSlider->setValue( trebleSlider->value() - 1 );
    } );
    connect( subMinusButton, &QskPushButton::pressed, [ subSlider ]() {
        subSlider->setValue( subSlider->value() - 1 );
    } );

    outterLayout->addItem( toneGridBox, 1, 0 );


    QskGridBox* carGridBox = new QskGridBox( outterLayout );
    carGridBox->setAutoLayoutChildren( true );

    QskPushButton* upButton = new QskPushButton( carGridBox );
    upButton->setFixedSize( 100, 50 );
    QImage upImage( ":/images/up.svg" );
    QskGraphic upGraphic = QskGraphic::fromImage( upImage );
    upButton->setGraphic( upGraphic ); // ### try with setGraphicSource
    carGridBox->addItem( upButton, 0, 0, 1, 3 );
    carGridBox->setAlignment( upButton, Qt::AlignCenter );

    QskPushButton* leftButton = new QskPushButton( carGridBox );
    leftButton->setFixedSize( 50, 100 );
    QImage leftImage( ":/images/left.svg" );
    QskGraphic leftGraphic = QskGraphic::fromImage( leftImage );
    leftButton->setGraphic( leftGraphic );
    carGridBox->addItem( leftButton, 1, 0 );

    StackedControl* carControl = new StackedControl( carGridBox );
    carControl->setSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );
    carControl->setPolishOnResize( true );
    carControl->setMargins( 10 );
    FilledRectangle* horizontalCarRectangle = new FilledRectangle( carControl );
    horizontalCarRectangle->setObjectName( "horizontalBar" );
    FilledRectangle* verticalCarRectangle = new FilledRectangle( carControl );
    verticalCarRectangle->setObjectName( "verticalBar" );
    BalanceFadeBox *box = new BalanceFadeBox( carControl );
    box->setObjectName( "box" );
    QImage carImage( ":/images/car.svg" );
    QskGraphic graphic = QskGraphic::fromImage( carImage );
    QskGraphicLabel* carLabel = new QskGraphicLabel( carControl );
    carLabel->setGraphic( graphic );
    carGridBox->addItem( carControl, 1, 1 );

    QskPushButton* rightButton = new QskPushButton( carGridBox );
    rightButton->setFixedSize( 50, 100 );
    QImage rightImage( ":/images/right.svg" );
    QskGraphic rightGraphic = QskGraphic::fromImage( rightImage );
    rightButton->setGraphic( rightGraphic );
    carGridBox->addItem( rightButton, 1, 2 );
    carGridBox->setAlignment( rightButton, Qt::AlignRight );

    QskPushButton* downButton = new QskPushButton( carGridBox );
    downButton->setFixedSize( 100, 50 );
    QImage downImage( ":/images/down.svg" );
    QskGraphic downGraphic = QskGraphic::fromImage( downImage );
    downButton->setGraphic( downGraphic );
    carGridBox->addItem( downButton, 2, 0, 1, 3 );
    carGridBox->setAlignment( downButton, Qt::AlignCenter );

    connect( upButton, &QskPushButton::pressed, [ carControl ]() {
        carControl->setOffset( QPointF( carControl->offset().x(),
                                        carControl->offset().y() - 5.0 ) );
        carControl->polish();
    } );
    connect( leftButton, &QskPushButton::pressed, [ carControl ]() {
        carControl->setOffset( QPointF( carControl->offset().x() - 5.0,
                                        carControl->offset().y() ) );
        carControl->polish();
    } );
    connect( rightButton, &QskPushButton::pressed, [ carControl ]() {
        carControl->setOffset( QPointF( carControl->offset().x() + 5.0,
                                        carControl->offset().y() ) );
        carControl->polish();
    } );
    connect( downButton, &QskPushButton::pressed, [ carControl ]() {
        carControl->setOffset( QPointF( carControl->offset().x(),
                                        carControl->offset().y() + 5.0 ) );
        carControl->polish();
    } );

    outterLayout->addItem( carGridBox, 1, 1 );
}
