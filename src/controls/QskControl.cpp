/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskControl.h"
#include "QskAspect.h"
#include "QskSetup.h"
#include "QskSkinlet.h"
#include "QskEvent.h"
#include "QskDirtyItemFilter.h"
#include "QskSkinHintTable.h"

#include <QLocale>
#include <QVector>

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
#if defined( QT_DEBUG )
#include <private/qquickpositioners_p.h>
#endif
QSK_QT_PRIVATE_END

#include <limits>
#include <unordered_map>

QSK_STATE( QskControl, Disabled, QskAspect::FirstSystemState )
QSK_STATE( QskControl, Hovered, QskAspect::LastSystemState >> 1 )
QSK_STATE( QskControl, Focused, QskAspect::LastSystemState )

typedef quint16 controlFlags_t;

void qskResolveLocale( QskControl* );

static void qskSendEventTo( QObject* object, QEvent::Type type )
{
    QEvent event( type );
    QCoreApplication::sendEvent( object, &event );
}

static controlFlags_t qskControlFlags()
{
    // we are only interested in the first 8 bits
    return static_cast< controlFlags_t >( qskSetup->controlFlags() );
}

static void qskFilterWindow( QQuickWindow* window )
{
    if ( window == nullptr )
        return;

    static QskDirtyItemFilter itemFilter;
    itemFilter.addWindow( window );
}

namespace
{
    /*
        A helper class to store the released window to be able to
        put it later into the WindowChange event.
     */
    class QskWindowStore
    {
    public:
        QskWindowStore():
            m_refCount( 0 ),
            m_window( nullptr )
        {
        }

        void setWindow( QQuickWindow* window )
        {
            if ( m_window != window )
            {
                m_window = window;
                m_refCount = 0;
            }

            if ( m_window )
                m_refCount++;
        }

        QQuickWindow* window()
        {
            QQuickWindow* w = m_window;

            if ( m_window )
            {
                if ( --m_refCount == 0 )
                    m_window = nullptr;
            }

            return w;
        }

    private:
        int m_refCount;
        QQuickWindow* m_window;
    };
}

static QskWindowStore qskReleasedWindowCounter;

class QskControlPrivate : public QQuickItemPrivate
{
    Q_DECLARE_PUBLIC(QskControl)

public:
    QskControlPrivate():
        controlFlags( qskControlFlags() ),
        controlFlagsMask( 0 ),
        explicitLocale( false ),
        autoFillBackground( false ),
        autoLayoutChildren( false ),
        polishOnResize( false ),
        blockedPolish( false ),
        blockedImplicitSize( true ),
        clearPreviousNodes( false ),
        isInitiallyPainted( false ),
        focusPolicy( Qt::NoFocus ),
        isWheelEnabled( false )
    {
    }

    void mirrorChange() override
    {
        Q_Q( QskControl );
        qskSendEventTo( q, QEvent::LayoutDirectionChange );
    }

#if 0
    // can we do something useful with overloading those ???

    virtual qreal getImplicitWidth() const override final;
    virtual qreal getImplicitHeight() const override final;
    virtual void implicitWidthChanged();
    virtual void implicitHeightChanged();

    virtual QSGTransformNode *createTransformNode();
#endif

    bool maybeGesture( QQuickItem* child, QEvent* event )
    {
        Q_Q( QskControl );

        switch( event->type() )
        {
            case QEvent::MouseButtonPress:
            {
                const auto mouseEvent = static_cast< const QMouseEvent* >( event );
                const QPointF pos = q->mapFromScene( mouseEvent->windowPos() );

                if ( !q->gestureRect().contains( pos ) )
                    return false;

                break;
            }

            case QEvent::MouseMove:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseButtonDblClick:
            case QEvent::UngrabMouse:
            case QEvent::TouchBegin:
            case QEvent::TouchCancel:
            case QEvent::TouchUpdate:
                break;

            default:
                return false;
        }

        return q->gestureFilter( child, event );
    }

