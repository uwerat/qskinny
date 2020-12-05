/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "StackLayoutPage.h"
#include "ButtonBox.h"
#include "TestRectangle.h"

#include <QskAspect.h>
#include <QskPageIndicator.h>
#include <QskProgressBar.h>
#include <QskRgbValue.h>
#include <QskStackBox.h>
#include <QskStackBoxAnimator.h>
#include <QskTextLabel.h>

#include <QElapsedTimer>

namespace
{
    class StackBox : public QskStackBox
    {
        Q_OBJECT

      public:
        StackBox( QQuickItem* parent = nullptr )
            : QskStackBox( parent )
        {
            setObjectName( "StackBox" );

            setBackgroundColor( Qt::white );
            setDefaultAlignment( Qt::AlignCenter );

            addRectangle( "Gold" );
            addRectangle( "SeaGreen" );
            addRectangle( "SlateBlue" );
            addRectangle( "Peru" );

            for ( int i = 0; i < itemCount(); i += 2 )
            {
                if ( auto control = qskControlCast( itemAtIndex( i ) ) )
                    control->setFixedSize( 200, 200 );
            }
        }

        void incrementFading( int offset )
        {
            auto animator = dynamic_cast< QskStackBoxAnimator3* >( this->animator() );

            if ( animator == nullptr )
            {
                animator = new QskStackBoxAnimator3( this );
                animator->setEasingCurve( QEasingCurve::InQuad );
                animator->setDuration( 500 );
            }

            setAnimator( animator );
            setCurrentIndex( incrementedIndex( offset ) );

            Q_EMIT transitionStarted( animator->duration() );
        }

        void incrementScrolling( Qt::Orientation orientation, int offset )
        {
            auto animator = dynamic_cast< QskStackBoxAnimator1* >( this->animator() );

            if ( animator == nullptr )
            {
                animator = new QskStackBoxAnimator1( this );
                animator->setDuration( 1000 );
            }

            animator->setOrientation( orientation );
            setAnimator( animator );

            setCurrentIndex( incrementedIndex( offset ) );

            Q_EMIT transitionStarted( animator->duration() );
        }

      Q_SIGNALS:
        void transitionStarted( int ms );

      private:
        void addRectangle( const char* colorName )
        {
            auto rect = new TestRectangle( colorName );
            rect->setText( QString::number( itemCount() + 1 ) );
            addItem( rect );
        }

        int incrementedIndex( int offset ) const
        {
            int index = currentIndex();
            if ( index < 0 )
                index = 0;

            int newIndex = ( index + offset ) % itemCount();
            if ( newIndex < 0 )
                newIndex += itemCount();

            return newIndex;
        }
    };

    class ProgressBar : public QskProgressBar
    {
      public:
        ProgressBar( QQuickItem* parent = nullptr )
            : QskProgressBar( parent )
        {
            setValue( 0.0 );
            setMaximumWidth( 200 );
        }

        void startProgressing( int ms )
        {
            reset();

            if ( ms > 0 )
            {
                m_duration = ms;
                m_elapsed.start();
                m_timerId = startTimer( 5 );
            }
        }

      protected:

        void timerEvent( QTimerEvent* ) override
        {
            if ( m_elapsed.elapsed() >= m_duration )
                reset();
            else
                setValue( m_elapsed.elapsed() / m_duration * 100.0 );
        }

      private:
        void reset()
        {
            if ( m_timerId )
            {
                killTimer( m_timerId );
                m_timerId = 0;
            }
            setValue( 0.0 );
            m_duration = 0.0;
        }

        int m_timerId = 0;
        qreal m_duration = 0.0;
        QElapsedTimer m_elapsed;
    };
}

StackLayoutPage::StackLayoutPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setObjectName( "StackLayoutPage" );

    setMargins( 10 );
    setBackgroundColor( QskRgb::LightSteelBlue );

    auto box = new StackBox();

    auto buttonBox = new ButtonBox();

    buttonBox->setLayoutAlignmentHint( Qt::AlignTop | Qt::AlignLeft );
    buttonBox->addButton( "<<", [ box ]() { box->incrementScrolling( Qt::Horizontal, +1 ); } );
    buttonBox->addButton( ">>", [ box ]() { box->incrementScrolling( Qt::Horizontal, -1 ); } );
    buttonBox->addButton( "^", [ box ]() { box->incrementScrolling( Qt::Vertical, -1 ); } );
    buttonBox->addButton( "v", [ box ]() { box->incrementScrolling( Qt::Vertical, +1 ); } );
    buttonBox->addButton( "Fading", [ box ]() { box->incrementFading( +1 ); } );

    // page indicator

    auto pageIndicator = new QskPageIndicator();
    pageIndicator->setCount( box->itemCount() );
    pageIndicator->setCurrentIndex( box->currentIndex() );
    pageIndicator->setLayoutAlignmentHint( Qt::AlignCenter );

    // progress bar
    auto progressBar = new ProgressBar();
    progressBar->setMargins( QMarginsF( 0, 0, 10, 0 ) );


    addItem( buttonBox );
    addItem( pageIndicator );
    addItem( box );
    addItem( progressBar, Qt::AlignRight );

    connect( box, &QskStackBox::currentIndexChanged,
        pageIndicator, &QskPageIndicator::setCurrentIndex );

    connect( box, &StackBox::transitionStarted,
        progressBar, &ProgressBar::startProgressing );
}

#include "StackLayoutPage.moc"
