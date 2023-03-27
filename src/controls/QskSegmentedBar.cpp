/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSegmentedBar.h"

#include "QskLabelData.h"
#include "QskTextOptions.h"
#include "QskEvent.h"
#include "QskSkinlet.h"
#include "QskAspect.h"
#include "QskAnimationHint.h"

#include <QGuiApplication>
#include <QStyleHints>
#include <QFontMetricsF>

QSK_SUBCONTROL( QskSegmentedBar, Panel )
QSK_SUBCONTROL( QskSegmentedBar, Splash )
QSK_SUBCONTROL( QskSegmentedBar, Segment )
QSK_SUBCONTROL( QskSegmentedBar, Separator )
QSK_SUBCONTROL( QskSegmentedBar, Cursor )
QSK_SUBCONTROL( QskSegmentedBar, Text )
QSK_SUBCONTROL( QskSegmentedBar, Icon )

QSK_SYSTEM_STATE( QskSegmentedBar, Minimum, QskAspect::FirstSystemState << 1 )
QSK_SYSTEM_STATE( QskSegmentedBar, Maximum, QskAspect::FirstSystemState << 2 )
QSK_SYSTEM_STATE( QskSegmentedBar, Selected, QskAspect::FirstSystemState << 3 )
QSK_SYSTEM_STATE( QskSegmentedBar, Pressed, QskAspect::FirstSystemState << 4 )

class QskSegmentedBar::PrivateData
{
  public:
    PrivateData( Qt::Orientation orientation )
        : orientation( orientation )
    {
    }

    QVector< QskLabelData > options;
    QVector< bool > enabled;

    int selectedIndex = -1;
    int currentIndex = -1;
    int focusedIndex = -1;

    Qt::Orientation orientation;
    bool isPressed = false;
};

QskSegmentedBar::QskSegmentedBar( QQuickItem* parent )
    : QskSegmentedBar( Qt::Horizontal, parent )
{
}

QskSegmentedBar::QskSegmentedBar( Qt::Orientation orientation, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData( orientation ) )
{
    setAcceptHoverEvents( true );

    if( orientation == Qt::Horizontal )
        initSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::Fixed );
    else
        initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::MinimumExpanding );

    setAcceptedMouseButtons( Qt::LeftButton );
    setWheelEnabled( true );
    setFocusPolicy( Qt::StrongFocus );

    connect( this, &QskSegmentedBar::currentIndexChanged,
        this, &QskControl::focusIndicatorRectChanged );
}

QskSegmentedBar::~QskSegmentedBar()
{
}

void QskSegmentedBar::setOrientation( Qt::Orientation orientation )
{
    if( orientation != m_data->orientation )
    {
        setSizePolicy( sizePolicy( Qt::Vertical ), sizePolicy( Qt::Horizontal ) );
        m_data->orientation = orientation;

        resetImplicitSize();
        update();
    }
}

Qt::Orientation QskSegmentedBar::orientation() const
{
    return m_data->orientation;
}

void QskSegmentedBar::setTextOptions( const QskTextOptions& textOptions )
{
    setTextOptionsHint( Text, textOptions );
}

QskTextOptions QskSegmentedBar::textOptions() const
{
    return textOptionsHint( Text );
}

int QskSegmentedBar::addOption( const QUrl& graphicSource, const QString& text )
{
    return addOption( QskLabelData( text, graphicSource ) );
}

int QskSegmentedBar::addOption( const QskLabelData& option )
{
    m_data->options += option;
    m_data->enabled += true;

    resetImplicitSize();
    update();

    if ( isComponentComplete() )
        Q_EMIT optionsChanged();

    if ( count() == 1 )
        setSelectedIndex( 0 );

    return count() - 1;
}

QskLabelData QskSegmentedBar::optionAt( int index ) const
{
    return m_data->options.value( index );
}

void QskSegmentedBar::setOptions( const QStringList& options )
{
    setOptions( qskCreateLabelData( options ) );
}

void QskSegmentedBar::setOptions( const QVector< QskLabelData >& options )
{
    m_data->options = options;
    m_data->enabled.fill( true, options.count() );

    resetImplicitSize();
    update();

    // selectedIndex ???
    Q_EMIT optionsChanged();
}

QskAspect::Variation QskSegmentedBar::effectiveVariation() const
{
    return static_cast< QskAspect::Variation >( m_data->orientation );
}

