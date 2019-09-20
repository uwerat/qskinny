/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBox.h"
#include "QskSkinHintTable.h"

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

void QskBox::setPadding( qreal padding )
{
    setPadding( QMarginsF( padding, padding, padding, padding ) );
}

void QskBox::setPadding( const QMarginsF& padding )
{
    using namespace QskAspect;

    const QMarginsF pd(
        qMax( qreal( padding.left() ), qreal( 0.0 ) ),
        qMax( qreal( padding.top() ), qreal( 0.0 ) ),
        qMax( qreal( padding.right() ), qreal( 0.0 ) ),
        qMax( qreal( padding.bottom() ), qreal( 0.0 ) ) );

    if ( pd != this->padding() )
    {
        const auto subControl = effectiveSubcontrol( QskBox::Panel );

        setMarginsHint( subControl | Padding, pd );
        resetImplicitSize();

        if ( polishOnResize() || autoLayoutChildren() )
            polish();

        Q_EMIT paddingChanged( pd );
    }
}

void QskBox::resetPadding()
{
    using namespace QskAspect;
    const Aspect aspect = QskBox::Panel | Metric | Padding;

    const auto oldPadding = marginsHint( aspect );

    auto& table = hintTable();
    if ( table.hint( aspect ).isValid() )
    {
        table.removeHint( aspect );

        const auto padding = marginsHint( aspect );
        if ( padding != oldPadding )
        {
            resetImplicitSize();

            if ( polishOnResize() || autoLayoutChildren() )
                polish();

            Q_EMIT paddingChanged( padding );
        }
    }
}

QMarginsF QskBox::padding() const
{
    return marginsHint( QskBox::Panel | QskAspect::Padding );
}

QRectF QskBox::layoutRectForSize( const QSizeF& size ) const
{
    if ( !m_hasPanel )
        return Inherited::layoutRectForSize( size );

    return innerBox( Panel, subControlRect( size, Panel ) );
}

QSizeF QskBox::contentsSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( m_hasPanel && which == Qt::PreferredSize )
    {
        return QSizeF(
            metric( Panel | QskAspect::MinimumWidth ),
            metric( Panel | QskAspect::MinimumHeight ) );
    }

    return Inherited::contentsSizeHint( which, constraint );
}

#include "moc_QskBox.cpp"
