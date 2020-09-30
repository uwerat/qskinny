#include "PieChartPainted.h"

#include <QskBox.h>
#include <QskSetup.h>
#include <QskSkin.h>
#include <QskTextLabel.h>

#include <QFontMetricsF>
#include <QQuickPaintedItem>

QSK_SUBCONTROL( PieChartPainted, Panel )

PieChartPainted::PieChartPainted( const QColor& color, const QGradient& gradient, int progress, int value, QQuickItem* parent )
    : QskControl( parent )
    , m_color( color )
    , m_gradient( gradient )
    , m_progressBar( new CircularProgressBar( gradient, progress, this ) )
    , m_progressLabel( new QskTextLabel( this ) )
//    , m_numberLabel(new QskTextLabel(QString::number(value), this))
//    , m_unitLabel(new QskTextLabel("kwH", this))
{
    setAutoLayoutChildren( true );

    auto progressText = QString::number( progress ) + " %";
    m_progressLabel->setText( progressText );
    m_progressLabel->setFontRole( QskSkin::SmallFont );
    m_progressLabel->setTextColor( color );

    const QColor c = this->color( Panel );
    m_progressBar->setBackgroundColor( c );

    connect( qskSetup, &QskSetup::skinChanged, [this]()
    {
        const QColor c = this->color( Panel );
        m_progressBar->setBackgroundColor( c );

        QRadialGradient gradient = m_progressBar->ringGradient();
        QRadialGradient newGradient = gradient;

        for( const QGradientStop& stop : gradient.stops() )
        {
            QColor s = stop.second;
            QColor newColor = { 255 - s.red(), 255 - s.green(), 255 - s.blue()};
            newGradient.setColorAt( stop.first, newColor );
        }

        m_progressBar->setRingGradient( newGradient );
    } );
}

QskAspect::Subcontrol PieChartPainted::effectiveSubcontrol( QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskBox::Panel )
    {
        return PieChartPainted::Panel;
    }

    return subControl;
}

QSizeF PieChartPainted::contentsSizeHint( Qt::SizeHint /*sizeHint*/, const QSizeF& /*size*/ ) const
{
    return {57, 57};
}

void PieChartPainted::updateLayout()
{
    m_progressBar->setContentsSize( size().toSize() );
    m_progressBar->update();

    auto rect = contentsRect();
    QFontMetricsF progressMetrics( m_progressLabel->effectiveFont( QskTextLabel::Text ) );
    auto textWidth = progressMetrics.width( m_progressLabel->text() );
    auto posX = rect.width() / 2 - textWidth / 2;
    auto posY = rect.height() / 2 - progressMetrics.height() / 2;
    m_progressLabel->setPosition( {posX, posY} );
    m_progressLabel->setFixedWidth( textWidth );
}
