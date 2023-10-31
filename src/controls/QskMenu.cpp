/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMenu.h"

#include "QskGraphicProvider.h"
#include "QskLabelData.h"
#include "QskTextOptions.h"
#include "QskGraphic.h"
#include "QskColorFilter.h"
#include "QskSkinlet.h"
#include "QskEvent.h"
#include "QskPlatform.h"

#include <qvector.h>
#include <qvariant.h>
#include <qeventloop.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskMenu, Overlay )
QSK_SUBCONTROL( QskMenu, Panel )
QSK_SUBCONTROL( QskMenu, Segment )
QSK_SUBCONTROL( QskMenu, Cursor )
QSK_SUBCONTROL( QskMenu, Text )
QSK_SUBCONTROL( QskMenu, Icon )
QSK_SUBCONTROL( QskMenu, Separator )

QSK_SYSTEM_STATE( QskMenu, Selected, QskAspect::FirstSystemState << 2 )
QSK_SYSTEM_STATE( QskMenu, Pressed, QskAspect::FirstSystemState << 3 )

static inline int qskActionIndex( const QVector< int >& actions, int index )
{
    if ( index < 0 )
        return -1;

    auto it = std::lower_bound( actions.constBegin(), actions.constEnd(), index );
    return it - actions.constBegin();
}

class QskMenu::PrivateData
{
  public:
    QPointF origin;

    QVector< QskLabelData > options;

    QVector< int > separators;
    QVector< int > actions;

    int triggeredIndex = -1;
    int currentIndex = -1;
    bool isPressed = false;
};

QskMenu::QskMenu( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
    setModal( true );

    setPopupFlag( QskPopup::CloseOnPressOutside, true );
    setPopupFlag( QskPopup::DeleteOnClose, true );

    setPlacementPolicy( QskPlacementPolicy::Ignore );
    setSubcontrolProxy( Inherited::Overlay, Overlay );

    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    // we hide the focus indicator while sliding
    connect( this, &QskPopup::transitioningChanged,
        this, &QskControl::focusIndicatorRectChanged );

    connect( this, &QskPopup::transitioningChanged,
        this, &QQuickItem::setClip );

    connect( this, &QskPopup::opened, this,
        [this]() { m_data->triggeredIndex = -1; } );

    setAcceptHoverEvents( true );
}

QskMenu::~QskMenu()
{
}

QRectF QskMenu::clipRect() const
{
    if ( isTransitioning() )
    {
        constexpr qreal d = 1e6;
        return QRectF( -d, m_data->origin.y() - y(), 2.0 * d, d );
    }

    return Inherited::clipRect();
}

#if 1

// has no effect as we do not offer submenus yet. TODO ...
bool QskMenu::isCascading() const
{
    return flagHint( QskMenu::Panel | QskAspect::Style, qskMaybeDesktopPlatform() );
}

void QskMenu::setCascading( bool on )
{
    if ( setFlagHint( QskMenu::Panel | QskAspect::Style, on ) )
        Q_EMIT cascadingChanged( on );
}

void QskMenu::resetCascading()
{
    if ( resetSkinHint( QskMenu::Panel | QskAspect::Style ) )
        Q_EMIT cascadingChanged( isCascading() );
}

#endif

void QskMenu::setOrigin( const QPointF& origin )
{
    if ( origin != m_data->origin )
    {
        m_data->origin = origin;
        Q_EMIT originChanged( origin );
    }
}

QPointF QskMenu::origin() const
{
    return m_data->origin;
}

void QskMenu::setTextOptions( const QskTextOptions& textOptions )
{
    setTextOptionsHint( Text, textOptions );
}

QskTextOptions QskMenu::textOptions() const
{
    return textOptionsHint( Text );
}

int QskMenu::addOption( const QString& graphicSource, const QString& text )
{
    return addOption( QskLabelData( text, graphicSource ) );
}

int QskMenu::addOption( const QUrl& graphicSource, const QString& text )
{
    return addOption( QskLabelData( text, graphicSource ) );
}

