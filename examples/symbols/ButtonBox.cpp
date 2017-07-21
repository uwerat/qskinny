/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <QskAspect.h>
#include <QskPushButton.h>
#include <QskCorner.h>
#include <QskGraphic.h>
#include <QskObjectCounter.h>
#include <SkinnyShapeFactory.h>
#include "ButtonBox.h"

ButtonBox::ButtonBox( QQuickItem* parent ):
    QskLinearBox( parent )
{
    setDimension( 3 );
    setMargins( 5 );

    insertButton( "RoyalBlue", SkinnyShapeFactory::Rectangle );
    insertButton( "IndianRed", SkinnyShapeFactory::Ellipse );
    insertButton( "DarkOliveGreen", SkinnyShapeFactory::Ring );
    insertButton( "SandyBrown", SkinnyShapeFactory::TriangleDown );
    insertButton( "DarkViolet", SkinnyShapeFactory::Star );
    insertButton( "DarkSlateGray", SkinnyShapeFactory::Hexagon );

    itemAtIndex( 0 )->setFocus( true );
}

ButtonBox::~ButtonBox()
{
}

void ButtonBox::insertButton( const QColor& color, SkinnyShapeFactory::Shape shape )
{
    QskGraphic graphic;

    QPen pen( Qt::black, 3 );
    pen.setJoinStyle( Qt::MiterJoin );
    pen.setCosmetic( true );

    QPainter painter( &graphic );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setPen( pen );
    painter.setBrush( color );

    painter.drawPath( SkinnyShapeFactory::shapePath( shape, QSizeF( 40, 40 ) ) );
    painter.end();

    QskPushButton* button = new QskPushButton( this );
    button->setObjectName( QString( "Shape %1" ).arg( int( shape ) ) );
    button->setGraphic( graphic );
    button->setMetric( QskPushButton::Panel | QskAspect::Border, 3 );
    //button->setCorner( QskCorner( Qt::RelativeSize, 100 ) );
    button->setSizePolicy( QskSizePolicy::MinimumExpanding,
        QskSizePolicy::MinimumExpanding );

    connect( button, SIGNAL( clicked() ), SLOT( onButtonClick() ) );
}

void ButtonBox::onButtonClick()
{
    delete sender();
}

#include "moc_ButtonBox.cpp"