    QLocale locale;
        
    quint16 controlFlags;
    quint16 controlFlagsMask;

    bool explicitLocale : 1;
        
    bool autoFillBackground : 1;
    bool autoLayoutChildren : 1;
    bool polishOnResize : 1;
        
    bool blockedPolish : 1;
    bool blockedImplicitSize : 1;
    bool clearPreviousNodes : 1;
        
    bool isInitiallyPainted : 1;

    uint focusPolicy : 4;
    bool isWheelEnabled;
};

QskControl::QskControl( QQuickItem* parent ):
    QskControl( *( new QskControlPrivate() ), parent )
{
}

QskControl::QskControl( QQuickItemPrivate& dd, QQuickItem* parent ):
    Inherited( dd, parent )
{
    Q_D( QskControl );

    if ( d->controlFlags & QskControl::DeferredLayout )
    {
        // In general the geometry of an item should be the job of
        // the parent - unfortunatly not done by Qt Quick
        // probably in the spirit of "making things easier".

        // To avoid potentially expensive calculations happening
        // too often and early QskControl blocks updates of
        // the implicitSize and any auto resizing of the control
        // according to it.

        // There should be no strong reason for using concepts
        // like Positioners, that rely on implicit resizing,
        // but to make it working: the DeferredLayout flag needs to be disabled.

        d->widthValid = d->heightValid = true;
    }

    setFlag( QQuickItem::ItemHasContents, true );
    QQuickItem::setActiveFocusOnTab( false );

    if ( parent )
    {
        // inheriting attributes from parent
        qskResolveLocale( this );
    }

    setupImplicitSizeConnections( true );

    connect( this, &QQuickItem::enabledChanged,
        [this] { setSkinStateFlag( Disabled, !isEnabled() ); } );

    // using a conventional connection as being automatically
    // removed by the destructor

    connect( qskSetup, SIGNAL( controlFlagsChanged() ),
        this, SLOT( updateControlFlags() ) );

    if ( d->controlFlags & QskControl::DeferredUpdate )
        qskFilterWindow( window() );
}

QskControl::~QskControl()
{
}

const char* QskControl::className() const
{
    return metaObject()->className();
}

void QskControl::setVisible( bool on )
{
    // QQuickItem::setVisible is no slot
    Inherited::setVisible( on );
}

void QskControl::show()
{
    Inherited::setVisible( true );
}

void QskControl::hide()
{
    Inherited::setVisible( false );
}

void QskControl::setGeometry( qreal x, qreal y, qreal width, qreal height )
{
    // QQuickItem does not even offer changing the geometry
    // in one call - what leads to 2 calls of the updateGeometry
    // hook. Grmpf ...

    Q_D( QQuickItem );

    d->heightValid = true;
    d->widthValid = true;

    const QRectF oldRect( d->x, d->y, d->width, d->height );

    int dirtyType = 0;

    if ( d->x != x || d->y != y )
    {
        d->x = x;
        d->y = y;

        dirtyType |= QQuickItemPrivate::Position;
    }

    if ( d->width != width || d->height != height )
    {
        d->height = height;
        d->width = width;

        dirtyType |= QQuickItemPrivate::Size;
    }

    if ( dirtyType )
    {
        if ( dirtyType & QQuickItemPrivate::Position )
            d->dirty( QQuickItemPrivate::Position );

        if ( dirtyType & QQuickItemPrivate::Size )
            d->dirty( QQuickItemPrivate::Size );

        geometryChanged( QRectF( d->x, d->y, d->width, d->height ), oldRect );
    }
}

QRectF QskControl::geometry() const
{
    Q_D( const QskControl );
    return QRectF( d->x, d->y, d->width, d->height );
}

void QskControl::setAutoFillBackground( bool on )
{
    Q_D( QskControl );
    if ( on != d->autoFillBackground )
    {
        d->autoFillBackground = on;

        update();
        Q_EMIT controlFlagsChanged();
    }
}

