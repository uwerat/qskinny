/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SoundControl.h"
#include "SkinFactory.h"

#include <QskBox.h>
#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskGraphicLabel.h>
#include <QskGridBox.h>
#include <QskLinearBox.h>
#include <QskNamespace.h>
#include <QskPushButton.h>
#include <QskSeparator.h>
#include <QskSlider.h>
#include <QskTextLabel.h>

QSK_SUBCONTROL( SoundControl, Overlay )
QSK_SUBCONTROL( SoundControl, CrossHair )
QSK_SUBCONTROL( SoundControl, Marker )
QSK_SUBCONTROL( SoundControl, MarkerControl )
QSK_SUBCONTROL( SoundControl, Vehicle )
QSK_SUBCONTROL( SoundControl, SliderControl )

class VehicleLabel final : public QskGraphicLabel
{
  public:
    VehicleLabel( QQuickItem* parentItem = nullptr )
        : QskGraphicLabel( parentItem )
    {
        setGraphic( QskGraphicIO::read( QString( ":/qvg/car.qvg" ) ) );
    }

    QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol subControl ) const override
    {
        // so that we can set specific colors in the skin

        if ( subControl == QskGraphicLabel::Graphic )
            return SoundControl::Vehicle;

        return subControl;
    }
};

class CrossHairLine final : public QskBox
{
  public:
    CrossHairLine( QQuickItem* parent )
        : QskBox( parent )
    {
    }

    QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol subControl ) const override
    {
        if ( subControl == QskBox::Panel )
            return SoundControl::CrossHair;

        return subControl;
    }
};

class BalanceFadeMarker final : public QskBox
{
  public:
    BalanceFadeMarker( QQuickItem* parent )
        : QskBox( parent )
    {
    }

    QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol subControl ) const override
    {
        if ( subControl == QskBox::Panel )
            return SoundControl::Marker;

        return subControl;
    }
};

class MarkerControlButton final : public QskPushButton
{
  public:
    MarkerControlButton( Qsk::Direction direction, QQuickItem* parentItem = nullptr )
        : QskPushButton( parentItem )
        , m_direction( direction )
    {
        const char* svgList[] = { "right", "left", "down", "up" };

        const QString fileName = QString( ":/qvg/%1.qvg" ).arg( svgList[ direction ] );
        setGraphic( QskGraphicIO::read( fileName ) );

        setAutoRepeat( true );
        setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
    }

    QPointF offset() const
    {
        const qreal off = 5.0;

        switch ( m_direction )
        {
            case Qsk::LeftToRight:
                return QPointF( off, 0.0 );

            case Qsk::RightToLeft:
                return QPointF( -off, 0.0 );

            case Qsk::TopToBottom:
                return QPointF( 0.0, off );

            case Qsk::BottomToTop:
                return QPointF( 0.0, -off );
        }

        return QPointF();
    }

    QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol subControl ) const override
    {
        // so that we can set specific colors in the skin

        if ( subControl == QskPushButton::Graphic )
            return SoundControl::MarkerControl;

        return subControl;
    }

  protected:
    QSizeF contentsSizeHint() const override
    {
        const qreal dim = 100;

        if ( m_direction == Qsk::LeftToRight || m_direction == Qsk::RightToLeft )
            return QSizeF( 0.5 * dim, dim );
        else
            return QSizeF( dim, 0.5 * dim );
    }

  private:
    const Qsk::Direction m_direction;
};

class ControlButton final : public QskPushButton
{
  public:
    ControlButton( const char symbol, QQuickItem* parentItem = nullptr )
        : QskPushButton( parentItem )
    {
        setText( QChar( symbol ) );
        setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

        setAutoRepeat( true );
    }

    QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol subControl ) const override
    {
        if ( subControl == QskPushButton::Panel )
            return SoundControl::SliderControl;

        return QskPushButton::effectiveSubcontrol( subControl );
    }

    QSizeF contentsSizeHint() const override
    {
        qreal h = QskPushButton::contentsSizeHint().height();
        return QSizeF( h, h );
    }
};

class StackedControl final : public QskControl
{
  public:
    StackedControl( QQuickItem* parent = nullptr )
        : QskControl( parent )
        , m_offset( 0.0, 0.0 )
    {
        setPolishOnResize( true ); // we have t re-layout the crosshair
        setSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );

        auto horizontalCarRectangle = new CrossHairLine( this );
        horizontalCarRectangle->setObjectName( "horizontalBar" );

        auto verticalCarRectangle = new CrossHairLine( this );
        verticalCarRectangle->setObjectName( "verticalBar" );

        ( void ) new VehicleLabel( this );

        auto marker = new BalanceFadeMarker( this );
        marker->setObjectName( "marker" );
    }

    QPointF offset() const
    {
        return m_offset;
    }

    void setOffset( const QPointF& offset )
    {
        m_offset = offset;
        polish();
    }

  protected:
    void updateLayout() override
    {
        const QRectF cr = contentsRect();
        const qreal crossHairSize = 3;

        for ( int a = 0; a < children().count(); a++ )
        {
            auto control = static_cast< QskControl* >( children().at( a ) );

            if ( control->objectName() == "verticalBar" )
            {
                control->setGeometry( cr.center().x(), cr.top(), crossHairSize, cr.height() );
                control->setZ( 1 );
            }
            else if ( control->objectName() == "horizontalBar" )
            {
                control->setGeometry( cr.left(), cr.center().y(), cr.width(), crossHairSize );
                control->setZ( 1 );
            }
            else if ( control->objectName() == "marker" )
            {
                qreal size = 31;

                control->setPosition(
                    cr.center() - 0.5 * QPointF( size, size ) + m_offset + QPointF( 1, 1 ) );
                control->setSize( QSizeF( size, size ) );
                control->setZ( 1 );
            }
            else
            {
                control->setGeometry( cr );
            }
        }
    }

  private:
    QPointF m_offset;
};