void QskSegmentedBar::mousePressEvent( QMouseEvent* event )
{
    using A = QskAspect;

    const int index = indexAtPosition( qskMousePosition( event ) );

    if( isSegmentEnabled( index ) )
    {
        m_data->isPressed = true;

        if( ( focusPolicy() & Qt::ClickFocus ) == Qt::ClickFocus )
        {
            if( !QGuiApplication::styleHints()->setFocusOnTouchRelease() )
            {
                if( index != m_data->currentIndex )
                {
                    setCurrentIndex( index );
                    setFocusedIndex( index );
                }
            }
        }
    }

    const auto hint = animationHint( Splash | A::Color );

    if( hint.isValid() )
    {
        setSkinHint( Splash | A::Metric | A::Position, qskMousePosition( event ) );
        startTransition( Splash | A::Metric | A::Size, hint, 0.0, 1.0 );
    }
}

void QskSegmentedBar::mouseUngrabEvent()
{
    m_data->isPressed = false;
}

void QskSegmentedBar::mouseReleaseEvent( QMouseEvent* event )
{
    int index = -1;

    if( m_data->isPressed )
    {
        m_data->isPressed = false;
        index = indexAtPosition( qskMousePosition( event ) );
    }

    if( isSegmentEnabled( index ) )
    {
        if( ( focusPolicy() & Qt::ClickFocus ) == Qt::ClickFocus )
        {
            if( QGuiApplication::styleHints()->setFocusOnTouchRelease() )
            {
                if( index != m_data->currentIndex )
                    setCurrentIndex( index );
            }
        }
    }

    setSelectedIndex( index );
}

void QskSegmentedBar::keyPressEvent( QKeyEvent* event )
{
    switch( event->key() )
    {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
        {
            bool forwards;

            if ( m_data->orientation == Qt::Vertical )
                forwards = ( event->key() == Qt::Key_Down );
            else
                forwards = ( event->key() == Qt::Key_Right );

            const int index = nextIndex( m_data->focusedIndex, forwards );

            if ( index != m_data->focusedIndex )
            {
                if ( index >= 0 && index < count() )
                    setFocusedIndex( index );
            }

            return;
        }

        case Qt::Key_Select:
        case Qt::Key_Space:
        {
            setCurrentIndex( m_data->focusedIndex );
            // stop further processing
            return;
        }

        default:
        {
            const int steps = qskFocusChainIncrement( event );

            if( steps != 0 )
            {
                const int index = nextIndex( m_data->focusedIndex, steps > 0 );

                if( index != m_data->focusedIndex )
                    setFocusedIndex( index );

                if( index >= 0 )
                    return;
            }
        }
    }

    Inherited::keyPressEvent( event );
}

void QskSegmentedBar::keyReleaseEvent( QKeyEvent* event )
{
    if( event->key() == Qt::Key_Select || event->key() == Qt::Key_Space )
    {
        if( m_data->currentIndex >= 0 )
            setSelectedIndex( m_data->currentIndex );

        return;
    }

    Inherited::keyReleaseEvent( event );
}

void QskSegmentedBar::hoverEnterEvent( QHoverEvent* event )
{
    using A = QskAspect;

    setSkinHint( Segment | Hovered | A::Metric | A::Position, qskHoverPosition( event ) );
    update();
}

void QskSegmentedBar::hoverMoveEvent( QHoverEvent* event )
{
    using A = QskAspect;

    setSkinHint( Segment | Hovered | A::Metric | A::Position, qskHoverPosition( event ) );
    update();
}

void QskSegmentedBar::hoverLeaveEvent( QHoverEvent* )
{
    using A = QskAspect;

    setSkinHint( Segment | Hovered | A::Metric | A::Position, QPointF() );
    update();
}

void QskSegmentedBar::focusInEvent( QFocusEvent* event )
{
    int index = m_data->focusedIndex;

    switch( event->reason() )
    {
        case Qt::TabFocusReason:
        {
            index = nextIndex( -1, true );
            break;
        }

        case Qt::BacktabFocusReason:
        {
            index = nextIndex( -1, false );
            break;
        }

        default:
        {
            if( index < 0 || index >= count() )
                index = nextIndex( -1, true );
        }
    }

    if( index != m_data->focusedIndex )
        setFocusedIndex( index );

    Inherited::focusInEvent( event );
}

