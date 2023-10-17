/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskControl.h"
#include "QskControlPrivate.h"

#include "QskAspect.h"
#include "QskFunctions.h"
#include "QskEvent.h"
#include "QskQuick.h"
#include "QskSetup.h"
#include "QskSkin.h"
#include "QskSkinlet.h"
#include "QskSkinHintTable.h"
#include "QskMargins.h"

#include <qlocale.h>
#include <qvector.h>

QSK_SUBCONTROL( QskControl, Background )

QSK_SYSTEM_STATE( QskControl, Disabled, QskAspect::FirstSystemState )
QSK_SYSTEM_STATE( QskControl, Hovered, QskAspect::LastSystemState >> 1 )
QSK_SYSTEM_STATE( QskControl, Focused, QskAspect::LastSystemState )

static inline void qskSendEventTo( QObject* object, QEvent::Type type )
{
    QEvent event( type );
    QCoreApplication::sendEvent( object, &event );
}

static inline bool qskMaybeGesture( QQuickItem* item,
    const QQuickItem* child, const QEvent* event )
{
    if ( qskIsTouchOrMouseEvent( event->type() ) )
    {
        QskGestureFilterEvent ev( child, event );
        QCoreApplication::sendEvent( item, &ev );

        return ev.maybeGesture();
    }

    return false;
}

QskControl::QskControl( QQuickItem* parent )
    : QskQuickItem( *( new QskControlPrivate() ), parent )
{
    Inherited::setActiveFocusOnTab( false );

    if ( parent )
    {
        // inheriting attributes from parent
        QskControlPrivate::resolveLocale( this );
        QskControlPrivate::resolveSection( this );
    }
}

QskControl::~QskControl()
{
#if defined( QT_DEBUG )
    if ( auto w = window() )
    {
        if( this == w->mouseGrabberItem() )
        {
            // to catch suicide situations as a result of mouse clicks
            qWarning() << "QskControl::~QskControl: probably suicide situation detected, "
                          "control is the mouse grabber item" << this;
        }

    }
#endif
}

void QskControl::setAutoLayoutChildren( bool on )
{
    Q_D( QskControl );
    if ( on != d->autoLayoutChildren )
    {
        d->autoLayoutChildren = on;
        if ( on )
            polish();
    }
}

bool QskControl::autoLayoutChildren() const
{
    return d_func()->autoLayoutChildren;
}

void QskControl::setWheelEnabled( bool on )
{
    Q_D( QskControl );
    if ( on != d->isWheelEnabled )
    {
        d->isWheelEnabled = on;
        Q_EMIT wheelEnabledChanged();
    }
}

bool QskControl::isWheelEnabled() const
{
    return d_func()->isWheelEnabled;
}

void QskControl::setFocusPolicy( Qt::FocusPolicy policy )
{
    Q_D( QskControl );
    if ( policy != d->focusPolicy )
    {
        d->focusPolicy = ( policy & ~Qt::TabFocus );

        const bool tabFocus = policy & Qt::TabFocus;

        if ( !tabFocus && window() )
        {
            // Removing the activeFocusItem from the focus tab chain is not possible
            if ( window()->activeFocusItem() == this )
            {
                if ( auto focusItem = nextItemInFocusChain( true ) )
                    focusItem->setFocus( true );
            }
        }

        Inherited::setActiveFocusOnTab( tabFocus );

        Q_EMIT focusPolicyChanged();
    }
}

Qt::FocusPolicy QskControl::focusPolicy() const
{
    uint policy = d_func()->focusPolicy;
    if ( Inherited::activeFocusOnTab() )
        policy |= Qt::TabFocus;

    return static_cast< Qt::FocusPolicy >( policy );
}

void QskControl::setBackgroundColor( const QColor& color )
{
    setBackground( QskGradient( color ) );
}

void QskControl::setBackground( const QskGradient& gradient )
{
    if ( setGradientHint( QskControl::Background, gradient ) )
        Q_EMIT backgroundChanged();
}

void QskControl::resetBackground()
{
    if ( resetColor( QskControl::Background ) )
        Q_EMIT backgroundChanged();
}

QskGradient QskControl::background() const
{
    return gradientHint( QskControl::Background );
}

