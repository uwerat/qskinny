/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskBox.h"
#include "QskMargins.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskGradient.h"

QSK_SUBCONTROL( QskBox, Panel )

QskBox::QskBox( QQuickItem* parent )
    : QskBox( true, parent )
{
}

QskBox::QskBox( bool hasPanel, QQuickItem* parent )
    : Inherited( parent )
    , m_hasPanel( hasPanel )
{
}

QskBox::~QskBox()
{
}

void QskBox::setPanel( bool on )
{
    if ( on != m_hasPanel )
    {
        m_hasPanel = on;

        resetImplicitSize();
        polish();
        update();
    }
}

bool QskBox::hasPanel() const
{
    return m_hasPanel;
}

QskBoxBorderMetrics QskBox::borderMetrics() const
{
    return boxBorderMetricsHint( Panel );
}

void QskBox::setBorderMetrics( const QskBoxBorderMetrics& metrics )
{
    if ( setBoxBorderMetricsHint( Panel, metrics ) )
        Q_EMIT borderMetricsChanged( borderMetrics() );
}

void QskBox::resetBorderMetrics()
{
    if ( resetBoxBorderMetricsHint( Panel ) )
        Q_EMIT borderMetricsChanged( borderMetrics() );
}

QskBoxBorderColors QskBox::borderColors() const
{
    return boxBorderColorsHint( Panel );
}

void QskBox::setBorderColors( const QskBoxBorderColors& colors )
{
    if ( setBoxBorderColorsHint( Panel, colors ) )
        Q_EMIT borderColorsChanged( borderColors() );
}

void QskBox::resetBorderColors()
{
    if ( resetBoxBorderColorsHint( Panel ) )
        Q_EMIT borderColorsChanged( borderColors() );
}

QskGradient QskBox::fillGradient() const
{
    return gradientHint( Panel );
}

void QskBox::setFillGradient( const QskGradient& gradient )
{
    if ( setGradientHint( Panel, gradient ) )
        Q_EMIT fillGradientChanged( gradient );
}

void QskBox::resetFillGradient()
{
    if ( resetColor( Panel ) )
        Q_EMIT fillGradientChanged( fillGradient() );
}

void QskBox::setPadding( qreal padding )
{
    setPadding( QskMargins( padding ) );
}

void QskBox::setPadding( const QMarginsF& padding )
{
    const auto pd = QskMargins().expandedTo( padding );

    if ( setPaddingHint( Panel, pd ) )
        Q_EMIT paddingChanged( pd );
}

void QskBox::resetPadding()
{
    if ( resetPaddingHint( Panel ) )
        Q_EMIT paddingChanged( paddingHint( Panel ) );
}

QMarginsF QskBox::padding() const
{
    return paddingHint( Panel );
}

QRectF QskBox::layoutRectForSize( const QSizeF& size ) const
{
    if ( m_hasPanel )
        return subControlContentsRect( size, Panel );

    return Inherited::layoutRectForSize( size );
}

#include "moc_QskBox.cpp"