bool QskControl::autoFillBackground() const
{
    return d_func()->autoFillBackground;
}

void QskControl::setAutoLayoutChildren( bool on )
{
    Q_D( QskControl );
    if ( on != d->autoLayoutChildren )
    {
        d->autoLayoutChildren = on;
        if ( on )
            polish();

        Q_EMIT controlFlagsChanged();
    }
}

bool QskControl::autoLayoutChildren() const
{
    return d_func()->autoLayoutChildren;
}

void QskControl::setTransparentForPositioner( bool on )
{
    Q_D( QskControl );
    if ( on != d->isTransparentForPositioner() )
    {
        d->setTransparentForPositioner( on );
        Q_EMIT controlFlagsChanged();
    }
}

bool QskControl::isTransparentForPositioner() const
{
    return d_func()->isTransparentForPositioner();
}

bool QskControl::isTransparentForPositioner( const QQuickItem* item )
{
    if ( item == nullptr )
        return true;

    return QQuickItemPrivate::get( item )->isTransparentForPositioner();
}

void QskControl::setPolishOnResize( bool on )
{
    Q_D( QskControl );
    if ( on != d->polishOnResize )
    {
        d->polishOnResize = on;
        polish();

        Q_EMIT controlFlagsChanged();
    }
}

bool QskControl::polishOnResize() const
{
    return d_func()->polishOnResize;
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
        d->focusPolicy = policy & ~Qt::TabFocus;
        QQuickItem::setActiveFocusOnTab( policy & Qt::TabFocus );

        Q_EMIT ( focusPolicyChanged() );
    }
}

Qt::FocusPolicy QskControl::focusPolicy() const
{
    uint policy = d_func()->focusPolicy;
    if ( activeFocusOnTab() )
        policy |= Qt::TabFocus;

    return static_cast< Qt::FocusPolicy >( policy );
}

void QskControl::setTabFence( bool on )
{
    Q_D( QskControl );
    if ( on != d->isTabFence )
    {
        d->isTabFence = on;
        Q_EMIT controlFlagsChanged();
    }
}

bool QskControl::isTabFence() const
{
    return d_func()->isTabFence;
}

QskControl::Flags QskControl::controlFlags() const
{
    return QskControl::Flags( d_func()->controlFlags );
}

void QskControl::setControlFlags( Flags flags )
{
    // set all bits in the mask
    d_func()->controlFlagsMask = std::numeric_limits< controlFlags_t >::max();
    updateControlFlags( flags );
}

void QskControl::resetControlFlags()
{
    // clear all bits in the mask
    d_func()->controlFlagsMask = 0;
    updateControlFlags( static_cast< Flags >( qskControlFlags() ) );
}

void QskControl::setControlFlag( Flag flag, bool on )
{
    Q_D( QskControl );

    d->controlFlagsMask |= flag;

    if ( ( d->controlFlags & flag ) != on )
    {
        updateControlFlag( flag, on );
        Q_EMIT controlFlagsChanged();
    }
}

void QskControl::resetControlFlag( Flag flag )
{
    Q_D( QskControl );

    d->controlFlagsMask &= ~flag;

    const bool on = qskSetup->testControlFlag( static_cast< QskSetup::Flag >( flag ) );

    if ( ( d->controlFlags & flag ) != on )
    {
        updateControlFlag( flag, on );
        Q_EMIT controlFlagsChanged();
    }
}

bool QskControl::testControlFlag( Flag flag ) const
{
    return d_func()->controlFlags & flag;
}

void QskControl::updateControlFlags()
{
    updateControlFlags( static_cast< Flags >( qskControlFlags() ) );
}

void QskControl::updateControlFlags( Flags flags )
{
    const auto oldFlags = d_func()->controlFlags;
    const auto newFlags = static_cast< controlFlags_t >( flags );

    if ( oldFlags != newFlags )
    {
        for ( uint i = 0; i <= LastFlag; ++i )
        {
            const uint flag = ( 1 << i );
            updateControlFlag( flag, flags & flag );
        }

        Q_EMIT controlFlagsChanged();
    }
}

