/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include <SkinnyNamespace.h>

#include <QskFunctions.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskQuick.h>
#include <QskWindow.h>
#include <QskGraphicLabel.h>
#include <QskGraphic.h>
#include <QskEvent.h>

#include <QGuiApplication>
#include <QDebug>

#include <SkinnyShortcut.h>
#include <cmath>

#include "BlurredOverlay.h"

class ButtonBox : public QskLinearBox
{
  public:
    ButtonBox( QQuickItem* parent = nullptr )
        : QskLinearBox( parent )
    {
        ( void ) new QskPushButton( "Button 1", this );
        ( void ) new QskPushButton( "Button 2", this );
        ( void ) new QskPushButton( "Button 3", this );
    }
};

class Overlay : public BlurredOverlay
{
    using Inherited = BlurredOverlay;

  public:
    Overlay( QQuickItem* parent = nullptr )
        : Inherited( parent )
    {
    }

  protected:
    void geometryChange( const QRectF& newRect, const QRectF& oldRect ) override
    {
        Inherited::geometryChange( newRect, oldRect );
        setGrabRect( grabbedItem()->mapRectFromItem( this, newRect ) );
    }
};

class BlurredBox : public QskControl
{
    using Inherited = QskControl;

  public:
    BlurredBox( QQuickItem* parent = nullptr )
        : QskControl( parent )
    {
        setFlag( QQuickItem::ItemHasContents, false );
        setAutoLayoutChildren( true );

        m_overlay = new Overlay( this );
    }

    void setGrabbedItem( QQuickItem* item )
    {
        m_overlay->setGrabbedItem( item );
    }

  private:
    Overlay* m_overlay;
};

class BackgroundItem : public QskControl
{
    using Inherited = QskControl;

  public:
    BackgroundItem( QQuickItem* parent = nullptr )
        : QskControl( parent )
    {
        m_label = new QskGraphicLabel( this );
        m_label->setFillMode( QskGraphicLabel::Stretch );

        const QImage image( ":/images/parrots.jpg" );
        m_label->setGraphic( QskGraphic::fromImage( image ) );

        startTimer( 20 );
    }

  protected:
    void timerEvent( QTimerEvent* ) override
    {
        updateLabel();
    }

    void geometryChange( const QRectF& newGeometry,
        const QRectF& oldGeometry ) override
    {
        Inherited::geometryChange( newGeometry, oldGeometry );
        updateLabel();
    }

  private:
    void updateLabel()
    {
        static int counter = 0;

        const auto angle = counter++ / 50.0 * M_PI * 2.0;

        const auto x = std::cos( angle );
        const auto y = std::sin( angle );

        const qreal margin = 20;

        auto labelRect = rect();
        labelRect.adjust( margin, margin, -margin, -margin );
        labelRect.translate( margin * x, margin * y );

        m_label->setGeometry( labelRect );
    }

  private:
    QskGraphicLabel* m_label;
};

class MainView : public QskControl
{
  public:
    MainView( QQuickItem* parent = nullptr )
        : QskControl( parent )
    {
        setPolishOnResize( true );

        m_background = new BackgroundItem( this );

        m_blurredBox = new BlurredBox( this );
        m_blurredBox->setGrabbedItem( m_background );

        (void )new ButtonBox( m_blurredBox );
    }

  protected:
    void updateLayout()
    {
        m_background->setGeometry( rect() );

        QRectF blurredRect( QPointF(), 0.7 * size() );
        blurredRect.moveCenter( rect().center() );

        m_blurredBox->setGeometry( blurredRect );
    }

  private:
    BackgroundItem* m_background;
    BlurredBox* m_blurredBox;
};

int main( int argc, char** argv )
{
    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QskWindow window;
    window.setColor( Qt::darkGray );
    window.addItem( new MainView( window.contentItem() ) );
    window.resize( 800, 600 );
    window.show();

    return app.exec();
}
