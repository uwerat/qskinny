/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskControl.h"
#include "QskQuick.h"
#include "QskAspect.h"
#include "QskSetup.h"
#include "QskSkinlet.h"
#include "QskSkin.h"
#include "QskEvent.h"
#include "QskDirtyItemFilter.h"
#include "QskSkinHintTable.h"

#include <QLocale>
#include <QVector>
#include <QGlobalStatic>

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
#if defined( QT_DEBUG )
#include <private/qquickpositioners_p.h>
#endif
QSK_QT_PRIVATE_END

#include <limits>
#include <unordered_set>
#include <unordered_map>

QSK_STATE( QskControl, Disabled, QskAspect::FirstSystemState )
QSK_STATE( QskControl, Hovered, QskAspect::LastSystemState >> 1 )
QSK_STATE( QskControl, Focused, QskAspect::LastSystemState )

// QGridLayoutEngine internally uses FLT_MAX
static constexpr qreal qskSizeHintMax = std::numeric_limits< float >::max();
static QSizeF qskDefaultSizeHints[3] =
    { { 0, 0 }, { -1, -1 }, { qskSizeHintMax, qskSizeHintMax } };

typedef quint16 controlFlags_t;

void qskResolveLocale( QskControl* ); // not static as being used from outside !
static void qskUpdateControlFlags( QskControl::Flags, QskControl* );

static inline void qskSendEventTo( QObject* object, QEvent::Type type )
{
    QEvent event( type );
    QCoreApplication::sendEvent( object, &event );
}

static inline controlFlags_t qskControlFlags()
{
    // we are only interested in the first 8 bits
    return static_cast< controlFlags_t >( qskSetup->controlFlags() );
}

static inline void qskFilterWindow( QQuickWindow* window )
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

    class QskControlRegistry
    {
    public:
        QskControlRegistry()
        {
            /*
                Its faster and saves some memory to have this registry instead
                of setting up direct connections between qskSetup and each control
             */
            QObject::connect( qskSetup, &QskSetup::controlFlagsChanged,
                [this] { updateControlFlags(); } );

            QObject::connect( qskSetup, &QskSetup::skinChanged,
                [this] { updateSkin(); } );
        }

        inline void insert( QskControl* control )
        {
            m_controls.insert( control );
        }

        inline void remove( QskControl* control )
        {
            m_controls.erase( control );
        }

        void updateControlFlags()
        {
            const auto flags = static_cast< QskControl::Flags >( qskControlFlags() );

            for ( auto control : m_controls )
                qskUpdateControlFlags( flags, control );
        }

        void updateSkin()
        {
            QEvent event( QEvent::StyleChange );

            for ( auto control : m_controls )
            {
                event.setAccepted( true );
                QCoreApplication::sendEvent( control, &event );
            }
        }

    private:
        std::unordered_set< QskControl* > m_controls;
    };
}

Q_GLOBAL_STATIC( QskWindowStore, qskReleasedWindowCounter )
Q_GLOBAL_STATIC( QskControlRegistry, qskRegistry )

class QskControlPrivate final : public QQuickItemPrivate
{
    Q_DECLARE_PUBLIC( QskControl )

public:
    class ExplicitSizeData
    {
    public:
        QSizeF sizeHints[3] =
            { qskDefaultSizeHints[0], qskDefaultSizeHints[1], qskDefaultSizeHints[2] };
    };

    QskControlPrivate():
        explicitSizeData( nullptr ),
        sizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Preferred ),
        controlFlags( qskControlFlags() ),
        controlFlagsMask( 0 ),
        explicitLocale( false ),
        autoFillBackground( false ),
        autoLayoutChildren( false ),
        polishOnResize( false ),
        blockedPolish( false ),
        blockedImplicitSize( true ),
        clearPreviousNodes( false ),
        blockImplicitSizeNotification( false ),
        isInitiallyPainted( false ),
        focusPolicy( Qt::NoFocus ),
        isWheelEnabled( false )
    {
        if ( controlFlags & QskControl::DeferredLayout )
        {
            /*
                In general the geometry of an item should be the job of
                the parent - unfortunatly not done by Qt Quick
                probably in the spirit of "making things easier".

                To avoid potentially expensive calculations happening
                too often and early QskControl blocks updates of
                the implicitSize and any auto resizing of the control
                according to it.

                There should be no strong reason for using concepts
                like Positioners, that rely on implicit resizing,
                but to make it working: the DeferredLayout flag needs to be disabled.
             */

            widthValid = heightValid = true;
        }
    }

    virtual ~QskControlPrivate()
    {
        delete explicitSizeData;
    }

    virtual void mirrorChange() override final
    {
        Q_Q( QskControl );
        qskSendEventTo( q, QEvent::LayoutDirectionChange );
    }

    inline void implicitSizeChanged()
    {
        Q_Q( QskControl );
        if ( !q->explicitSizeHint( Qt::PreferredSize ).isValid() )
        {
            // when we have no PreferredSize we fall back
            // to the implicit size

            q->layoutConstraintChanged();
        }
    }