int QskMenu::addOption( const QskLabelData& option )
{
    const int index = m_data->options.count();

    m_data->options += option;

    if ( option.isEmpty() )
        m_data->separators += index;
    else
        m_data->actions += index;

    resetImplicitSize();
    update();

    if ( isComponentComplete() )
        Q_EMIT optionsChanged();

    return index;
}

void QskMenu::setOptions( const QStringList& options )
{
    setOptions( qskCreateLabelData( options ) );
}

void QskMenu::setOptions( const QVector< QskLabelData >& options )
{
    m_data->options = options;

    for ( int i = 0; i < options.count(); i++ )
    {
        if ( options[i].isEmpty() )
            m_data->separators += i;
        else
            m_data->actions += i;
    }

    if ( m_data->currentIndex >= 0 )
    {
        m_data->currentIndex = -1;

        if ( isComponentComplete() )
            Q_EMIT currentIndexChanged( m_data->currentIndex );
    }

    resetImplicitSize();
    update();

    if ( isComponentComplete() )
        Q_EMIT optionsChanged();
}

void QskMenu::clear()
{
    setOptions( QVector< QskLabelData >() );
}

QVector< QskLabelData > QskMenu::options() const
{
    return m_data->options;
}

QskLabelData QskMenu::optionAt( int index ) const
{
    return m_data->options.value( index );
}

void QskMenu::addSeparator()
{
    addOption( QskLabelData() );
}

QVector< int > QskMenu::separators() const
{
    return m_data->separators;
}

QVector< int > QskMenu::actions() const
{
    return m_data->actions;
}

int QskMenu::currentIndex() const
{
    return m_data->currentIndex;
}

void QskMenu::setCurrentIndex( int index )
{
    if( index < 0 || index >= m_data->options.count() )
    {
        index = -1;
    }
    else
    {
        if ( m_data->options[index].isEmpty() ) // a seperator
            index = -1;
    }

    if( index != m_data->currentIndex )
    {
        setPositionHint( Cursor, index );

        m_data->currentIndex = index;
        update();

        Q_EMIT currentIndexChanged( index );
        Q_EMIT focusIndicatorRectChanged();
    }
}

QString QskMenu::currentText() const
{
    return optionAt( m_data->currentIndex ).text();
}

int QskMenu::triggeredIndex() const
{
    return m_data->triggeredIndex;
}

QString QskMenu::triggeredText() const
{
    return optionAt( m_data->triggeredIndex ).text();
}

void QskMenu::updateResources()
{
    if ( isTransitioning() )
    {
        const auto dy = ( 1.0 - transitioningFactor() ) * height();
        setPosition( m_data->origin.x(), m_data->origin.y() - dy );
    }

    Inherited::updateResources();
}

void QskMenu::updateNode( QSGNode* node )
{
    if ( isTransitioning() && clip() )
    {
        if ( auto clipNode = QQuickItemPrivate::get( this )->clipNode() )
        {
            /*
                The clipRect is changing while transitioning. Couldn't
                find a way how to trigger updates - maybe be enabling/disabling
                the clip. So we do the updates manually. TODO ...
             */
            const auto r = clipRect();
            if ( r != clipNode->rect() )
            {
                clipNode->setRect( r );
                clipNode->update();
            }
        }
    }

    Inherited::updateNode( node );
}

void QskMenu::keyPressEvent( QKeyEvent* event )
{
    if( m_data->currentIndex < 0 )
        return;

    switch( event->key() )
    {
        case Qt::Key_Up:
        {
            traverse( -1 );
            return;
        }

        case Qt::Key_Down:
        {
            traverse( 1 );
            return;
        }

        case Qt::Key_Select:
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
        {
            m_data->isPressed = true;
            return;
        }

        default:
        {
            if ( const int steps = qskFocusChainIncrement( event ) )
            {
                traverse( steps );
                return;
            }
        }
    }

    return Inherited::keyPressEvent( event );
}

void QskMenu::keyReleaseEvent( QKeyEvent* )
{
    if( isPressed() )
    {
        m_data->isPressed = false;

        if ( m_data->currentIndex >= 0 )
        {
            close();
            trigger( m_data->currentIndex );
        }
    }
}

