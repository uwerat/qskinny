#include "RadioControl.h"

#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskTabBar.h>
#include <QskTabButton.h>
#include <QskTextLabel.h>

RadioControl::RadioControl( QQuickItem* parent ):
    QskControl( parent ),
    m_currentBand( "FM" )
{
    setMargins( QMarginsF( 40, 30, 40, 30 ) );
    setAutoLayoutChildren( true );

    QskLinearBox* outterLayout = new QskLinearBox( Qt::Vertical, this );
    outterLayout->setAutoAddChildren( true );

    QskTabBar* favoritesBar = new QskTabBar( outterLayout );
    favoritesBar->addTab( "AM" );
    favoritesBar->addTab( "FM" );
    favoritesBar->addTab( "DAB" );

    outterLayout->addSpacer( 80 );
    outterLayout->setMargins( QMarginsF( 150, 0, 150, 0 ) );
    outterLayout->setAlignment( 2, Qt::AlignHCenter );

    QskPushButton* frequencyButton = new QskPushButton( outterLayout );
    frequencyButton->setAutoLayoutChildren( true );
    frequencyButton->setFlat( true );
    frequencyButton->setBackgroundColor( QColor( 183, 210, 255 ) );
    frequencyButton->setEnabled( false );
    frequencyButton->setFixedWidth( 400 );

    QskLinearBox* frequencyLayout = new QskLinearBox( Qt::Horizontal, frequencyButton );
    frequencyLayout->setAutoAddChildren( true );
    frequencyLayout->setMargins( QMarginsF( 150, 20, 150, 20 ) );

    QskTextLabel* bandLabel = new QskTextLabel( m_currentBand, frequencyLayout );
    QskTextLabel* frequencyLabel = new QskTextLabel( "87.50", frequencyLayout );
    QskTextLabel* hzLabel = new QskTextLabel( "MHz", frequencyLayout );

    QskLinearBox* tuneLayout = new QskLinearBox( Qt::Horizontal, outterLayout );
    tuneLayout->setAutoLayoutChildren( true );

    QskPushButton* prevTuneButton = new QskPushButton( tuneLayout );
    QImage prevTuneImage( ":/images/ic_skip_previous_white_18dp_2x.png" );
    QskGraphic prevTuneGraphic = QskGraphic::fromImage( prevTuneImage );
    prevTuneButton->setGraphic( prevTuneGraphic );

    QskPushButton* prevSearchButton = new QskPushButton( tuneLayout );
    QImage prevSearchImage( ":/images/ic_fast_rewind_white_18dp_2x.png" );
    QskGraphic prevSearchGraphic = QskGraphic::fromImage( prevSearchImage );
    prevSearchButton->setGraphic( prevSearchGraphic );

    QskPushButton* nextSearchButton = new QskPushButton( tuneLayout );
    QImage nextSearchImage( ":/images/ic_fast_forward_white_18dp_2x.png" );
    QskGraphic nextSearchGraphic = QskGraphic::fromImage( nextSearchImage );
    nextSearchButton->setGraphic( nextSearchGraphic );

    QskPushButton* nextTuneButton = new QskPushButton( tuneLayout );
    QImage nextTuneImage( ":/images/ic_skip_next_white_18dp_1x.png" );
    QskGraphic nextTuneGraphic = QskGraphic::fromImage( nextTuneImage );
    nextTuneButton->setGraphic( nextTuneGraphic );
}