void QskControl::updateControlFlag( uint flag, bool on )
{
    Q_D( QskControl );

    if ( ( flag > std::numeric_limits< controlFlags_t >::max() )
        || ( bool( d->controlFlags & flag ) == on ) )
    {
        return;
    }

    if ( on )
        d->controlFlags |= flag;
    else
        d->controlFlags &= ~flag;

    switch( flag )
    {
        case QskControl::DeferredUpdate:
        {
            if ( on )
            {
                qskFilterWindow( window() );
            }
            else
            {
                if ( !isVisible() )
                    update();
            }

            break;
        }
        case QskControl::DeferredPolish:
        {
            if ( !on && d->blockedPolish )
                polish();

            break;
        }
        case QskControl::DeferredLayout:
        {
            if ( !on )
            {
                // Update the implicitSize and rebind the size to it.
                // Having set the size explicitly gets lost.

                d->widthValid = d->heightValid = false;
                updateImplicitSize();
            }

            break;
        }
        case QskControl::CleanupOnVisibility:
        {
            if ( on && !isVisible() )
                cleanupNodes();

            break;
        }
        case QskControl::DebugForceBackground:
        {
            // no need to mark it dirty
            update();
            break;
        }
        default:
            break;
    }
}

void QskControl::setupImplicitSizeConnections( bool on )
{
    // in case of someone manipulating the implicit size from
    // outside, we might need to adjust some layouts

    if ( on )
    {
        QObject::connect( this, &QskControl::implicitWidthChanged,
            this, &QskControl::onImplicitSizeChanged );

        QObject::connect( this, &QskControl::implicitHeightChanged,
            this, &QskControl::onImplicitSizeChanged );
    }
    else
    {
        QObject::disconnect( this, &QskControl::implicitWidthChanged,
            this, &QskControl::onImplicitSizeChanged );

        QObject::disconnect( this, &QskControl::implicitHeightChanged,
            this, &QskControl::onImplicitSizeChanged );
    }
}

void QskControl::setBackgroundColor( const QColor& color )
{
    setAutoFillBackground( true );
    setBackground( QskGradient( color ) );
}

void QskControl::setBackground( const QskGradient& gradient )
{
    using namespace QskAspect;
    const Aspect aspect = Control | Color;

    if ( autoFillBackground() )
    {
        if ( hintTable().gradient( aspect ) != gradient )
        {
            setGradientHint( aspect, gradient );

            // might be wrong, when the effective gradient was from the skin
            update();
        }
    }
    else
    {
        setGradientHint( aspect, gradient );
    }
}

void QskControl::resetBackground()
{
    using namespace QskAspect;
    const Aspect aspect = Control | Color;

    auto& table = hintTable();

    if ( autoFillBackground() )
    {
        const auto oldGradient = gradientHint( aspect );

        if ( table.hint( aspect ).isValid() )
        {
            table.removeHint( aspect );
            if ( gradientHint( aspect ) != oldGradient )
                update();
        }
    }
    else
    {
        table.removeHint( aspect );
    }
}

QskGradient QskControl::background() const
{
    using namespace QskAspect;
    return gradientHint( Control );
}

void QskControl::setMargins( qreal margin )
{
    setMargins( QMarginsF( margin, margin, margin, margin ) );
}

void QskControl::setMargins( const QMarginsF& margins )
{
    using namespace QskAspect;

    const Subcontrol subControl = effectiveSubcontrol( QskAspect::Control );

    const QMarginsF m(
        qMax( qreal( margins.left() ), qreal( 0.0 ) ),
        qMax( qreal( margins.top() ), qreal( 0.0 ) ),
        qMax( qreal( margins.right() ), qreal( 0.0 ) ),
        qMax( qreal( margins.bottom() ), qreal( 0.0 ) ) );

    if ( m != this->margins() )
    {
        setMarginsHint( subControl | Margin, m );
        resetImplicitSize();

        Q_D( const QskControl );
        if ( d->polishOnResize || d->autoLayoutChildren )
            polish();

        qskSendEventTo( this, QEvent::ContentsRectChange );
    }
}

