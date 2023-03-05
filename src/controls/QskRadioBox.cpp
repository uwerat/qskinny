/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskRadioBox.h"
#include "QskEvent.h"
#include "QskAnimationHint.h"
#include "QskSkinlet.h"

QSK_SUBCONTROL( QskRadioBox, Panel )
QSK_SUBCONTROL( QskRadioBox, Button )
QSK_SUBCONTROL( QskRadioBox, CheckIndicatorPanel )
QSK_SUBCONTROL( QskRadioBox, CheckIndicator )
QSK_SUBCONTROL( QskRadioBox, Text )
QSK_SUBCONTROL( QskRadioBox, Ripple )

QSK_STATE( QskRadioBox, Selected, QskAspect::FirstUserState << 1 )
QSK_STATE( QskRadioBox, Pressed, QskAspect::FirstUserState << 2 )

class QskRadioBox::PrivateData
{
  public:
    QStringList options;

    int selectedIndex = -1;
    int focusedIndex = -1;
    int pressedIndex = -1;
};

QskRadioBox::QskRadioBox( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Minimum );

    setFocusPolicy( Qt::StrongFocus );
    setAcceptedMouseButtons( Qt::LeftButton );

    setFocusedIndex( -1 );
}

QskRadioBox::QskRadioBox( const QStringList& options, QQuickItem* parent )
    : QskRadioBox( parent )
{
    setOptions( options );
}

QskRadioBox::QskRadioBox( const QStringList& items,
        int selectedIndex, QQuickItem* parent )
    : QskRadioBox( items, parent )
{
    if( selectedIndex >= 0 && selectedIndex < items.count() )
        m_data->selectedIndex = selectedIndex;
}

QskRadioBox::~QskRadioBox()
{
}

QRectF QskRadioBox::focusIndicatorRect() const
{
    if ( m_data->focusedIndex < 0 )
        return QRectF();

    const auto rect = contentsRect();

    auto skinlet = effectiveSkinlet();

    const auto panelRect = skinlet->sampleRect( this,
        rect, QskRadioBox::CheckIndicatorPanel, m_data->focusedIndex );

    auto y = panelRect.y();
    auto h = panelRect.height();

    const auto textRect = skinlet->sampleRect( this,
        rect, QskRadioBox::Text, m_data->focusedIndex );

    if( textRect.height() > 0.0 )
    {
        y = qMin( y, textRect.y() );
        h = qMax( h, textRect.height() );
    }

    return QRectF( rect.x(), y, rect.width(), h );
}

int QskRadioBox::selectedIndex() const
{
    return m_data->selectedIndex;
}

QStringList QskRadioBox::options() const
{
    return m_data->options;
}

QString QskRadioBox::optionAt( int index ) const
{
    return m_data->options.value( index );
}

int QskRadioBox::pressedIndex() const
{
    return m_data->pressedIndex;
}

void QskRadioBox::setSelectedIndex( int index )
{
    if( index == m_data->selectedIndex || index >= m_data->options.count() )
        return;

    if( index < 0 )
        m_data->selectedIndex = -1;
    else
        m_data->selectedIndex = index;

    Q_EMIT selectedIndexChanged( m_data->selectedIndex );
}

void QskRadioBox::setOptions( const QStringList& options )
{
    if( m_data->options == options )
        return;

    m_data->options = options;

    Q_EMIT optionsChanged( options );
    setSelectedIndex( m_data->selectedIndex );

    if( m_data->focusedIndex > options.size() )
        setFocusedIndex( 0 );
}

void QskRadioBox::keyPressEvent( QKeyEvent* event )
{
    switch ( event->key() )
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
        {
            m_data->selectedIndex = qMax( m_data->selectedIndex - 1, 0 );
            setFocusedIndex( m_data->selectedIndex );
            update();

            return;
        }
        case Qt::Key_Down:
        case Qt::Key_Right:
        {
            m_data->selectedIndex = qMin( m_data->selectedIndex + 1,
                m_data->options.size() - 1 );

            setFocusedIndex( m_data->selectedIndex );
            update();

            return;
        }
        case Qt::Key_Select:
        case Qt::Key_Return:
        case Qt::Key_Space:
        {
            m_data->selectedIndex = m_data->focusedIndex;
            update();

            return;
        }
    }

    const auto currentTabIndex = m_data->focusedIndex;
    const auto nextTabIndex = currentTabIndex + qskFocusChainIncrement( event );

    if( nextTabIndex >= m_data->options.size() || nextTabIndex < 0 )
    {
        Inherited::keyPressEvent( event );
        setFocusedIndex( -1 );
    }
    else
    {
        setFocusedIndex( ( float ) nextTabIndex );

        const auto aspect = Ripple | QskAspect::Metric | QskAspect::Position;
        const auto hint = animationHint( aspect | skinStates() );

        if( hint.isValid() )
        {
            startTransition( aspect, hint,
                ( float ) currentTabIndex, ( float ) nextTabIndex );
        }
    }

    update();
}

void QskRadioBox::keyReleaseEvent( QKeyEvent* )
{
}

void QskRadioBox::mousePressEvent( QMouseEvent* event )
{
    auto indexAtPosition = indexAt( qskMousePosition( event ) );

    m_data->pressedIndex = indexAtPosition;
    m_data->selectedIndex = -1;

    setFocusedIndex( indexAtPosition );
    update();
}

void QskRadioBox::mouseUngrabEvent()
{
    if ( m_data->pressedIndex >= 0 )
    {
        m_data->pressedIndex = -1;
        update();
    }
}

void QskRadioBox::mouseReleaseEvent( QMouseEvent* event )
{
    const auto index = indexAt( qskMousePosition( event ) );
    if( index == m_data->pressedIndex )
        setSelectedIndex( index );

    m_data->pressedIndex = -1;
    update();
}

void QskRadioBox::focusInEvent( QFocusEvent* event )
{
    if( event->reason() == Qt::TabFocusReason )
    {
        setFocusedIndex( 0 );
    }
    else if( event->reason() == Qt::BacktabFocusReason )
    {
        setFocusedIndex( m_data->options.size() - 1 );
    }

    update();
    Inherited::focusInEvent( event );
}

void QskRadioBox::focusOutEvent( QFocusEvent* event )
{
    setFocusedIndex( -1 );
    update();

    Inherited::focusOutEvent( event );
}

int QskRadioBox::indexAt( const QPointF& pos ) const
{
    return effectiveSkinlet()->sampleIndexAt( this,
        contentsRect(), QskRadioBox::Button, pos );
}

void QskRadioBox::setFocusedIndex( int index )
{
    if ( m_data->focusedIndex == index )
        return;

    m_data->focusedIndex = index;
    setPositionHint( Ripple, index );

    Q_EMIT focusIndicatorRectChanged();
}

#include "moc_QskRadioBox.cpp"
