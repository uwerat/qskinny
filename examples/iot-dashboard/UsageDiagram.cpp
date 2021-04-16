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

    m_diagram->setTypes( Diagram::Line | Diagram::Area );
    m_diagram->setChartPosition( Qsk::Bottom );
    int number = 100;
    QVector<QPointF> dataPoints1;
    dataPoints1.reserve( number );
    std::vector<qreal> yValues1 = {40, 20, 30, 50, 30, 70, 80, 100, 90, 60};
    qreal t0 = yValues1[0];
    qreal step = 10;
    boost::math::cubic_b_spline<qreal> spline1( yValues1.data(), yValues1.size(), t0, step );

    for( int x = 0; x < number; ++x )
    {
        qreal y = spline1( x );
        dataPoints1.append( QPointF( x, y ) );
    }

    m_diagram->setDataPoints( dataPoints1 );
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

#include "Diagram.moc"
