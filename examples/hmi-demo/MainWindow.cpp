#include "MainWindow.h"
#include "RadioControl.h"
#include "SoundControl.h"

#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskGraphicLabel.h>
#include <QskLinearBox.h>
#include <QskTextLabel.h>

#include <QDate>
#include <QImage>

namespace
{
    class ButtonBar : public QskLinearBox
    {
    public:
        ButtonBar( QQuickItem* parentItem = nullptr ):
            QskLinearBox( parentItem )
        {
            QColor c( Qt::black );
            c.setAlphaF( 0.5 );
            setBackgroundColor( c );

            setMargins( QMarginsF( 20, 15, 20, 15 ) );
            setSpacing( 20 );

            setSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::MinimumExpanding );
        }

        void addIcon( const char* name )
        {
            auto* label = new QskGraphicLabel( this );

            /*
                The label should adjust vertically and be stretched horizontally
                according to its aspect ratio.
             */

            label->setSizePolicy( QskSizePolicy::Constrained, QskSizePolicy::Ignored );

            const QString fileName = QString( ":/qvg/%1.qvg" ).arg( name );
            label->setGraphic( QskGraphicIO::read( fileName ) );
        }

    protected:
        virtual QSizeF contentsSizeHint() const override final
        {
            return QSizeF( -1, 20 );
        }
    };
}

MainWindow::MainWindow()
{
    const QImage image( ":/images/background.jpg" );

    auto backgroundImage = new QskGraphicLabel( contentItem() );
    backgroundImage->setGraphic( QskGraphic::fromImage( image ) );
    backgroundImage->setFillMode( QskGraphicLabel::Stretch );

    auto header = headerBar();
    auto content = mainContent();
    auto footer = footerBar();

    auto layout = new QskLinearBox( Qt::Vertical, contentItem() );

    layout->addItem( header );
    layout->setStretchFactor( header, 1 );

    layout->addItem( content );
    layout->setStretchFactor( content, 10 );

    layout->addItem( footer );
    layout->setStretchFactor( footer, 1 );

    setAutoLayoutChildren( true );
}

QQuickItem* MainWindow::headerBar() const
{
    auto* header = new ButtonBar();
    header->addIcon( "bluetooth" );
    header->addIcon( "location" );
    header->addIcon( "phone" );

    auto dateLabel = new QskTextLabel( QDate::currentDate().toString(), header );
    dateLabel->setColor( QskTextLabel::Text, Qt::white );

    header->addIcon( "user" );
    header->addIcon( "bookmark" );
    header->addIcon( "menu" );

    return header;
}

QQuickItem* MainWindow::mainContent() const
{
//    RadioControl* radioControl = new RadioControl( m_layout );
//    m_layout->setRetainSizeWhenHidden( radioControl, true );
//    radioControl->setVisible( false );

    return new SoundControl();
}

QQuickItem* MainWindow::footerBar() const
{
    auto* footer = new ButtonBar();

    footer->addIcon( "cloud" );
    footer->addIcon( "man" );
    footer->addIcon( "bus" );
    footer->addIcon( "plane" );
    footer->addIcon( "train" );

    footer->addStretch( 10 );

    return footer;
}