void QskMenu::hoverEnterEvent( QHoverEvent* event )
{
    using A = QskAspect;

    setSkinHint( Segment | Hovered | A::Metric | A::Position, qskHoverPosition( event ) );
    update();
}

void QskMenu::hoverMoveEvent( QHoverEvent* event )
{
    using A = QskAspect;

    setSkinHint( Segment | Hovered | A::Metric | A::Position, qskHoverPosition( event ) );
    update();
}

void QskMenu::hoverLeaveEvent( QHoverEvent* )
{
    using A = QskAspect;

    setSkinHint( Segment | Hovered | A::Metric | A::Position, QPointF() );
    update();
}

#ifndef QT_NO_WHEELEVENT

void QskMenu::wheelEvent( QWheelEvent* event )
{
    const auto steps = qskWheelSteps( event );
    traverse( -steps );
}

#endif

void QskMenu::traverse( int steps )
{
    const auto& actions = m_data->actions;
    const auto count = actions.count();

    // -1 -> only one entry ?
    if ( actions.isEmpty() || ( steps % count == 0 ) )
        return;

    int action1 = qskActionIndex( actions, m_data->currentIndex );
    int action2 = action1 + steps;

    // when cycling we want to slide in
    int index1;

    if ( action2 < 0 )
        index1 = m_data->options.count();
    else if ( action2 >= count )
        index1 = -1;
    else
        index1 = actions[ action1 ];

    action2 %= count;
    if ( action2 < 0 )
        action2 += count;

    const auto index2 = actions[ action2 ];

    movePositionHint( Cursor, index1, index2 );
    setCurrentIndex( index2 );
}

void QskMenu::mousePressEvent( QMouseEvent* event )
{
    // QGuiApplication::styleHints()->setFocusOnTouchRelease ??

    if ( event->button() == Qt::LeftButton )
    {
        const auto index = indexAtPosition( qskMousePosition( event ) );
        if ( index >= 0 )
        {
            setCurrentIndex( index );
            m_data->isPressed = true;
        }

        return;
    }

    Inherited::mousePressEvent( event );
}

void QskMenu::mouseUngrabEvent()
{
    m_data->isPressed = false;
    Inherited::mouseUngrabEvent();
}

void QskMenu::mouseReleaseEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton )
    {
        if( isPressed() )
        {
            m_data->isPressed = false;

            const auto index = m_data->currentIndex;

            if ( ( index >= 0 )
                && ( index == indexAtPosition( qskMousePosition( event ) ) ) )
            {
                close();
                trigger( m_data->currentIndex );
            }
        }

        return;
    }

    Inherited::mouseReleaseEvent( event );
}

void QskMenu::aboutToShow()
{
    setSize( sizeConstraint() );

    if ( m_data->currentIndex < 0 )
    {
        if ( !m_data->actions.isEmpty() )
            setCurrentIndex( m_data->actions.first() );
    }

    Inherited::aboutToShow();
}

QRectF QskMenu::focusIndicatorRect() const
{
    if ( isTransitioning() )
        return QRectF();

    if( currentIndex() >= 0 )
    {
        auto actionIndex = qskActionIndex( m_data->actions, currentIndex() );

        return effectiveSkinlet()->sampleRect( this,
            contentsRect(), Segment, actionIndex );
    }

    return Inherited::focusIndicatorRect();
}

QRectF QskMenu::cellRect( int index ) const
{
    const auto actionIndex = qskActionIndex( m_data->actions, index );

    return effectiveSkinlet()->sampleRect(
        this, contentsRect(), QskMenu::Segment, actionIndex );
}

int QskMenu::indexAtPosition( const QPointF& pos ) const
{
    const auto index = effectiveSkinlet()->sampleIndexAt(
        this, contentsRect(), QskMenu::Segment, pos );

    return m_data->actions.value( index, -1 );
}

bool QskMenu::isPressed() const
{
    return m_data->isPressed;
}

void QskMenu::trigger( int index )
{
    if ( index >= 0 && index < m_data->options.count() )
    {
        m_data->triggeredIndex = index;
        Q_EMIT triggered( index );
    }
}

int QskMenu::exec()
{
    (void) execPopup();
    return m_data->triggeredIndex;
}

#include "moc_QskMenu.cpp"
