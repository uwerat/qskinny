#include "UsageDiagram.h"

#include "Diagram.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskGridBox.h>
#include <QskSkin.h>
#include <QskTextLabel.h>

#include <QPainter>
#include <QPainterPath>

#include <cmath>
#include <boost/math/interpolators/cubic_b_spline.hpp>

QSK_SUBCONTROL( WeekdayLabel, Panel )
QSK_SUBCONTROL( WeekdayLabel, Text )
QSK_SUBCONTROL( WeekdayBox, Panel )

QSK_SUBCONTROL( CaptionColorBox, Panel )

QSK_SUBCONTROL( CaptionItem, Panel )
QSK_STATE( CaptionItem, Water, ( QskAspect::FirstUserState << 1 ) )
QSK_STATE( CaptionItem, Electricity, ( QskAspect::FirstUserState << 2 ) )
QSK_STATE( CaptionItem, Gas, ( QskAspect::FirstUserState << 3 ) )

QSK_SUBCONTROL( CaptionBox, Panel )

QSK_SUBCONTROL( UsageDiagram, Panel )

CaptionItem::CaptionItem( QskAspect::State state, QQuickItem* parent )
    : QskLinearBox( Qt::Horizontal, parent )
{
    setSpacing( 10 );
    auto* box = new CaptionColorBox( this );
    box->setSkinState( state );

    QString text;

    if( state == CaptionItem::Water )
    {
        text = "Water";
    }
    else if( state == CaptionItem::Electricity )
    {
        text = "Electricity";
    }
    else if( state == CaptionItem::Gas )
    {
        text = "Gas";
    }

    auto* textLabel = new QskTextLabel( text, this );
    textLabel->setFontRole( QskSkin::TinyFont );

    const qreal size = metric( CaptionColorBox::Panel | QskAspect::Size );
    box->setFixedSize( {size, size} );
}


static constexpr int segments = 7;

UsageDiagram::UsageDiagram( QQuickItem* parent )
    : Box( "", parent )
    , m_diagram( new Diagram( this ) )
    , m_weekdays( new QskGridBox( this ) )
{
    setAutoAddChildren( false );
    setAutoLayoutChildren( true );

    int number = 100;
    std::vector< std::vector<qreal> > yValues =
    {
        {40, 55, 60, 50, 40, 50, 75, 80, 100, 90},
        {30, 15, 30, 40, 60, 10, 70, 20, 40, 45},
        {60, 45, 60, 70, 10, 70, 20, 50, 20, 30}
    };

    for( int i = 0; i < 3; ++i )
    {
        auto y = yValues.at( i );

        QVector<QPointF> dataPoints;
        dataPoints.reserve( number );
        qreal t0 = y[0];
        qreal step = 10;
        boost::math::cubic_b_spline<qreal> spline( y.data(), y.size(), t0, step );

        for( int x = 0; x < number; ++x )
        {
            const qreal y = spline( x );
            QPointF p( x, qBound( 0.0, y, 100.0 ) );

            printf( "%.2f, ", y );

            dataPoints.append( p );
        }

        printf( "\n" );

        m_diagram->addDataPoints( dataPoints, Diagram::Area );
    }

    m_diagram->setYMax( 100 );
    addItem( m_diagram );


    m_weekdays->setSpacing( 0 );
    QStringList weekdays = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

    for( int i = 0; i < segments; ++i )
    {
        auto* box = new WeekdayBox( m_weekdays );
        m_weekdays->addItem( box, 0, i );
        auto* label = new WeekdayLabel( weekdays.at( i ), m_weekdays );
        m_weekdays->addItem( label, 1, i );
    }

    addItem( m_weekdays, Qt::AlignBottom );

    m_captionBox = new CaptionBox( this );
    addItem( m_captionBox, Qt::AlignRight );
    m_captionBox->setSizePolicy( QskSizePolicy::Maximum, QskSizePolicy::Maximum );
    m_captionBox->setSpacing( 30 );
    m_captionBox->addItem( new CaptionItem( CaptionItem::Water, this ) );
    m_captionBox->addItem( new CaptionItem( CaptionItem::Electricity, this ) );
    m_captionBox->addItem( new CaptionItem( CaptionItem::Gas, this ) );
}

void UsageDiagram::updateLayout()
{
    auto weekdaysHeight = m_weekdays->preferredSize().height();
    m_diagram->setHeight( m_diagram->height() - weekdaysHeight );
    const qreal captionX = width() - m_captionBox->width();
    m_captionBox->setPosition( {captionX, 0} );
}

#include "Diagram.moc"