class SectionTitleBar final : public QskLinearBox
{
  public:
    SectionTitleBar( const char* title, QQuickItem* parentItem = nullptr )
        : QskLinearBox( Qt::Horizontal, parentItem )
    {
        setSpacing( 10 );

        auto label = new QskTextLabel( title );
        label->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );

        addItem( new QskSeparator() );
        addItem( label );
        addItem( new QskSeparator() );

        setStretchFactor( 0, 1 );
        setStretchFactor( 2, 5 );
    }
};

class SliderBox final : public QskLinearBox
{
  public:
    SliderBox( const char* title, qreal min, qreal max, QQuickItem* parentItem = nullptr )
        : QskLinearBox( Qt::Vertical, parentItem )
    {
        auto label = new QskTextLabel( title );
        m_numberLabel = new QskTextLabel();

        // don't stretch the labels, so that the layout centers them
        label->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
        m_numberLabel->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );

        auto plusButton = new ControlButton( '+' );
        auto minusButton = new ControlButton( '-' );

        m_slider = new QskSlider( Qt::Vertical );
        m_slider->setMinimum( min );
        m_slider->setMaximum( max );
        m_slider->setStepSize( 10 );

        // layout

        addItem( label );
        addItem( m_numberLabel );
        addItem( plusButton );
        addSpacer( 10 );
        addItem( m_slider );
        addSpacer( 10 );
        addItem( minusButton );

        setDefaultAlignment( Qt::AlignCenter );

        // finally connect buttons/slider/labels

        connect( plusButton, &QskPushButton::pressed,
            this, [ this ]() { increment( 1 ); } );

        connect( minusButton, &QskPushButton::pressed,
            this, [ this ]() { increment( -1 ); } );

        connect( m_slider, &QskSlider::valueChanged,
            this, &SliderBox::setValue );
    }

  public Q_SLOTS:
    void setValue( qreal value )
    {
        m_slider->setValue( value );

        QString txt;
        txt.setNum( m_slider->value(), 'f', 1 );

        m_numberLabel->setText( txt );
    }

  private:
    void increment( qreal offset )
    {
        setValue( m_slider->value() + offset );
    }

    QskTextLabel* m_numberLabel;
    QskSlider* m_slider;
};

class ToneControlBox final : public QskLinearBox
{
  public:
    ToneControlBox( QQuickItem* parentItem = nullptr )
        : QskLinearBox( Qt::Horizontal, parentItem )
    {
        auto bassControl = new SliderBox( "Bass", 0.0, 40.0, this );
        auto treebleControl = new SliderBox( "Treeble", 0.0, 40.0, this );
        auto subControl = new SliderBox( "Sub", 0.0, 40.0, this );

        bassControl->setValue( 30 );
        treebleControl->setValue( 11 );
        subControl->setValue( 18 );
    }
};

class BalanceFadeControlBox final : public QskGridBox
{
  public:
    BalanceFadeControlBox( QQuickItem* parentItem = nullptr )
        : QskGridBox( parentItem )
    {
        MarkerControlButton* buttons[ 4 ];
        for ( int i = 0; i < 4; i++ )
            buttons[ i ] = new MarkerControlButton( static_cast< Qsk::Direction >( i ) );

        m_carControl = new StackedControl();

        addItem( buttons[ Qsk::RightToLeft ], 1, 0 );
        addItem( buttons[ Qsk::BottomToTop ], 0, 0, 1, 3 );
        addItem( buttons[ Qsk::LeftToRight ], 1, 2 );
        addItem( buttons[ Qsk::TopToBottom ], 2, 0, 1, 3 );

        addItem( m_carControl, 1, 1 );

        for ( int i = 0; i < 4; i++ )
        {
            const auto button = buttons[ i ];

            setAlignment( button, Qt::AlignCenter );

            connect( button, &QskPushButton::pressed,
                this, [ this, button ]() { shift( button->offset() ); } );
        }
    }

    void shift( const QPointF& offset )
    {
        m_carControl->setOffset( m_carControl->offset() + offset );
    }

    StackedControl* m_carControl;
};

SoundControl::SoundControl( QQuickItem* parent )
    : QskBox( parent )
{
    setAutoLayoutChildren( true );

    auto layout = new QskGridBox( this );
    layout->setMargins( QMarginsF( 40, 20, 40, 20 ) );
    layout->setSpacing( Qt::Vertical, 10 );
    layout->setSpacing( Qt::Horizontal, 60 );
    layout->setColumnStretchFactor( 0, 1 );
    layout->setColumnStretchFactor( 1, 2 );

    layout->addItem( new SectionTitleBar( "Tone" ), 0, 0 );
    layout->addItem( new ToneControlBox(), 1, 0 );

    layout->addItem( new SectionTitleBar( "Balance / Fade" ), 0, 1 );
    layout->addItem( new BalanceFadeControlBox(), 1, 1 );
}

QskAspect::Subcontrol SoundControl::effectiveSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskBox::Panel )
        return SoundControl::Overlay;

    return subControl;
}