void QskControl::setMargins( qreal margin )
{
    setMargins( QskMargins( margin ) );
}

void QskControl::setMargins( qreal left, qreal top, qreal right, qreal bottom )
{
    setMargins( QskMargins( left, top, right, bottom ) );
}

void QskControl::setMargins( const QMarginsF& margins )
{
    const auto m = QskMargins().expandedTo( margins );

    if ( setPaddingHint( QskControl::Background, m ) )
    {
        qskSendEventTo( this, QEvent::ContentsRectChange );
        Q_EMIT marginsChanged( m );
    }
}

void QskControl::resetMargins()
{
    if ( resetPaddingHint( QskControl::Background ) )
    {
        qskSendEventTo( this, QEvent::ContentsRectChange );
        Q_EMIT marginsChanged( margins() );
    }
}

QMarginsF QskControl::margins() const
{
    return paddingHint( QskControl::Background );
}

QRectF QskControl::contentsRect() const
{
    return qskValidOrEmptyInnerRect( rect(), margins() );
}

QRectF QskControl::subControlRect( QskAspect::Subcontrol subControl ) const
{
    return subControlRect( contentsRect(), subControl );
}

QRectF QskControl::subControlRect(
    const QSizeF& size, QskAspect::Subcontrol subControl ) const
{
    QRectF rect( 0.0, 0.0, size.width(), size.height() );
    rect = qskValidOrEmptyInnerRect( rect, margins() );

    return subControlRect( rect, subControl );
}

QRectF QskControl::subControlContentsRect( QskAspect::Subcontrol subControl ) const
{
    return subControlContentsRect( contentsRect(), subControl );
}

QRectF QskControl::subControlContentsRect(
    const QSizeF& size, QskAspect::Subcontrol subControl ) const
{
    QRectF rect( 0.0, 0.0, size.width(), size.height() );
    rect = qskValidOrEmptyInnerRect( rect, margins() );

    return subControlContentsRect( rect, subControl );
}

QLocale QskControl::locale() const
{
    return d_func()->locale;
}

void QskControl::setLocale( const QLocale& locale )
{
    Q_D( QskControl );

    d->explicitLocale = true;

    if ( d->locale != locale )
    {
        extern void qskInheritLocale( QObject*, const QLocale& );

        d->locale = locale;
        qskSendEventTo( this, QEvent::LocaleChange );

        qskInheritLocale( this, locale );
    }
}

void QskControl::resetLocale()
{
    Q_D( QskControl );

    if ( d->explicitLocale )
    {
        d->explicitLocale = false;
        QskControlPrivate::resolveLocale( this );
    }
}

void QskControl::setSection( QskAspect::Section section )
{
    if ( section > QskAspect::LastSection )
    {
        qWarning() << "Trying to set an invalid section, ignored";
        return;
    }

    Q_D( QskControl );

    d->explicitSection = true;

    if ( d->section != section )
    {
        extern void qskInheritSection( QskControl*, const QskAspect::Section );

        d->section = section;

        update();
        resetImplicitSize();

        qskInheritSection( this, section );
    }
}

void QskControl::resetSection()
{
    Q_D( QskControl );

    if ( d->explicitSection )
    {
        d->explicitSection = false;
        QskControlPrivate::resolveSection( this );
    }
}

QskAspect::Section QskControl::section() const
{
    return static_cast< QskAspect::Section >( d_func()->section );
}

void QskControl::initSizePolicy(
    QskSizePolicy::Policy horizontalPolicy,
    QskSizePolicy::Policy verticalPolicy )
{
    Q_D( QskControl );

    /*
       In constructors of derived classes you don't need
       to propagate changes by layoutConstraintChanged.
       Sometimes it is even worse as the parent might not be
       even prepared to handle the LayouRequest event.
     */

    d->sizePolicy.setHorizontalPolicy( horizontalPolicy );
    d->sizePolicy.setVerticalPolicy( verticalPolicy );

    if ( horizontalPolicy == QskSizePolicy::Constrained
        && verticalPolicy == QskSizePolicy::Constrained )
    {
        qWarning( "QskControl::initSizePolicy: conflicting constraints" );
    }
}

