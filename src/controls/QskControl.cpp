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
};

static_assert( sizeof( QskControlPrivate ) == sizeof( QQuickItemPrivate ),
    "QskControlPrivate may not have additional members." );

QskControl::QskControl( QQuickItem* parent ):
    QskControl( *( new QskControlPrivate() ), parent )
{
}

QskControl::QskControl( QQuickItemPrivate& dd, QQuickItem* parent ):
    Inherited( dd, parent ),
    m_controlFlags( qskControlFlags() ),
    m_controlFlagsMask( 0 ),
    m_explicitLocale( false ),
    m_autoFillBackground( false ),
    m_autoLayoutChildren( false ),
    m_polishOnResize( false ),
    m_blockedPolish( false ),
    m_blockedImplicitSize( true ),
    m_clearPreviousNodes( false ),
    m_isInitiallyPainted( false )
{
    Q_D( QskControl );

    if ( m_controlFlags & QskControl::DeferredLayout )
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
    setActiveFocusOnTab( false );

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

    if ( m_controlFlags & QskControl::DeferredUpdate )
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
    Q_D( const QQuickItem );
    return QRectF( d->x, d->y, d->width, d->height );
}

void QskControl::setAutoFillBackground( bool on )
{
    if ( on != m_autoFillBackground )
    {
        m_autoFillBackground = on;

        update();
        Q_EMIT controlFlagsChanged();
    }
}

bool QskControl::autoFillBackground() const
{
    return m_autoFillBackground;
}

void QskControl::setAutoLayoutChildren( bool on )
{
    if ( on != m_autoLayoutChildren )
    {
        m_autoLayoutChildren = on;
        if ( on )
            polish();

        Q_EMIT controlFlagsChanged();
    }
}

bool QskControl::autoLayoutChildren() const
{
    return m_autoLayoutChildren;
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
    Q_D( const QskControl );
    return d->isTransparentForPositioner();
}

bool QskControl::isTransparentForPositioner( const QQuickItem* item )
{
    if ( item == nullptr )
        return true;

    return QQuickItemPrivate::get( item )->isTransparentForPositioner();
}

void QskControl::setPolishOnResize( bool on )
{
    if ( on != m_polishOnResize )
    {
        m_polishOnResize = on;
        polish();

        Q_EMIT controlFlagsChanged();
    }
}

bool QskControl::polishOnResize() const
{
    return m_polishOnResize;
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
    Q_D( const QskControl );
    return d->isTabFence;
}

QskControl::Flags QskControl::controlFlags() const
{
    return QskControl::Flags( m_controlFlags );
}

void QskControl::setControlFlags( Flags flags )
{
    // set all bits in the mask
    m_controlFlagsMask = std::numeric_limits< controlFlags_t >::max();
    updateControlFlags( flags );
}

void QskControl::resetControlFlags()
{
    // clear all bits in the mask
    m_controlFlagsMask = 0;
    updateControlFlags( static_cast< Flags >( qskControlFlags() ) );
}

void QskControl::setControlFlag( Flag flag, bool on )
{
    m_controlFlagsMask |= flag;

    if ( ( m_controlFlags & flag ) != on )
    {
        updateControlFlag( flag, on );
        Q_EMIT controlFlagsChanged();
    }
}

void QskControl::resetControlFlag( Flag flag )
{
    m_controlFlagsMask &= ~flag;

    const bool on = qskSetup->testControlFlag( static_cast< QskSetup::Flag >( flag ) );

    if ( ( m_controlFlags & flag ) != on )
    {
        updateControlFlag( flag, on );
        Q_EMIT controlFlagsChanged();
    }
}

bool QskControl::testControlFlag( Flag flag ) const
{
    return m_controlFlags & flag;
}

void QskControl::updateControlFlags()
{
    updateControlFlags( static_cast< Flags >( qskControlFlags() ) );
}

void QskControl::updateControlFlags( Flags flags )
{
    const auto oldFlags = m_controlFlags;
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
        || ( bool( m_controlFlags & flag ) == on ) )
    {
        return;
    }

    if ( on )
        m_controlFlags |= flag;
    else
        m_controlFlags &= ~flag;

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
            if ( !on && m_blockedPolish )
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
    setColor( QskAspect::Control | QskAspect::Background, color );
    setAutoFillBackground( true );
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
        Aspect aspect = subControl | Margin;

        setMetric( aspect | LeftEdge, m.left() );
        setMetric( aspect | TopEdge, m.top() );
        setMetric( aspect | RightEdge, m.right() );
        setMetric( aspect | BottomEdge, m.bottom() );

        resetImplicitSize();

        if ( m_polishOnResize || m_autoLayoutChildren )
            polish();

        qskSendEventTo( this, QEvent::ContentsRectChange );
    }
}

