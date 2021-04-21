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

#include "CircularProgressBar.h"

#include <QPainter>

CircularProgressBar::CircularProgressBar( const QskGradient& gradient, int progress, QQuickItem* parent )
    : QQuickPaintedItem( parent )
    , m_progress( progress )
{
    // This is a bit hackish, but let's do this properly
    // once QSkinny has an arc renderer in place
    QLinearGradient g( 0, 0, 30, 0 );
    QGradientStop stop1( 0.0, gradient.colorAt( 0 ) );
    QGradientStop stop2( 1.0, gradient.colorAt( 1 ) );
    g.setStops( {stop1, stop2} );
    m_gradient = g;

    connect( this, &QQuickPaintedItem::contentsSizeChanged, [this]()
    {
        auto size = contentsSize();
        QRadialGradient ringGradient( size.width() / 2, size.height() / 2, 45 );
        QGradientStop stop1( 0.0, "#c0c0c0" );
        QGradientStop stop2( 0.5, "#f0f0f0" );
        QGradientStop stop3( 1.0, "#c0c0c0" );
        ringGradient.setStops( {stop1, stop2, stop3} );

        m_ringGradient = ringGradient;
    } );
}

void CircularProgressBar::paint( QPainter* painter )
{
    auto size = contentsSize();
    QRectF outerRect( {0, 0}, size );

    painter->setRenderHint( QPainter::Antialiasing, true );

    painter->setBrush( m_ringGradient );
    painter->setPen( m_backgroundColor );
    painter->drawEllipse( outerRect );

    int startAngle = 1440;
    int endAngle = -16 * ( m_progress / 100.0 ) * 360;

    painter->setBrush( m_gradient );
    painter->drawPie( outerRect, startAngle, endAngle );

    painter->setBrush( m_backgroundColor );
    painter->setPen( m_backgroundColor );
    QRectF innerRect( width() / 2, width() / 2, size.width() - width(), size.height() - width() );
    painter->drawEllipse( innerRect );
}