void QskControl::setSizePolicy( QskSizePolicy policy )
{
    Q_D( QskControl );

    if ( policy != d->sizePolicy )
    {
        d->sizePolicy = policy;
        d->layoutConstraintChanged();

        if ( policy.policy( Qt::Horizontal ) == QskSizePolicy::Constrained
            && policy.policy( Qt::Vertical ) == QskSizePolicy::Constrained )
        {
            qWarning( "QskControl::setSizePolicy: conflicting constraints" );
        }
    }
}

void QskControl::setSizePolicy(
    QskSizePolicy::Policy horizontalPolicy,
    QskSizePolicy::Policy verticalPolicy )
{
    setSizePolicy( QskSizePolicy( horizontalPolicy, verticalPolicy ) );
}

void QskControl::setSizePolicy(
    Qt::Orientation orientation, QskSizePolicy::Policy policy )
{
    Q_D( QskControl );

    if ( d->sizePolicy.policy( orientation ) != policy )
    {
        d->sizePolicy.setPolicy( orientation, policy );
        d->layoutConstraintChanged();
    }
}

QskSizePolicy QskControl::sizePolicy() const
{
    return d_func()->sizePolicy;
}

QskSizePolicy::Policy QskControl::sizePolicy( Qt::Orientation orientation ) const
{
    return d_func()->sizePolicy.policy( orientation );
}

/*
    Layout attributes belong more to the layout code, than
    being parameters of the control. So storing them here is kind of a
    design flaw ( similar to QWidget/QSizePolicy ).
    But this way we don't need to add the attributes to all type of
    layout engines + we can make use of them when doing layouts
    manually ( f.e autoLayoutChildren ).
 */
void QskControl::setLayoutAlignmentHint( Qt::Alignment alignment )
{
    Q_D( QskControl );

    if ( layoutAlignmentHint() != alignment )
    {
        d->layoutAlignmentHint = alignment;
        d->layoutConstraintChanged();
    }
}

Qt::Alignment QskControl::layoutAlignmentHint() const
{
    return static_cast< Qt::Alignment >( d_func()->layoutAlignmentHint );
}

void QskControl::setPlacementPolicy(
    Qsk::Visibilities visibilities, QskPlacementPolicy::Policy policy )
{
    auto placementPolicy = this->placementPolicy();
    placementPolicy.setPolicy( visibilities, policy );

    setPlacementPolicy( placementPolicy );
}

void QskControl::setPlacementPolicy(
    QskPlacementPolicy::Policy visiblePolicy, QskPlacementPolicy::Policy hiddenPolicy )
{
    setPlacementPolicy( QskPlacementPolicy( visiblePolicy, hiddenPolicy ) );
}

void QskControl::setPlacementPolicy( QskPlacementPolicy policy )
{
    Q_D( QskControl );

    if ( policy != placementPolicy() )
    {
        d->setPlacementPolicy( true, policy.visiblePolicy() );
        d->setPlacementPolicy( false, policy.hiddenPolicy() );

        d->layoutConstraintChanged();
    }
}

void QskControl::resetPlacementPolicy()
{
    setPlacementPolicy( QskPlacementPolicy() );
}

QskPlacementPolicy QskControl::placementPolicy() const
{
    Q_D( const QskControl );

    return QskPlacementPolicy(
        d->placementPolicy( true ), d->placementPolicy( false ) );
}

QskPlacementPolicy::Policy QskControl::placementPolicy( Qsk::Visibility visiblity ) const
{
    return d_func()->placementPolicy( visiblity == Qsk::Visible );
}

QskPlacementPolicy::Policy QskControl::effectivePlacementPolicy() const
{
    return d_func()->placementPolicy( isVisibleToParent() );
}

bool QskControl::isVisibleToLayout() const
{
    return qskIsVisibleToLayout( this );
}

void QskControl::setPreferredSize( const QSizeF& size )
{
    setExplicitSizeHint( Qt::PreferredSize, size );
}

void QskControl::setPreferredSize( qreal width, qreal height )
{
    setExplicitSizeHint( Qt::PreferredSize, QSizeF( width, height ) );
}