#if 0
    // can we do something useful with overloading those ???

    virtual qreal getImplicitWidth() const override final;
    virtual qreal getImplicitHeight() const override final;

    virtual QSGTransformNode* createTransformNode();
#endif

    virtual void implicitWidthChanged() override final
    {
        QQuickItemPrivate::implicitWidthChanged();

        if ( !blockImplicitSizeNotification )
            implicitSizeChanged();
    }

    virtual void implicitHeightChanged() override final
    {
        QQuickItemPrivate::implicitWidthChanged();

        if ( !blockImplicitSizeNotification )
            implicitSizeChanged();
    }

    inline void setExplicitSizeHint( Qt::SizeHint whichHint, const QSizeF& size )
    {
        if ( explicitSizeData == nullptr )
            explicitSizeData = new ExplicitSizeData;

        explicitSizeData->sizeHints[ whichHint ] = size;
    }

    inline QSizeF explicitSizeHint( Qt::SizeHint whichHint ) const
    {
        if ( explicitSizeData )
            return explicitSizeData->sizeHints[ whichHint ];

        return qskDefaultSizeHints[ whichHint ];
    }

    
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

    void updateControlFlags( QskControl::Flags flags )
    {
        Q_Q( QskControl );

        const auto oldFlags = controlFlags;
        const auto newFlags = static_cast< controlFlags_t >( flags );

        if ( oldFlags != newFlags )
        {
            const auto numBits = qCountTrailingZeroBits(
                static_cast< quint32 >( QskControl::LastFlag ) );

            for ( quint32 i = 0; i <= numBits; ++i )
            {
                const quint32 flag = ( 1 << i );
                q->updateControlFlag( flag, flags & flag );
            }

            Q_EMIT q->controlFlagsChanged();
        }
    }

private:
    ExplicitSizeData* explicitSizeData;

public:
    QLocale locale;

    QskSizePolicy sizePolicy;

    quint16 controlFlags;
    quint16 controlFlagsMask;

    bool explicitLocale : 1;

    bool autoFillBackground : 1;
    bool autoLayoutChildren : 1;
    bool polishOnResize : 1;

    bool blockedPolish : 1;
    bool blockedImplicitSize : 1;
    bool clearPreviousNodes : 1;

    bool blockImplicitSizeNotification : 1;

    bool isInitiallyPainted : 1;

    uint focusPolicy : 4;
    bool isWheelEnabled : 1;
};

static void qskUpdateControlFlags( QskControl::Flags flags, QskControl* control )
{
    auto d = static_cast< QskControlPrivate* >( QQuickItemPrivate::get( control ) );
    d->updateControlFlags( flags );
}

QskControl::QskControl( QQuickItem* parent ):
    Inherited( *( new QskControlPrivate() ), parent )
{
    setFlag( QQuickItem::ItemHasContents, true );
    QQuickItem::setActiveFocusOnTab( false );

    if ( parent )
    {
        // inheriting attributes from parent
        qskResolveLocale( this );
    }

    // since Qt 5.10 we have QQuickItem::ItemEnabledHasChanged
#if QT_VERSION < QT_VERSION_CHECK( 5, 10, 0 )
    /*
        Setting up this connections slows down the time needed
        for construction by almost 100%. Would be nice to
        avoid this penalty also for earlier Qt versions.
     */
    connect( this, &QQuickItem::enabledChanged,
        [this] { setSkinStateFlag( Disabled, !isEnabled() ); } );
#endif

    Q_D( QskControl );
    if ( d->controlFlags & QskControl::DeferredUpdate )
        qskFilterWindow( window() );

    qskRegistry->insert( this );
}