void QskControl::resetMargins()
{
    using namespace QskAspect;
    const Aspect aspect = Control | Metric | Margin;

    const auto oldMargin = marginsHint( aspect );

    auto& table = hintTable();
    if ( table.hint( aspect ).isValid() )
    {
        table.removeHint( aspect );
        if ( marginsHint( aspect ) != oldMargin )
        {
            resetImplicitSize();

            Q_D( const QskControl );
            if ( d->polishOnResize || d->autoLayoutChildren )
                polish();

            qskSendEventTo( this, QEvent::ContentsRectChange );
        }
    }
}

QMarginsF QskControl::margins() const
{
    return marginsHint( QskAspect::Control | QskAspect::Margin );
}

QRectF QskControl::contentsRect() const
{
    const QRectF r = boundingRect();

    const auto m = margins();
    qreal left = r.left() + m.left();
    qreal top = r.top() + m.top();
    qreal right = r.right() - m.right();
    qreal bottom = r.bottom() - m.bottom();

    if ( left > r.right() )
        left = r.right();

    if ( top > r.bottom() )
        top = r.bottom();

    if ( right < left )
        right = left;

    if ( bottom < top )
        bottom = top;

    return QRectF( QPointF( left, top ), QPointF( right, bottom ) );
}

bool QskControl::layoutMirroring() const
{
    return d_func()->effectiveLayoutMirror;
}

void QskControl::setLayoutMirroring( bool on, bool recursive )
{
    // Again we have to deal with an existing API made for QML,
    // that is weired for C++: LayoutMirroring/QQuickLayoutMirroringAttached
    // Internally it is managed by 5(!) different flags - condolences
    // to the poor guy who has been sentenced to maintain this.

    // Anyway, the code below might achieve the desired behavior without
    // breaking the QML path.

    Q_D( QskControl );

    if ( recursive != d->inheritMirrorFromItem)
    {
        d->inheritMirrorFromItem = recursive;
        d->resolveLayoutMirror();
    }

    d->isMirrorImplicit = false;

    if ( on != d->effectiveLayoutMirror )
    {
        d->setLayoutMirror( on );
        if ( recursive )
            d->resolveLayoutMirror();
    }
}

void QskControl::resetLayoutMirroring()
{
    Q_D( QskControl );

    if ( d && !d->isMirrorImplicit)
    {
        d->isMirrorImplicit = true;
        //d->inheritMirrorFromItem = false;
        d->resolveLayoutMirror();
    }
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
        d->locale = locale;
        qskSendEventTo( this, QEvent::LocaleChange );
        qskSetup->inheritLocale( this, locale );
    }
}

void QskControl::resetLocale()
{
    Q_D( QskControl );

    if ( d->explicitLocale )
    {
        d->explicitLocale = false;
        qskResolveLocale( this );
    }
}

// not static as being called from QskSetup.cpp
bool qskInheritLocale( QskControl* control, const QLocale& locale )
{
    auto d = static_cast< QskControlPrivate *>( QQuickItemPrivate::get( control ) );

    if ( d->explicitLocale || d->locale == locale )
        return false;

    d->locale = locale;
    qskSendEventTo( control, QEvent::LocaleChange );

    return true;
}

void qskResolveLocale( QskControl* control )
{
    const QLocale locale = qskSetup->inheritedLocale( control );

    auto d = static_cast< QskControlPrivate *>( QQuickItemPrivate::get( control ) );
    if ( d->locale != locale )
    {
        d->locale = locale;

        qskSendEventTo( control, QEvent::LocaleChange );
        qskSetup->inheritLocale( control, locale );
    }
}