void QskControl::setPreferredWidth( qreal width )
{
    const qreal height = explicitSizeHint( Qt::PreferredSize ).height();
    setExplicitSizeHint( Qt::PreferredSize, QSizeF( width, height ) );
}

void QskControl::setPreferredHeight( qreal height )
{
    const qreal width = explicitSizeHint( Qt::PreferredSize ).width();
    setExplicitSizeHint( Qt::PreferredSize, QSizeF( width, height ) );
}

void QskControl::setMinimumSize( const QSizeF& size )
{
    setExplicitSizeHint( Qt::MinimumSize, size );
}

void QskControl::setMinimumSize( qreal width, qreal height )
{
    setExplicitSizeHint( Qt::MinimumSize, QSizeF( width, height ) );
}

void QskControl::setMinimumWidth( qreal width )
{
    const qreal height = explicitSizeHint( Qt::MinimumSize ).height();
    setExplicitSizeHint( Qt::MinimumSize, QSizeF( width, height ) );
}

void QskControl::setMinimumHeight( qreal height )
{
    const qreal width = explicitSizeHint( Qt::MinimumSize ).width();
    setExplicitSizeHint( Qt::MinimumSize, QSizeF( width, height ) );
}

void QskControl::setMaximumSize( const QSizeF& size )
{
    setExplicitSizeHint( Qt::MaximumSize, size );
}

void QskControl::setMaximumSize( qreal width, qreal height )
{
    setExplicitSizeHint( Qt::MaximumSize, QSizeF( width, height ) );
}

void QskControl::setMaximumWidth( qreal width )
{
    const qreal height = explicitSizeHint( Qt::MaximumSize ).height();
    setExplicitSizeHint( Qt::MaximumSize, QSizeF( width, height ) );
}

void QskControl::setMaximumHeight( qreal height )
{
    const qreal width = explicitSizeHint( Qt::MaximumSize ).width();
    setExplicitSizeHint( Qt::MaximumSize, QSizeF( width, height ) );
}

void QskControl::setFixedSize( const QSizeF& size )
{
    const QSizeF newSize = size.expandedTo( QSizeF( 0, 0 ) );

    const QskSizePolicy policy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    Q_D( QskControl );

    if ( policy != d->sizePolicy ||
        d->explicitSizeHint( Qt::PreferredSize ) != newSize )
    {
        d->sizePolicy = policy;
        d->setExplicitSizeHint( Qt::PreferredSize, newSize );

        d->layoutConstraintChanged();
    }
}

void QskControl::setFixedSize( qreal width, qreal height )
{
    setFixedSize( QSizeF( width, height ) );
}

void QskControl::setFixedWidth( qreal width )
{
    if ( width < 0 )
        width = 0;

    Q_D( QskControl );

    auto size = d->explicitSizeHint( Qt::PreferredSize );

    if ( ( d->sizePolicy.horizontalPolicy() != QskSizePolicy::Fixed ) ||
        ( size.width() != width ) )
    {
        size.setWidth( width );

        d->sizePolicy.setHorizontalPolicy( QskSizePolicy::Fixed );
        d->setExplicitSizeHint( Qt::PreferredSize, size );

        d->layoutConstraintChanged();
    }
}

void QskControl::setFixedHeight( qreal height )
{
    if ( height < 0 )
        height = 0;

    Q_D( QskControl );

    auto size = d->explicitSizeHint( Qt::PreferredSize );

    if ( ( d->sizePolicy.verticalPolicy() != QskSizePolicy::Fixed ) ||
        ( size.height() != height ) )
    {
        size.setHeight( height );

        d->sizePolicy.setVerticalPolicy( QskSizePolicy::Fixed );
        d->setExplicitSizeHint( Qt::PreferredSize, size );

        d->layoutConstraintChanged();
    }
}

void QskControl::resetExplicitSizeHint( Qt::SizeHint whichHint )
{
    if ( whichHint >= Qt::MinimumSize && whichHint <= Qt::MaximumSize )
    {
        Q_D( QskControl );

        const auto oldHint = d->explicitSizeHint( whichHint );
        d->resetExplicitSizeHint( whichHint );

        if ( oldHint != d->explicitSizeHint( whichHint ) )
            d->layoutConstraintChanged();
    }
}