QskControl::~QskControl()
{
    if ( qskRegistry )
        qskRegistry->remove( this );

    /*
        We set componentComplete to false, so that operations
        that are triggered by detaching the item from its parent
        can be aware of the about-to-delete state.
     */
    Q_D( QskControl );
    d->componentComplete = false;
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

bool QskControl::isVisibleTo( const QQuickItem* ancestor ) const
{
    return qskIsVisibleTo( this, ancestor );
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

        /*
            Unfortunately geometryChanged is protected and we can't implement
            this code as qskSetItemGeometry - further hacking required: TODO ...
         */

        geometryChanged( QRectF( d->x, d->y, d->width, d->height ), oldRect );
    }
}

QRectF QskControl::rect() const
{
    Q_D( const QskControl );
    return QRectF( 0, 0, d->width, d->height );
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

        Q_EMIT focusPolicyChanged();
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
    Q_D( QskControl );

    // set all bits in the mask
    d->controlFlagsMask = std::numeric_limits< controlFlags_t >::max();
    d->updateControlFlags( flags );
}

void QskControl::resetControlFlags()
{
    Q_D( QskControl );

    // clear all bits in the mask
    d->controlFlagsMask = 0;
    d->updateControlFlags( static_cast< Flags >( qskControlFlags() ) );
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
            if ( flags() & QQuickItem::ItemHasContents )
                update();
            break;
        }
        default:
            break;
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

    if ( hintTable().gradient( aspect ) != gradient )
    {
        setGradientHint( aspect, gradient );
        if ( autoFillBackground() )
            update();

        Q_EMIT backgroundChanged();
    }
}

void QskControl::resetBackground()
{
    using namespace QskAspect;
    const Aspect aspect = Control | Color;

    auto& table = hintTable();

    if ( table.hint( aspect ).isValid() )
    {
        table.removeHint( aspect );

        update();
        Q_EMIT backgroundChanged();
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
        Q_EMIT marginsChanged();
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
            Q_EMIT marginsChanged();
        }
    }
}

QMarginsF QskControl::margins() const
{
    return marginsHint( QskAspect::Control | QskAspect::Margin );
}

QRectF QskControl::contentsRect() const
{
    const QRectF r = rect();

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
    auto d = static_cast< QskControlPrivate* >( QQuickItemPrivate::get( control ) );

    if ( d->explicitLocale || d->locale == locale )
        return true;

    d->locale = locale;
    qskSendEventTo( control, QEvent::LocaleChange );

    return false;
}

void qskResolveLocale( QskControl* control )
{
    const QLocale locale = qskSetup->inheritedLocale( control );

    auto d = static_cast< QskControlPrivate* >( QQuickItemPrivate::get( control ) );
    if ( d->locale != locale )
    {
        d->locale = locale;

        qskSendEventTo( control, QEvent::LocaleChange );
        qskSetup->inheritLocale( control, locale );
    }
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
}

void QskControl::setSizePolicy( const QskSizePolicy& policy )
{
    Q_D( QskControl );

    if ( policy != d->sizePolicy )
    {
        d->sizePolicy = policy;
        layoutConstraintChanged();
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
        layoutConstraintChanged();
    }
}

const QskSizePolicy& QskControl::sizePolicy() const
{
    return d_func()->sizePolicy;
}

QskSizePolicy::Policy QskControl::sizePolicy( Qt::Orientation orientation ) const
{
    return d_func()->sizePolicy.policy( orientation );
}

void QskControl::setPreferredSize( const QSizeF& size )
{
    setExplicitSizeHint( Qt::PreferredSize, size );
}

void QskControl::setPreferredSize( qreal width, qreal height )
{
    setPreferredSize( QSizeF( width, height ) );
}

void QskControl::setPreferredWidth( qreal width )
{
    setPreferredSize( QSizeF( width, preferredSize().height() ) );
}

void QskControl::setPreferredHeight( qreal height )
{
    setPreferredSize( QSizeF( preferredSize().width(), height ) );
}

void QskControl::setMinimumSize( const QSizeF& size )
{
    setExplicitSizeHint( Qt::MinimumSize, size );
}

void QskControl::setMinimumSize( qreal width, qreal height )
{
    setMinimumSize( QSizeF( width, height ) );
}

void QskControl::setMinimumWidth( qreal width )
{
    setMinimumSize( QSizeF( width, minimumSize().height() ) );
}

void QskControl::setMinimumHeight( qreal height )
{
    setMinimumSize( QSizeF( minimumSize().width(), height ) );
}