QSizeF QskControl::effectiveConstraint( Qt::SizeHint whichHint ) const
{
    if ( whichHint < Qt::MinimumSize || whichHint > Qt::MaximumSize )
        return QSizeF( 0, 0 );

    QSizeF constraint = QskResizable::sizeHint( whichHint );
    if ( whichHint == Qt::PreferredSize && !constraint.isValid() )
    {
        // in most cases we don't have a preferred width or height
        // and fall back to the implicit size.

        if ( d_func()->blockedImplicitSize )
        {
            QskControl* that = const_cast< QskControl* >( this );
            that->updateImplicitSize();
        }

        if ( constraint.width() < 0 )
            constraint.setWidth( implicitWidth() );

        if ( constraint.height() < 0 )
            constraint.setHeight( implicitHeight() );
    }

    return constraint;
}

void QskControl::resetImplicitSize()
{
    Q_D( QskControl );

    if ( d->controlFlags & QskControl::DeferredLayout )
    {
        d->blockedImplicitSize = true;
        layoutConstraintChanged();
    }
    else
    {
        const QSizeF sz = implicitSize();

        updateImplicitSize();

        if ( sz != implicitSize() )
            onImplicitSizeChanged();
    }
}

bool QskControl::event( QEvent* event )
{
    const int eventType = event->type();

    switch( eventType )
    {
#if 0
        case QEvent::PolishRequest:
        {
            d->polishScheduled = false;
            updatePolish();
            break;
        }
#endif
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
        case QEvent::LayoutRequest:
        {
            if ( d_func()->autoLayoutChildren )
                resetImplicitSize();

            break;
        }
    }

    switch( eventType )
    {
        case QEvent::EnabledChange:
        case QEvent::FontChange:
        case QEvent::PaletteChange:
        case QEvent::LocaleChange:
        case QEvent::ReadOnlyChange:
        case QEvent::ParentChange:
        case QEvent::StyleChange:
        case QEvent::ContentsRectChange:
        {
            changeEvent( event );
            return true;
        }
        case QskEvent::GeometryChange:
        {
            geometryChangeEvent( static_cast< QskGeometryChangeEvent* >( event ) );
            return true;
        }
        case QskEvent::WindowChange:
        {
            windowChangeEvent( static_cast< QskWindowChangeEvent* >( event ) );
            return true;
        }
        case QskEvent::Gesture:
        {
            gestureEvent( static_cast< QskGestureEvent* >( event ) );
            return true;
        }
        default:
        {
            if ( d_func()->maybeGesture( this, event ) )
                return true;
        }
    }

    return Inherited::event( event );
}

bool QskControl::childMouseEventFilter( QQuickItem* item, QEvent* event )
{
    return d_func()->maybeGesture( item, event );
}

bool QskControl::gestureFilter( QQuickItem* item, QEvent* event )
{
    Q_UNUSED( item )
    Q_UNUSED( event )

    return false;
}

void QskControl::windowChangeEvent( QskWindowChangeEvent* )
{
}

void QskControl::geometryChangeEvent( QskGeometryChangeEvent* )
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

void QskControl::changeEvent( QEvent* event )
{
    if ( event->type() == QEvent::StyleChange )
    {
        /*
            the skin has changed and we might have totally
            different skinlets, that can't deal with nodes
            created from other skinlets
         */
        d_func()->clearPreviousNodes = true;

        resetImplicitSize();
        polish();
        update();
    }
}

void QskControl::gestureEvent( QskGestureEvent* )
{
}

void QskControl::classBegin()
{
    Inherited::classBegin();
}

void QskControl::componentComplete()
{
#if defined( QT_DEBUG )
    if ( qobject_cast< const QQuickBasePositioner* >( parent() ) )
    {
        if ( d_func()->controlFlags & QskControl::DeferredLayout )
        {
            qWarning( "QskControl in DeferredLayout mode under control of a positioner" );
        }
    }
#endif
    Inherited::componentComplete();
}