void QskControl::resetMargins()
{
    setMargins( QMarginsF() );
}

QMarginsF QskControl::margins() const
{
    return edgeMetrics( QskAspect::Control, QskAspect::Margin );
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
    Q_D( const QskControl );
    return d->effectiveLayoutMirror;
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
    return m_locale;
}

void QskControl::setLocale( const QLocale& locale )
{
    m_explicitLocale = true;

    if ( m_locale != locale )
    {
        m_locale = locale;
        qskSendEventTo( this, QEvent::LocaleChange );
        qskSetup->inheritLocale( this, locale );
    }
}

void QskControl::resetLocale()
{
    if ( m_explicitLocale )
    {
        m_explicitLocale = false;
        qskResolveLocale( this );
    }
}

// not static as being called from QskSetup.cpp
bool qskInheritLocale( QskControl* control, const QLocale& locale )
{
    if ( control->m_explicitLocale || control->m_locale == locale )
        return false;

    control->m_locale = locale;
    qskSendEventTo( control, QEvent::LocaleChange );

    return true;
}

void qskResolveLocale( QskControl* control )
{
    const QLocale locale = qskSetup->inheritedLocale( control );

    if ( control->m_locale != locale )
    {
        control->m_locale = locale;
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

        if ( m_blockedImplicitSize )
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
    if ( m_controlFlags & QskControl::DeferredLayout )
    {
        m_blockedImplicitSize = true;
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
            if ( m_autoLayoutChildren )
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
        m_clearPreviousNodes = true;

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
        if ( m_controlFlags & QskControl::DeferredLayout )
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

    switch( change )
    {
        case QQuickItem::ItemParentHasChanged:
        {
            if ( !m_explicitLocale )
                qskResolveLocale( this );

            // not necessarily correct, when parent != parentItem ???
            qskSendEventTo( this, QEvent::ParentChange );

            break;
        }
        case QQuickItem::ItemVisibleHasChanged:
        {
            if ( value.boolValue )
            {
                if ( m_blockedPolish )
                    polish();

                if ( m_controlFlags & QskControl::DeferredUpdate )
                {
                    Q_D( QskControl );
                    if ( d->dirtyAttributes )
                        update();
                }
            }
            else
            {
                if ( m_controlFlags & QskControl::CleanupOnVisibility )
                    cleanupNodes();

                m_isInitiallyPainted = false;
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
                if ( m_controlFlags & QskControl::DeferredUpdate )
                    qskFilterWindow( value.window );
            }

            QskWindowChangeEvent event(
                qskReleasedWindowCounter.window(), value.window );
            QCoreApplication::sendEvent( this, &event );

            break;
        }
        case QQuickItem::ItemActiveFocusHasChanged:
        {
            // creating focus events ???
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
        if ( m_polishOnResize || m_autoLayoutChildren )
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
    if ( m_controlFlags & QskControl::DeferredPolish )
    {
        if ( !isVisible() )
        {
            m_blockedPolish = true;
            return;
        }
    }

    m_blockedPolish = false;

    if ( m_autoLayoutChildren )
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
    Q_ASSERT( isVisible() || !( m_controlFlags & QskControl::DeferredUpdate ) );

    if ( !m_isInitiallyPainted )
        m_isInitiallyPainted = true;

    if ( m_clearPreviousNodes )
    {
        delete node;
        node = nullptr;

        m_clearPreviousNodes = false;
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
    m_blockedImplicitSize = false;

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

    if ( m_autoLayoutChildren )
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
    Q_D( const QskControl );
    return d->polishScheduled;
}

bool QskControl::isUpdateNodeScheduled() const
{
    Q_D( const QskControl );

    return ( d->dirtyAttributes & QQuickItemPrivate::ContentUpdateMask )
        && ( d->flags & QQuickItem::ItemHasContents );
}

bool QskControl::isInitiallyPainted() const
{
    return m_isInitiallyPainted;
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