void QskControl::setMaximumSize( const QSizeF& size )
{
    setExplicitSizeHint( Qt::MaximumSize, size );
}

void QskControl::setMaximumSize( qreal width, qreal height )
{
    setMaximumSize( QSizeF( width, height ) );
}

void QskControl::setMaximumWidth( qreal width )
{
    setMaximumSize( QSizeF( width, maximumSize().height() ) );
}

void QskControl::setMaximumHeight( qreal height )
{
    setMaximumSize( QSizeF( maximumSize().width(), height ) );
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

        layoutConstraintChanged();
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

    if ( ( d->sizePolicy.horizontalPolicy() != QskSizePolicy::Fixed )
         || ( size.width() != width ) )
    {
        size.setWidth( width );

        d->sizePolicy.setHorizontalPolicy( QskSizePolicy::Fixed );
        d->setExplicitSizeHint( Qt::PreferredSize, size );

        layoutConstraintChanged();
    }
}

void QskControl::setFixedHeight( qreal height )
{
    if ( height < 0 )
        height = 0;

    Q_D( QskControl );

    auto size = d->explicitSizeHint( Qt::PreferredSize );

    if ( ( d->sizePolicy.verticalPolicy() != QskSizePolicy::Fixed )
         || ( size.height() != height ) )
    {
        size.setHeight( height );

        d->sizePolicy.setVerticalPolicy( QskSizePolicy::Fixed );
        d->setExplicitSizeHint( Qt::PreferredSize, size );

        layoutConstraintChanged();
    }
}

void QskControl::resetExplicitSizeHint( Qt::SizeHint whichHint )
{
    if ( whichHint >= Qt::MinimumSize && whichHint <= Qt::MaximumSize )
        setExplicitSizeHint( whichHint, qskDefaultSizeHints[ whichHint ] );
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
            layoutConstraintChanged();
        }
    }
}

void QskControl::setExplicitSizeHint( Qt::SizeHint whichHint, qreal width, qreal height )
{
    setExplicitSizeHint( whichHint, QSizeF( width, height ) );
}

QSizeF QskControl::explicitSizeHint( Qt::SizeHint whichHint ) const
{
    if ( whichHint >= Qt::MinimumSize && whichHint <= Qt::MaximumSize )
        return d_func()->explicitSizeHint( whichHint );

    return QSizeF( -1, -1 );
}

QSizeF QskControl::effectiveSizeHint( Qt::SizeHint whichHint ) const
{
    if ( whichHint < Qt::MinimumSize || whichHint > Qt::MaximumSize )
        return QSizeF( 0, 0 );

    QSizeF size = explicitSizeHint( whichHint );

    if ( whichHint == Qt::PreferredSize && !size.isValid() )
    {
        // in most cases we don't have a preferred width/height
        // and fall back to the implicit size.

        if ( d_func()->blockedImplicitSize )
        {
            QskControl* that = const_cast< QskControl* >( this );
            that->updateImplicitSize();
        }

        if ( size.width() < 0 )
            size.setWidth( implicitWidth() );

        if ( size.height() < 0 )
            size.setHeight( implicitHeight() );
    }

    return size;
}

void QskControl::resetImplicitSize()
{
    Q_D( QskControl );

    if ( d->controlFlags & QskControl::DeferredLayout )
    {
        if ( !d->blockedImplicitSize )
        {
            d->blockedImplicitSize = true;
            layoutConstraintChanged();
        }
    }
    else
    {
        const QSizeF sz = implicitSize();

        updateImplicitSize();

        if ( sz != implicitSize() )
            d->implicitSizeChanged();
    }
}

qreal QskControl::heightForWidth( qreal width ) const
{
    qreal h = -1;

    if ( d_func()->autoLayoutChildren )
    {
        const auto innerSize = layoutRect().size();
        const auto outerSize = size();

        width -= outerSize.width() - innerSize.width();

        const auto children = childItems();
        for ( auto child : children )
        {
            if ( auto control = qobject_cast< const QskControl* >( child ) )
            {
                if ( !control->isTransparentForPositioner() )
                    h = qMax( h, control->heightForWidth( width ) );
            }
        }

        if ( h >= 0 )
            h += outerSize.height() - innerSize.height();
    }

    return h;
}

