#include "MainContent.h"

#include "Card.h"
#include "PieChart.h"

#include <QskTextLabel.h>

MainContent::MainContent( QQuickItem *parent ) : QskLinearBox( Qt::Horizontal, parent )
{
    setSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );

    for( int a = 0; a < 4; ++a )
    {
        auto* column = new QskLinearBox( Qt::Vertical, this );
        m_columns.append( column );
    }

    auto* pieChart =  new PieChart;
    QVector< float > angles = { 60, 90, 150, 60 };
    pieChart->setAngles( angles );
    addCard( "Sample usage", pieChart, 0 );

    auto* barGraph = new QskTextLabel( "here bar graph" );
    addCard( "Power consumption", barGraph, 0 );

    auto* statistics = new QskTextLabel( "here detailed statistics" );
    addCard( "Detailed statistics", statistics, 1 );

    auto* users = new QskTextLabel( "here users" );
    addCard( "Users", users, 1 );
}

void MainContent::addCard( const QString &title, QskControl *content, int column )
{
    new Card( title, content, m_columns.at( column ) );
}
