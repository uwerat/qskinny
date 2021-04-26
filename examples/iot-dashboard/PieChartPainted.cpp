/****************************************************************************
**
** Copyright 2021 Edelhirsch Software GmbH. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of the copyright holder nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

#include "PieChartPainted.h"

#include <QskAnimator.h>
#include <QskBox.h>
#include <QskRgbValue.h>
#include <QskSetup.h>
#include <QskSkin.h>
#include <QskTextLabel.h>

#include <QFontMetricsF>
#include <QGuiApplication>
#include <QQuickPaintedItem>
#include <QQuickWindow>

QSK_SUBCONTROL( PieChartPainted, Panel )

namespace
{
    QColor invertedColor( const QColor& c )
    {
        QColor ret = { 255 - c.red(), 255 - c.green(), 255 - c.blue()};
        return ret;
    }
}

// ### There must be an easier way to do this
class ProgressBarAnimator : public QskAnimator
{
    public:
        ProgressBarAnimator( PieChartPainted* pieChart, CircularProgressBar* progressBar )
            : m_pieChart( pieChart )
            , m_progressBar( progressBar )
        {
            QQuickWindow* w = static_cast<QQuickWindow*>( qGuiApp->allWindows().at( 0 ) );
            setWindow( w );
            setDuration( 500 );
            setEasingCurve( QEasingCurve::Linear );
            setAutoRepeat( false );
        }

        void setup() override
        {
            m_backgroundColor = m_pieChart->color( PieChartPainted::Panel );
            m_ringGradient = m_progressBar->ringGradient();
        }

        void advance( qreal value ) override
        {
            const QColor c = m_backgroundColor;
            const QColor c2 = invertedColor( c );
            const QColor newColor = QskRgb::interpolated( c2, c, value );
            m_progressBar->setBackgroundColor( newColor );

            QRadialGradient gradient = m_ringGradient;
            QRadialGradient newGradient = gradient;

            for( const QGradientStop& stop : gradient.stops() )
            {
                QColor c = stop.second;
                QColor c2 = invertedColor( c );
                const QColor newColor = QskRgb::interpolated( c, c2, value );
                newGradient.setColorAt( stop.first, newColor );
            }

            m_progressBar->setRingGradient( newGradient );
            m_progressBar->update();
        }

    private:
        QColor m_backgroundColor;
        QRadialGradient m_ringGradient;
        PieChartPainted* m_pieChart;
        CircularProgressBar* m_progressBar;
};

PieChartPainted::PieChartPainted( const QColor& color, const QskGradient& gradient, int progress, int /*value*/, QQuickItem* parent )
    : QskControl( parent )
    , m_color( color )
    , m_gradient( gradient )
    , m_progressBar( new CircularProgressBar( gradient, progress, this ) )
    , m_progressLabel( new QskTextLabel( this ) )
    , m_animator( new ProgressBarAnimator( this, m_progressBar ) )
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
        m_animator->start();
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