void QskSegmentedBar::focusOutEvent( QFocusEvent* event )
{
    setFocusedIndex( -1 );
    update();

    Inherited::focusOutEvent( event );
}

void QskSegmentedBar::clear()
{
    if( count() == 0 )
        return;

    m_data->options.clear();
    Q_EMIT optionsChanged();

    if( m_data->selectedIndex >= 0 )
    {
        m_data->selectedIndex = -1;
        Q_EMIT selectedIndexChanged( m_data->selectedIndex );
    }

    if( m_data->currentIndex >= 0 )
    {
        m_data->currentIndex = -1;
        Q_EMIT currentIndexChanged( m_data->currentIndex );
    }

    update();
}

void QskSegmentedBar::setCurrentIndex( int index )
{
    if ( !isSegmentEnabled( index ) )
        index = -1;

    if( index != m_data->currentIndex )
    {
        m_data->currentIndex = index;
        Q_EMIT currentIndexChanged( index );
    }
}

int QskSegmentedBar::currentIndex() const
{
    return m_data->currentIndex;
}

QString QskSegmentedBar::currentText() const
{
    if ( m_data->currentIndex >= 0 )
        return optionAt( m_data->currentIndex ).text();

    return QString();
}

void QskSegmentedBar::setSelectedIndex( int index )
{
    if ( !isSegmentEnabled( index ) )
        index = -1;

    if( index != m_data->selectedIndex )
    {
        const auto oldIndex = m_data->selectedIndex;
        m_data->selectedIndex = index;

        movePositionHint( Cursor, index );
        update();

        Q_EMIT selectedIndexChanged( index );

        setSkinStateFlag( Minimum, ( m_data->selectedIndex == 0 ) );
        setSkinStateFlag( Maximum, ( m_data->selectedIndex == count() - 1 ) );

        const auto states = skinStates();

        if ( oldIndex >= 0 )
            startHintTransitions( states | Selected, states, oldIndex );

        if ( index >= 0 )
            startHintTransitions( states, states | Selected, index );
    }
}

int QskSegmentedBar::selectedIndex() const
{
    return m_data->selectedIndex;
}

int QskSegmentedBar::nextIndex( int index, bool forwards ) const
{
    const auto& options = m_data->options;
    const int count = options.count();

    if( forwards )
    {
        if( index < 0 || index >= count )
            index = -1;

        while( ++index < count )
        {
            if( isSegmentEnabled( index ) )
                return index;
        }
    }
    else
    {
        if( index < 0 || index >= count )
            index = count;

        while( --index >= 0 )
        {
            if( isSegmentEnabled( index ) )
                return index;
        }
    }

    return -1;
}

QVector< QskLabelData > QskSegmentedBar::options() const
{
    return m_data->options;
}

int QskSegmentedBar::count() const
{
    return m_data->options.count();
}

void QskSegmentedBar::setSegmentEnabled( int index, bool enabled )
{
    auto& bitVector = m_data->enabled;

    if( ( index < 0 ) || ( index >= bitVector.count() )
        || ( bitVector[ index ] == enabled ) )
    {
        return;
    }

    bitVector[ index ] = enabled;

    if( !enabled )
    {
        if( m_data->currentIndex == index )
            setCurrentIndex( -1 );
    }

    update();
}

bool QskSegmentedBar::isSegmentEnabled( int index ) const
{
    return m_data->enabled.value( index, false );
}

int QskSegmentedBar::indexAtPosition( const QPointF& pos ) const
{
    return effectiveSkinlet()->sampleIndexAt( this,
        contentsRect(), QskSegmentedBar::Segment, pos );
}

QRectF QskSegmentedBar::focusIndicatorRect() const
{
    if( m_data->currentIndex >= 0 )
    {
        return effectiveSkinlet()->sampleRect( this,
            contentsRect(), QskSegmentedBar::Segment, m_data->currentIndex );
    }

    return Inherited::focusIndicatorRect();
}

void QskSegmentedBar::setFocusedIndex( int index )
{
    if ( m_data->focusedIndex == index )
        return;

    m_data->focusedIndex = index;
    setPositionHint( Segment | Focused, index );

    update();

    Q_EMIT focusIndicatorRectChanged();
}

#include "moc_QskSegmentedBar.cpp"
