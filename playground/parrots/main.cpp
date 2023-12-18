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
#include <QskRgbValue.h>

#include <QGuiApplication>
#include <QDebug>

#include <SkinnyShortcut.h>
#include <cmath>

#include "Overlay.h"

class Image : public QskGraphicLabel
{
  public:
    Image( QQuickItem* parent = nullptr )
        : QskGraphicLabel( parent )
    {
        const QImage image( ":/images/parrots.jpg" );
        setGraphic( QskGraphic::fromImage( image ) );
    }
};

class ForegroundItem : public QskLinearBox
{
  public:
    ForegroundItem( QQuickItem* parent = nullptr )
        : QskLinearBox( Qt::Vertical, parent )
    {
        setMargins( 20 );

#if 0
        auto label = new Image( this );
        label->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
        label->setLayoutAlignmentHint( Qt::AlignCenter );
        label->setObjectName( "miniParrots" );
#endif

        auto button = new QskPushButton( "Button", this );
        button->setLayoutAlignmentHint( Qt::AlignHCenter | Qt::AlignBottom );

        button->setObjectName( "button" );
        setObjectName( "foreground" );
    }
};

class BackgroundItem : public QskControl
{
    using Inherited = QskControl;

  public:
    BackgroundItem( QQuickItem* parent = nullptr )
        : QskControl( parent )
    {
        setObjectName( "background" );

        m_label = new Image( this );
        m_label->setFillMode( QskGraphicLabel::Stretch );
        m_label->setObjectName( "parrots" );

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

        if ( m_label )
            m_label->setGeometry( labelRect );
    }

  private:
    QskGraphicLabel* m_label = nullptr;
};

class MainView : public QskControl
{
  public:
    MainView( QQuickItem* parent = nullptr )
        : QskControl( parent )
    {
        setPolishOnResize( true );

        m_background = new BackgroundItem( this );
#if 0
        {
            auto box = new QskBox( m_background );
            box->setGeometry( 20, 20, 600, 400 );
            box->setFillGradient( QskRgb::SaddleBrown );
            box->setObjectName( "redBox" );
        }
#endif
        m_overlay = new Overlay( m_background );
        m_overlay->setAutoLayoutChildren( true );
        m_overlay->setObjectName( "overlay" );

        (void )new ForegroundItem( m_overlay );

#if 0
        {
            auto box = new QskBox( m_background );
            box->setGeometry( 50, 50, 400, 200 );
            box->setFillGradient( QskRgb::PaleGreen );
            box->setObjectName( "blueBox" );
        }
#endif
        setObjectName( "mainView" );
    }

  protected:
    void updateLayout()
    {
        if ( m_background )
            m_background->setGeometry( rect() );

        QRectF blurredRect( QPointF(), 0.7 * size() );
        blurredRect.moveCenter( rect().center() );

        if ( m_overlay )
            qskSetItemGeometry( m_overlay, blurredRect );
    }

  private:
    BackgroundItem* m_background = nullptr;
    Overlay* m_overlay = nullptr;
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