void QskControl::releaseResources()
{
    Inherited::releaseResources();

    // QQuickItem::derefWindow runs over the children between
    // calling releaseResources and itemChange. So we need to have
    // a reference count to know, when we have processed all
    // sequences to be able to provide the correct "oldWindow"
    // in the WindowChange event.

    qskReleasedWindowCounter.setWindow( window() );
}

const QSGNode* QskControl::itemNode( const QQuickItem* item )
{
    if ( item == nullptr )
        return nullptr;

    return QQuickItemPrivate::get( item )->itemNodeInstance;
}

const QSGNode* QskControl::paintNode( const QQuickItem* item )
{
    if ( item == nullptr )
        return nullptr;

    return QQuickItemPrivate::get( item )->paintNode;
}

void QskControl::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    Q_D( QskControl );

    switch( change )
    {
        case QQuickItem::ItemParentHasChanged:
        {
            if ( !d->explicitLocale )
                qskResolveLocale( this );

            // not necessarily correct, when parent != parentItem ???
            qskSendEventTo( this, QEvent::ParentChange );

            break;
        }
        case QQuickItem::ItemVisibleHasChanged:
        {
            if ( value.boolValue )
            {
                if ( d->blockedPolish )
                    polish();

                if ( d->controlFlags & QskControl::DeferredUpdate )
                {
                    if ( d->dirtyAttributes )
                        update();
                }
            }
            else
            {
                if ( d->controlFlags & QskControl::CleanupOnVisibility )
                    cleanupNodes();

                d->isInitiallyPainted = false;
            }

#if 1
            // Layout code might consider the visiblility of the children
            // and therefore needs to be updated. Posting a statement about
            // changed layout constraints has this effect, but is not correct.
            // The right way to go would be to create show/hide events and to
            // handle them, where visibility of the children matters.
            // TODO ...

            layoutConstraintChanged();
#endif
            break;
        }
        case QQuickItem::ItemSceneChange:
        {
            if ( value.window )
            {
                if ( d->controlFlags & QskControl::DeferredUpdate )
                    qskFilterWindow( value.window );
            }

            QskWindowChangeEvent event(
                qskReleasedWindowCounter.window(), value.window );
            QCoreApplication::sendEvent( this, &event );

            break;
        }
        case QQuickItem::ItemActiveFocusHasChanged:
        {
            setSkinStateFlag( Focused, hasActiveFocus() );
            break;
        }
        default:
        {
            break;
        }
    }
}

void QskControl::geometryChanged(
    const QRectF& newGeometry, const QRectF& oldGeometry )
{
    Inherited::geometryChanged( newGeometry, oldGeometry );

    if ( newGeometry.size() != oldGeometry.size() )
    {
        Q_D( const QskControl );
        if ( d->polishOnResize || d->autoLayoutChildren )
            polish();
    }

    QskGeometryChangeEvent event( newGeometry, oldGeometry );
    QCoreApplication::sendEvent( this, &event );
}

void QskControl::layoutConstraintChanged()
{
    QQuickItem* item = parentItem();
    if ( item )
        qskSendEventTo( item, QEvent::LayoutRequest );
}

void QskControl::updatePolish()
{
    Q_D( QskControl );

    if ( d->controlFlags & QskControl::DeferredPolish )
    {
        if ( !isVisible() )
        {
            d->blockedPolish = true;
            return;
        }
    }

    d->blockedPolish = false;

    if ( d->autoLayoutChildren )
    {
        const QRectF rect = layoutRect();

        for ( auto child : childItems() )
        {
            if ( !QQuickItemPrivate::get( child )->isTransparentForPositioner() )
            {
                child->setPosition( rect.topLeft() );
                child->setSize( rect.size() );
            }
        }
    }

    updateLayout();
}