void QskControl::setExplicitSizeHint( Qt::SizeHint whichHint, const QSizeF& size )
{
    if ( whichHint >= Qt::MinimumSize && whichHint <= Qt::MaximumSize )
    {
        const QSizeF newSize( ( size.width() < 0 ) ? -1.0 : size.width(),
            ( size.height() < 0 ) ? -1.0 : size.height() );

        Q_D( QskControl );

        if ( newSize != d->explicitSizeHint( whichHint ) )
        {
            d->setExplicitSizeHint( whichHint, newSize );
            d->layoutConstraintChanged();
        }
    }
}

void QskControl::setExplicitSizeHint(
    Qt::SizeHint whichHint, qreal width, qreal height )
{
    setExplicitSizeHint( whichHint, QSizeF( width, height ) );
}

QSizeF QskControl::explicitSizeHint( Qt::SizeHint whichHint ) const
{
    if ( whichHint < Qt::MinimumSize || whichHint > Qt::MaximumSize )
        return QSizeF();

    return d_func()->explicitSizeHint( whichHint );
}

QSizeF QskControl::implicitSizeHint(
    Qt::SizeHint whichHint, const QSizeF& constraint ) const
{
    if ( whichHint < Qt::MinimumSize || whichHint > Qt::MaximumSize )
        return QSizeF();

    if ( constraint.isValid() )
    {
        // having constraints in both directions does not make sense
        return constraint;
    }

    QSizeF hint;

    if ( whichHint == Qt::PreferredSize
        && constraint.width() < 0.0 && constraint.height() < 0.0 )
    {
        // this one might be cached
        hint = implicitSize();
    }
    else
    {
        hint = d_func()->implicitSizeHint( whichHint, constraint );
    }

    return hint;
}

QSizeF QskControl::effectiveSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which < Qt::MinimumSize || which > Qt::MaximumSize )
        return QSizeF( 0, 0 );

    if ( constraint.isValid() )
        return constraint;

    const bool isConstrained =
        constraint.width() >= 0 || constraint.height() >= 0;

    Q_D( const QskControl );

    d->blockLayoutRequestEvents = false;

    QSizeF hint;

    /*
        The explicit size has always precedence over the implicit size,
        and will kill the effect of the constraint
     */

    hint = d->explicitSizeHint( which );

    if ( !hint.isValid() )
    {
        const auto implicitHint = implicitSizeHint( which, constraint );

        if ( hint.width() < 0 )
            hint.setWidth( implicitHint.width() );

        if ( hint.height() < 0 )
            hint.setHeight( implicitHint.height() );
    }

    if ( !isConstrained && ( hint.width() >= 0 || hint.height() >= 0 ) )
    {
        /*
            We normalize the unconstrained hints by the explicit hints, so that
            we always have: minimum <= preferred <= maximum.
         */

        if ( which == Qt::MaximumSize )
        {
            const auto minimumHint = d->explicitSizeHint( Qt::MinimumSize );

            if ( hint.width() >= 0 )
                hint.rwidth() = qMax( hint.width(), minimumHint.width() );

            if ( hint.height() >= 0 )
                hint.rheight() = qMax( hint.height(), minimumHint.height() );
        }
        else if ( which == Qt::PreferredSize )
        {
            const auto minimumHint = d->explicitSizeHint( Qt::MinimumSize );
            const auto maximumHint = d->explicitSizeHint( Qt::MaximumSize );

            if ( hint.width() >= 0 )
            {
                if ( maximumHint.width() >= 0 )
                    hint.rwidth() = qMin( hint.width(), maximumHint.width() );

                hint.rwidth() = qMax( hint.width(), minimumHint.width() );
            }

            if ( hint.height() >= 0 )
            {
                if ( maximumHint.height() >= 0 )
                    hint.rheight() = qMin( hint.height(), maximumHint.height() );

                hint.rheight() = qMax( hint.height(), minimumHint.height() );
            }
        }
    }

    return hint;
}

qreal QskControl::heightForWidth( qreal width ) const
{
    const auto hint = effectiveSizeHint(
        Qt::PreferredSize, QSizeF( width, -1.0 ) );

    return hint.height();
}

