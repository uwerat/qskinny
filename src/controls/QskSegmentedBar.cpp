/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSegmentedBar.h"

#include "QskGraphic.h"
#include "QskGraphicProvider.h"
#include "QskTextOptions.h"
#include "QskEvent.h"
#include "QskSkinlet.h"
#include "QskAspect.h"

#include <QGuiApplication>
#include <QStyleHints>
#include <QFontMetricsF>

QSK_SUBCONTROL( QskSegmentedBar, Panel )
QSK_SUBCONTROL( QskSegmentedBar, Segment )
QSK_SUBCONTROL( QskSegmentedBar, Cursor )
QSK_SUBCONTROL( QskSegmentedBar, Text )
QSK_SUBCONTROL( QskSegmentedBar, Graphic )

QSK_SYSTEM_STATE( QskSegmentedBar, Selected, QskAspect::FirstSystemState << 2 )

namespace
{
    class Option
    {
      public:
        Option() = default;

        Option( const QUrl& graphicSource, const QString& text )
            : graphicSource( graphicSource )
            , text( text )
        {
#if 1
            // lazy loading TODO ...
            if ( !graphicSource.isEmpty() )
                graphic = Qsk::loadGraphic( graphicSource );
#endif
        }

        QUrl graphicSource;
        QString text;

        QskGraphic graphic;

        bool isEnabled = true;
    };
}

class QskSegmentedBar::PrivateData
{
  public:
    PrivateData( Qt::Orientation orientation )
        : orientation( orientation )
    {
        textOptions.setElideMode( Qt::ElideMiddle );
    }

    void addOption( QskSegmentedBar* bar, const Option& option )
    {
        this->options += option;

        bar->resetImplicitSize();
        bar->update();

        Q_EMIT bar->countChanged();

        if ( this->options.count() == 1 )
            bar->setSelectedIndex( 0 );
    }

    QVector< Option > options;

    QskTextOptions textOptions;

    int selectedIndex = -1;
    int currentIndex = -1;

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
    if( textOptions != m_data->textOptions )
    {
        m_data->textOptions = textOptions;
        update();
    }
}

QskTextOptions QskSegmentedBar::textOptions() const
{
    return m_data->textOptions;
}

int QskSegmentedBar::addText( const QString& text )
{
    m_data->addOption( this, Option( QUrl(), text ) );
    return count() - 1;
}

int QskSegmentedBar::addGraphic( const QUrl& graphicSource )
{
    m_data->addOption( this, Option( graphicSource, QString() ) );
    return count() - 1;
}

QVariant QskSegmentedBar::optionAt( int index ) const
{
    const auto& options = m_data->options;

    if( index < 0 || index >= options.count() )
        return QVariantList();

    const auto& option = options[ index ];

    QVariant value;

    if ( option.graphicSource.isValid() )
        value = QVariant::fromValue( option.graphic );
    else
        value = QVariant::fromValue( option.text );

    return value;
}

QskAspect::Placement QskSegmentedBar::effectivePlacement() const
{
    return static_cast< QskAspect::Placement >( m_data->orientation );
}

void QskSegmentedBar::mousePressEvent( QMouseEvent* event )
{
    const int index = indexAtPosition( qskMousePosition( event ) );

    if( index < 0 || index >= count() || !m_data->options[ index ].isEnabled )
        return;

    m_data->isPressed = true;

    if( ( focusPolicy() & Qt::ClickFocus ) == Qt::ClickFocus )
    {
        if( !QGuiApplication::styleHints()->setFocusOnTouchRelease() )
        {
            if( index != m_data->currentIndex )
                setCurrentIndex( index );
        }
    }
}

void QskSegmentedBar::mouseReleaseEvent( QMouseEvent* event )
{
    int index = -1;

    if( m_data->isPressed )
    {
        m_data->isPressed = false;
        index = indexAtPosition( qskMousePosition( event ) );
    }

    if( index < 0 || !m_data->options[ index ].isEnabled )
        return;

    if( ( focusPolicy() & Qt::ClickFocus ) == Qt::ClickFocus )
    {
        if( QGuiApplication::styleHints()->setFocusOnTouchRelease() )
        {
            if( index != m_data->currentIndex )
                setCurrentIndex( index );
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

            const int index = nextIndex( m_data->selectedIndex, forwards );
            if ( index != m_data->selectedIndex )
            {
                if ( index >= 0 && index < count() )
                    setSelectedIndex( index );
            }

            return;
        }

        case Qt::Key_Select:
        case Qt::Key_Space:

            // stop further processing
            return;

        default:
        {
            const int steps = qskFocusChainIncrement( event );

            if( steps != 0 )
            {
                const int index = nextIndex( m_data->currentIndex, steps > 0 );

                if( index != m_data->currentIndex )
                    setCurrentIndex( index );

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

void QskSegmentedBar::focusInEvent( QFocusEvent* event )
{
    int index = m_data->currentIndex;

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

    if( index != m_data->currentIndex )
        setCurrentIndex( index );

    Inherited::focusInEvent( event );
}

void QskSegmentedBar::clear()
{
    if( count() == 0 )
        return;

    m_data->options.clear();
    Q_EMIT countChanged();

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
    const auto& options = m_data->options;

    if( ( index < 0 ) || ( index >= options.count() )
        || !options[ index ].isEnabled )
    {
        index = -1;
    }

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

void QskSegmentedBar::setSelectedIndex( int index )
{
    if( index < 0 || index >= m_data->options.count() )
    {
        index = -1;
    }
    else if ( !m_data->options[ index ].isEnabled )
    {
        index = -1; // ???
    }

    if( index != m_data->selectedIndex )
    {
        m_data->selectedIndex = index;

        movePositionHint( Cursor, index );
        update();

        Q_EMIT selectedIndexChanged( index );
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
            if( options[ index ].isEnabled )
                return index;
        }
    }
    else
    {
        if( index < 0 || index >= count )
            index = count;

        while( --index >= 0 )
        {
            if( options[ index ].isEnabled )
                return index;
        }
    }

    return -1;
}

int QskSegmentedBar::count() const
{
    return m_data->options.count();
}

void QskSegmentedBar::setSegmentEnabled( int index, bool enabled )
{
    auto& options = m_data->options;

    if( ( index < 0 ) || ( index >= options.count() )
        || ( options[ index ].isEnabled == enabled ) )
    {
        return;
    }

    options[ index ].isEnabled = enabled;

    if( !enabled )
    {
        if( m_data->currentIndex == index )
            setCurrentIndex( -1 );
    }

    update();
}

bool QskSegmentedBar::isSegmentEnabled( int index ) const
{
    const auto& options = m_data->options;

    if( index < 0 || index >= options.count() )
        return false;

    return options[ index ].isEnabled;
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

#include "moc_QskSegmentedBar.cpp"