QSGNode* QskControl::updatePaintNode( QSGNode* node, UpdatePaintNodeData* data )
{
    Q_UNUSED( data );

    Q_D( QskControl );

    Q_ASSERT( isVisible() || !( d->controlFlags & QskControl::DeferredUpdate ) );

    if ( !d->isInitiallyPainted )
        d->isInitiallyPainted = true;

    if ( d->clearPreviousNodes )
    {
        delete node;
        node = nullptr;

        d->clearPreviousNodes = false;
    }

    if ( node == nullptr )
        node = new QSGNode;

    updateNode( node );
    return node;
}

void QskControl::cleanupNodes()
{
    Q_D( QskControl );
    if ( d->itemNodeInstance == nullptr )
        return;

    // setting the dirty flags, so that nodes will be recreated
    // the next time we participate in a scene graph update

    if ( !d->itemNodeInstance->matrix().isIdentity() )
        d->dirtyAttributes |= QQuickItemPrivate::Position;

    if ( d->extra.isAllocated() )
    {
        if ( d->extra->clipNode )
            d->dirtyAttributes |= QQuickItemPrivate::Clip;

        if ( d->extra->opacityNode )
            d->dirtyAttributes |= QQuickItemPrivate::OpacityValue;

        if ( d->extra->rootNode )
            d->dirtyAttributes |= QQuickItemPrivate::EffectReference;
    }

    // putting the nodes on the cleanup list of the window to be deleteted
    // in the next cycle of the scene graph

    QQuickWindowPrivate::get( window() )->cleanup( d->itemNodeInstance );

    // now we can forget about the nodes

    d->itemNodeInstance = nullptr;
    d->paintNode = nullptr;

    if ( d->extra.isAllocated() )
    {
        d->extra->opacityNode = nullptr;
        d->extra->clipNode = nullptr;
        d->extra->rootNode = nullptr;
    }
}

void QskControl::onImplicitSizeChanged()
{
    if ( !QskResizable::sizeHint( Qt::PreferredSize ).isValid() )
    {
        // when we have no PreferredSize we fall back
        // the constraint depends on the implicit size

        layoutConstraintChanged();
    }
}

QskControl* QskControl::owningControl() const
{
    return const_cast< QskControl* >( this );
}

QRectF QskControl::layoutRect() const
{
    return contentsRect();
}

QRectF QskControl::gestureRect() const
{
    return boundingRect();
}

void QskControl::updateLayout()
{
}

void QskControl::updateImplicitSize()
{
    d_func()->blockedImplicitSize = false;

    const auto m = margins();
    const auto dw = m.left() + m.right();
    const auto dh = m.top() + m.bottom();

    const QSizeF hint = contentsSizeHint();

    const auto w = ( hint.width() >= 0 ) ? dw + hint.width() : 0.0;
    const auto h = ( hint.height() >= 0 ) ? dh + hint.height() : 0.0;

    setupImplicitSizeConnections( false );
    setImplicitSize( w, h );
    setupImplicitSizeConnections( true );
}

QSizeF QskControl::contentsSizeHint() const
{
    qreal w = -1; // no hint
    qreal h = -1;

    if ( d_func()->autoLayoutChildren )
    {
        for ( const auto child : childItems() )
        {
            if ( auto* control = qobject_cast< const QskControl* >( child ) )
            {
                if ( !QQuickItemPrivate::get( control )->isTransparentForPositioner() )
                {
                    const QSizeF hint = control->sizeHint();

                    w = qMax( w, hint.width() );
                    h = qMax( h, hint.height() );
                }
            }
        }
    }

    return QSizeF( w, h );
}

bool QskControl::isPolishScheduled() const
{
    return d_func()->polishScheduled;
}

bool QskControl::isUpdateNodeScheduled() const
{
    Q_D( const QskControl );

    return ( d->dirtyAttributes & QQuickItemPrivate::ContentUpdateMask )
        && ( d->flags & QQuickItem::ItemHasContents );
}

bool QskControl::isInitiallyPainted() const
{
    return d_func()->isInitiallyPainted;
}

QVector< QskAspect::Subcontrol > QskControl::subControls() const
{
    using namespace QskAspect;

    QVector< Subcontrol > subControls;

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
