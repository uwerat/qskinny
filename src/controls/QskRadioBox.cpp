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
QSK_SUBCONTROL( QskRadioBox, Symbol )
QSK_SUBCONTROL( QskRadioBox, Text )
QSK_SUBCONTROL( QskRadioBox, Ripple )

QSK_STATE( QskRadioBox, Selected, QskAspect::FirstUserState << 1 )
QSK_STATE( QskRadioBox, Pressed, QskAspect::FirstUserState << 2 )
QSK_STATE( QskRadioBox, Focused, QskAspect::FirstUserState << 3 )

class QskRadioBox::PrivateData
{
  public:
    QStringList items;

    int selectedIndex = -1;
    int focusedIndex = -1;
    int pressedIndex = -1;
};

QskRadioBox::QskRadioBox( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setFocusPolicy( Qt::NoFocus );
    setAcceptedMouseButtons( Qt::LeftButton );

    connect(this, &QskRadioBox::itemsChanged, this,
        [this]( const QStringList& items )
            { setFocusPolicy( items.count() > 0 ? Qt::StrongFocus : Qt::NoFocus ); }
    );

    setFocusedIndex( -1 );
}

QskRadioBox::QskRadioBox( const QStringList& list, QQuickItem* parent )
    : QskRadioBox( parent )
{
    setItems( list );
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
    if( m_data->focusedIndex > -1)
    {
        auto skinlet = effectiveSkinlet();

        auto textRect = skinlet->sampleRect( this,
            contentsRect(), QskRadioBox::Text, m_data->focusedIndex );

        auto buttonRect = skinlet->sampleRect( this,
            contentsRect(), QskRadioBox::Button, m_data->focusedIndex );

        if( textRect == QRectF() )
            return buttonRect;

        auto result = QRectF(
            qMin( textRect.x(), buttonRect.x() ),
            qMin( textRect.y(), buttonRect.y() ),
            buttonRect.width() + textRect.width(),
            qMax( buttonRect.height(), textRect.height() ) );

        if( layoutMirroring() )
        {
            result.setWidth(
                result.width() + marginHint( Text ).right()
                + marginHint( Button ).left() );
        }
        else
        {
            result.setWidth(
                result.width() + marginHint( Text ).left()
                + marginHint( Button ).right() );
        }

        return result;
    }

    return QRectF();
}

int QskRadioBox::selectedIndex() const
{
    return m_data->selectedIndex;
}

const QStringList& QskRadioBox::items() const
{
    return m_data->items;
}

int QskRadioBox::pressedIndex() const
{
    return m_data->pressedIndex;
}

void QskRadioBox::setSelectedIndex( int index )
{
    if( index == m_data->selectedIndex || index >= m_data->items.count() )
        return;

    if( index < 0 )
        m_data->selectedIndex = -1;
    else
        m_data->selectedIndex = index;

    selectedIndexChanged( m_data->selectedIndex );
}

void QskRadioBox::setItems( const QStringList& items )
{
    if( m_data->items == items )
        return;

    m_data->items = items;

    itemsChanged( items );
    setSelectedIndex( m_data->selectedIndex );

    if( m_data->focusedIndex > items.size() )
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
                items().size() - 1 );
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

    if( nextTabIndex >= items().size() || nextTabIndex < 0 )
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

void QskRadioBox::mouseReleaseEvent( QMouseEvent* event )
{
    const auto index = indexAt( qskMousePosition( event ) );
    if( index == m_data->pressedIndex )
        setSelectedIndex( index );

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
        setFocusedIndex( items().size() - 1 );
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

int QskRadioBox::indexAt( const QPointF& target ) const
{
    const auto itemHeight = contentsRect().height() / items().size();
    auto index = target.y() / itemHeight;

    if( index < 0 || index >= items().size() )
        return -1;

    return index;
}

void QskRadioBox::setFocusedIndex( int index )
{
    m_data->focusedIndex = index;
    setPositionHint( Ripple, index );
    focusIndicatorRectChanged();
}

#include "moc_QskRadioBox.cpp"