qreal QskControl::widthForHeight( qreal height ) const
{
    const auto hint = effectiveSizeHint(
        Qt::PreferredSize, QSizeF( -1.0, height ) );

    return hint.width();
}

QSizeF QskControl::sizeConstraint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    return qskSizeConstraint( this, which, constraint );
}

bool QskControl::event( QEvent* event )
{
    switch ( static_cast< int >( event->type() ) )
    {
        case QEvent::EnabledChange:
        {
            setSkinStateFlag( Disabled, !isEnabled() );
            break;
        }
        case QEvent::LocaleChange:
        {
            Q_EMIT localeChanged( locale() );
            break;
        }
        case QEvent::ContentsRectChange:
        {
            resetImplicitSize();
            if ( d_func()->autoLayoutChildren )
                polish();

            break;
        }
        case QEvent::LayoutRequest:
        {
            if ( d_func()->autoLayoutChildren )
            {
                resetImplicitSize();
                polish();
            }

            break;
        }
        case QEvent::StyleChange:
        {
            // The skin has changed

            if ( skinlet() == nullptr )
            {
                /*
                    When we don't have a local skinlet, the skinlet
                    from the previous skin might be cached.
                 */

                setSkinlet( nullptr );
            }

            break;
        }
        case QskEvent::Gesture:
        {
            gestureEvent( static_cast< QskGestureEvent* >( event ) );
            return true;
        }
        case QEvent::MouseButtonPress:
        {
            /*
                We need to do a gesture detection with abort criterions
                first. When it fails the input events will be processed
                in ascending order along the item tree until an item accepts
                the event.

                This detection can be done in childMouseEventFilter() for all
                children - but not for the filtering control ( = this ) itself.
                So we need to do this here.
             */
            if ( qskMaybeGesture( this, this, event ) )
                return true;

            const bool ok = Inherited::event( event );

            if ( !event->isAccepted() )
            {
                /*
                    When the initial gesture detection failed and the event
                    has not been handled here we do a second gesture detection
                    without passing a child. It can be used for detections
                    without abort criterions.

                    An example is the pan gesture detection, that can be started without
                    any timeouts now.
                 */
                if ( qskMaybeGesture( this, nullptr, event ) )
                    return true;
            }

            return ok;
        }
    }

    if ( qskMaybeGesture( this, this, event ) )
        return true;

    return Inherited::event( event );
}

bool QskControl::childMouseEventFilter( QQuickItem* child, QEvent* event )
{
    /*
        The strategy implemented in many classes of the Qt development is
        to analyze the events without blocking the handling of the child. 
        Once a gesture is detected the gesture handling trys to steal the
        mouse grab hoping for the child to abort its operation.

        This approach has obvious problems:

        - operations already started on press can't be aborted anymore
        - the child needs to agree on losing the grab ( setKeepMouseGrab( false ) )
        - ...

        We implement a different strategy: processing of the events
        by the children is blocked until the gesture detection has accepted
        or rejected. In case of a rejection the events will be replayed.
        ( see QskGestureRecognizer )
     */

    return qskMaybeGesture( this, child, event );
}

void QskControl::gestureEvent( QskGestureEvent* )
{
}

void QskControl::hoverEnterEvent( QHoverEvent* event )
{
    Inherited::hoverEnterEvent( event );
    setSkinStateFlag( Hovered );
}

void QskControl::hoverLeaveEvent( QHoverEvent* event )
{
    Inherited::hoverLeaveEvent( event );
    setSkinStateFlag( Hovered, false );
}

void QskControl::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    switch ( static_cast< int >( change ) )
    {
        case QQuickItem::ItemParentHasChanged:
        {
            if ( value.item )
            {
                if ( !d_func()->explicitLocale )
                    QskControlPrivate::resolveLocale( this );

                if ( !d_func()->explicitSection )
                    QskControlPrivate::resolveSection( this );
            }

#if 1
            // not necessarily correct, when parent != parentItem ???
            qskSendEventTo( this, QEvent::ParentChange );
#endif

            break;
        }
        case QQuickItem::ItemChildAddedChange:
        case QQuickItem::ItemChildRemovedChange:
        {
            if ( autoLayoutChildren() )
            {
                if ( qskIsVisibleToLayout( value.item ) )
                    resetImplicitSize();

                if ( qskIsAdjustableByLayout( value.item ) )
                    polish();
            }
            break;
        }
        case QQuickItem::ItemActiveFocusHasChanged:
        {
            setSkinStateFlag( Focused, hasActiveFocus() );
            break;
        }
    }

    Inherited::itemChange( change, value );
}