qreal QskControl::widthForHeight( qreal height ) const
{
    qreal w = -1;
        
    if ( d_func()->autoLayoutChildren )
    {
        const auto innerSize = layoutRect().size();
        const auto outerSize = size();
        
        height -= outerSize.height() - innerSize.height();
        
        const auto children = childItems();
        for ( auto child : children )
        {   
            if ( auto control = qobject_cast< const QskControl* >( child ) )
            {   
                if ( !control->isTransparentForPositioner() )
                    w = qMax( w, control->widthForHeight( height ) );
            }
        }
        
        if ( w >= 0 )
            w += outerSize.width() - innerSize.width();
    }
    
    return w;
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

            /*
                We might have a totally different skinlet,
                that can't deal with nodes created from other skinlets
             */
            d_func()->clearPreviousNodes = true;

            resetImplicitSize();
            polish();

            if ( flags() & QQuickItem::ItemHasContents )
                update();

            changeEvent( event );
            return true;
        }

        case QEvent::EnabledChange:
        case QEvent::FontChange:
        case QEvent::PaletteChange:
        case QEvent::LocaleChange:
        case QEvent::ReadOnlyChange:
        case QEvent::ParentChange:
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

        if ( flags() & QQuickItem::ItemHasContents )
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

    qskReleasedWindowCounter->setWindow( window() );
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
            if ( value.item )
            {
                if ( !d->explicitLocale )
                    qskResolveLocale( this );
            }

            // not necessarily correct, when parent != parentItem ???
            qskSendEventTo( this, QEvent::ParentChange );

            break;
        }
        case QQuickItem::ItemChildAddedChange:
        {
            if ( d->autoLayoutChildren && !qskIsTransparentForPositioner( value.item ) )
                polish();

            break;
        }
        case QQuickItem::ItemVisibleHasChanged:
        {
#if 1
            /*
                ~QQuickItem sends QQuickItem::ItemVisibleHasChanged recursively
                to all childItems. When being a child ( not only a childItem() )
                we are short before being destructed too and any updates
                done here are totally pointless. TODO ...
             */
#endif
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

            if ( parentItem() && parentItem()->isVisible() )
            {
                /*
                    Layout code might consider the visiblility of the children
                    and therefore needs to be updated. Posting a statement about
                    changed layout constraints has this effect, but is not correct.
                    The right way to go would be to create show/hide events and to
                    handle them, where visibility of the children matters.
                    TODO ...
                 */

                layoutConstraintChanged();
            }

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
                qskReleasedWindowCounter->window(), value.window );
            QCoreApplication::sendEvent( this, &event );

            break;
        }
        case QQuickItem::ItemActiveFocusHasChanged:
        {
            setSkinStateFlag( Focused, hasActiveFocus() );
            break;
        }
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
        case QQuickItem::ItemEnabledHasChanged:
        {
            setSkinStateFlag( Disabled, !value.boolValue );
            break;
        }
#endif
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

        const auto children = childItems();
        for ( auto child : children )
        {
            if ( !QQuickItemPrivate::get( child )->isTransparentForPositioner() )
                qskSetItemGeometry( child, rect );
        }
    }

    if ( !d->isInitiallyPainted )
    {
        /*
            We should find a better way for identifying, when
            an item is about to be shown, than making it dependend
            from polishing and the existence of scene graph nodes. TODO ...
         */
        aboutToShow();
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

    if ( d->window )
    {
        // putting the nodes on the cleanup list of the window to be deleteted
        // in the next cycle of the scene graph

        QQuickWindowPrivate::get( d->window )->cleanup( d->itemNodeInstance );
    }

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
    return rect();
}

QRectF QskControl::focusIndicatorRect() const
{
    return contentsRect();
}

void QskControl::aboutToShow()
{
}

void QskControl::updateLayout()
{
}

void QskControl::updateImplicitSize()
{
    Q_D( QskControl );
    d->blockedImplicitSize = false;

    const auto m = margins();
    const auto dw = m.left() + m.right();
    const auto dh = m.top() + m.bottom();

    const QSizeF hint = contentsSizeHint();

    const qreal w = ( hint.width() >= 0 ) ? dw + hint.width() : 0.0;
    const qreal h = ( hint.height() >= 0 ) ? dh + hint.height() : 0.0;

    d->blockImplicitSizeNotification = true;
    setImplicitSize( w, h );
    d->blockImplicitSizeNotification = false;
}

QSizeF QskControl::contentsSizeHint() const
{
    qreal w = -1; // no hint
    qreal h = -1;

    if ( d_func()->autoLayoutChildren )
    {
        const auto children = childItems();
        for ( const auto child : children )
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