void QskControl::geometryChange(
    const QRectF& newGeometry, const QRectF& oldGeometry )
{
    if ( d_func()->autoLayoutChildren )
    {
        if ( newGeometry.size() != oldGeometry.size() )
            polish();
    }

    Inherited::geometryChange( newGeometry, oldGeometry );
}

void QskControl::updateItemPolish()
{
    updateResources(); // an extra dirty bit for this ???

    if ( width() >= 0.0 || height() >= 0.0 )
    {
        if ( d_func()->autoLayoutChildren && !maybeUnresized() )
        {
            const auto rect = layoutRect();

            const auto children = childItems();
            for ( auto child : children )
            {
                if ( qskIsAdjustableByLayout( child ) )
                {
                    const auto r = qskConstrainedItemRect( child, rect );
                    qskSetItemGeometry( child, r );
                }
            }
        }

        updateLayout();
    }
}

QSGNode* QskControl::updateItemPaintNode( QSGNode* node )
{
    if ( node == nullptr )
        node = new QSGNode;

    updateNode( node );
    return node;
}

QskControl* QskControl::owningItem() const
{
    return const_cast< QskControl* >( this );
}

QRectF QskControl::layoutRect() const
{
    Q_D( const QskControl );

    if ( d->width <= 0.0 && d->height <= 0.0 )
        return QRectF();

    return layoutRectForSize( size() );
}

QRectF QskControl::layoutRectForSize( const QSizeF& size ) const
{
    const QRectF r( 0.0, 0.0, size.width(), size.height() );
    return qskValidOrEmptyInnerRect( r, margins() );
}

QRectF QskControl::focusIndicatorRect() const
{
    return contentsRect();
}

QRectF QskControl::focusIndicatorClipRect() const
{
    return clipRect();
}

void QskControl::updateLayout()
{
}

void QskControl::updateResources()
{
}

QSizeF QskControl::contentsSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    return effectiveSkinlet()->sizeHint( this, which, constraint );
}

QSizeF QskControl::layoutSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which == Qt::MaximumSize || !d_func()->autoLayoutChildren )
        return QSizeF();

    qreal w = -1.0;
    qreal h = -1.0;

    const auto children = childItems();

    for ( const auto child : children )
    {
        if ( !qskIsVisibleToLayout( child ) )
            continue;

        const auto policy = qskSizePolicy( child );

        if ( constraint.width() >= 0.0 && policy.isConstrained( Qt::Vertical ) )
        {
            const auto hint = qskSizeConstraint( child, which, constraint );
            h = qMax( h, hint.height() );
        }
        else if ( constraint.height() >= 0.0 && policy.isConstrained( Qt::Horizontal ) )
        {
            const auto hint = qskSizeConstraint( child, which, constraint );
            w = qMax( w, hint.width() );
        }
        else
        {
            const auto hint = qskSizeConstraint( child, which, QSizeF() );

            w = qMax( w, hint.width() );
            h = qMax( h, hint.height() );
        }
    }

    return QSizeF( w, h );
}

QVector< QskAspect::Subcontrol > QskControl::subControls() const
{
    QVector< QskAspect::Subcontrol > subControls;

    for ( auto mo = metaObject(); mo != nullptr; mo = mo->superClass() )
    {
        const auto moSubControls = QskAspect::subControls( mo );

        for ( auto subControl : moSubControls )
        {
            const auto subControlEffective = effectiveSubcontrol( subControl );

            if ( subControlEffective == subControl )
            {
                subControls += subControlEffective;
            }
            else
            {
                // when subControlEffective differs it usually has
                // been mapped to a subcontrol of an inherited class
                // that is already in the list.

                if ( !subControls.contains( subControlEffective ) )
                {
                    subControls += subControlEffective;
                }
            }
        }
    }

    return subControls;
}

#include "moc_QskControl.cpp"
