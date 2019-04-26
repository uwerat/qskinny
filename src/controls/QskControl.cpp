/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskControl.h"
#include "QskAspect.h"
#include "QskFunctions.h"
#include "QskDirtyItemFilter.h"
#include "QskEvent.h"
#include "QskQuick.h"
#include "QskSetup.h"
#include "QskSkin.h"
#include "QskSkinlet.h"
#include "QskSkinHintTable.h"
#include "QskLayoutConstraint.h"

#include <qglobalstatic.h>
#include <qlocale.h>
#include <qvector.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
#if defined( QT_DEBUG )
#include <private/qquickpositioners_p.h>
#endif
QSK_QT_PRIVATE_END

#include <limits>
#include <unordered_set>

QSK_SYSTEM_STATE( QskControl, Disabled, QskAspect::FirstSystemState )
QSK_SYSTEM_STATE( QskControl, Hovered, QskAspect::LastSystemState >> 1 )
QSK_SYSTEM_STATE( QskControl, Focused, QskAspect::LastSystemState )

// QGridLayoutEngine internally uses FLT_MAX
static constexpr qreal qskSizeHintMax = std::numeric_limits< float >::max();
static QSizeF qskDefaultSizeHints[ 3 ] =
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
        QskWindowStore()
            : m_refCount( 0 )
            , m_window( nullptr )
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
                qskSetup, [ this ] { updateControlFlags(); } );

            QObject::connect( qskSetup, &QskSetup::skinChanged,
                qskSetup, [ this ] { updateSkin(); } );
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

/*
    QQuickItemPrivate somehow implies, that it is about private data,
    but it is only the part of the public API, that does not fall
    under the Qt binary compatibility rules.

    Actually QQuickItemPrivate puts everything into its public section
    and classes - even from totally different modules like QC2 -
    are manipulating its members in a totally unguarded way

    While hacking internals of QQuickItemPrivate can't be considered
    being acceptable for any standards of software engineering
    we need to do the same to convince QQuickItem/QuickWindow to do what we
    need.

    This is not the way how we want to create APIs in QSkinny, but that
    does not mean, that the D-Pointer concept is bad in general:
    it allows to allocate private data of base and derived class
    together.

    At the moment QSkinny uses D-Pointer only, when deriving from
    Qt classes, but does not expose these private data to the public.
    Once QSkinny is in a stable state this might be changed - but
    without compromising the privacy of its members.
 */
class QskControlPrivate final : public QQuickItemPrivate
{
    Q_DECLARE_PUBLIC( QskControl )

  public:
    class ExplicitSizeData
    {
      public:
        QSizeF sizeHints[ 3 ] =
            { qskDefaultSizeHints[ 0 ], qskDefaultSizeHints[ 1 ], qskDefaultSizeHints[ 2 ] };
    };

    QskControlPrivate();
    ~QskControlPrivate() override;

    void mirrorChange() override;

    qreal getImplicitWidth() const override;
    qreal getImplicitHeight() const override;

    void updateImplicitSize( bool doNotify );

#if 0
    // can we do something useful with overloading ???
    QSGTransformNode* createTransformNode() override;
#endif

    void implicitWidthChanged() override;
    void implicitHeightChanged() override;

    void setExplicitSizeHint( Qt::SizeHint, const QSizeF& );
    QSizeF explicitSizeHint( Qt::SizeHint ) const;

    bool maybeGesture( QQuickItem*, QEvent* );
    void updateControlFlags( QskControl::Flags );

    /*
        Qt 5.11:
            sizeof( QQuickItemPrivate::ExtraData ) -> 184
            sizeof( QQuickItemPrivate ) -> 320

            sizeof( QskControlPrivate ) -> sizeof( QQuickItemPrivate ) + 32
            sizeof( QskSkinnable::PrivateData ) -> 40

            ( these numbers include pointers to optional extra data, that might
              increase the effective memory footprint, when being accurate ).

        It might be possible to save some bytes, but in the end QskControl
        is heavy simply because of deriving from QQuickItem. So without
        patching Qt the only way to limit the memory footprint of an application
        substantially is to limit the number of QQuickItems.

        That's why QSkinny builds more complex controls from scene graph nodes
        instead of doing QQuickItem composition. As this can only be done
        in C++ it is kind of obvious, why it is often a bad idea to build
        custom controls in QML.
     */

  private:
    void implicitSizeChanged();
    void setImplicitSize( qreal width, qreal height, bool doNotify );

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

    bool isInitiallyPainted : 1;

    uint focusPolicy : 4;
    bool isWheelEnabled : 1;
};

static inline void qskUpdateControlFlags( QskControl::Flags flags, QskControl* control )
{
    auto d = static_cast< QskControlPrivate* >( QQuickItemPrivate::get( control ) );
    d->updateControlFlags( flags );
}

QskControlPrivate::QskControlPrivate()
    : explicitSizeData( nullptr )
    , sizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Preferred )
    , controlFlags( qskControlFlags() )
    , controlFlagsMask( 0 )
    , explicitLocale( false )
    , autoFillBackground( false )
    , autoLayoutChildren( false )
    , polishOnResize( false )
    , blockedPolish( false )
    , blockedImplicitSize( true )
    , clearPreviousNodes( false )
    , isInitiallyPainted( false )
    , focusPolicy( Qt::NoFocus )
    , isWheelEnabled( false )
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

QskControlPrivate::~QskControlPrivate()
{
    delete explicitSizeData;
}

void QskControlPrivate::mirrorChange()
{
    Q_Q( QskControl );
    qskSendEventTo( q, QEvent::LayoutDirectionChange );
}

inline void QskControlPrivate::implicitSizeChanged()
{
    blockedImplicitSize = false;

    Q_Q( QskControl );
    if ( !q->explicitSizeHint( Qt::PreferredSize ).isValid() )
    {
        // when we have no PreferredSize we fall back
        // to the implicit size

        q->layoutConstraintChanged();
    }
}

qreal QskControlPrivate::getImplicitWidth() const
{
    if ( blockedImplicitSize )
    {
        auto that = const_cast< QskControlPrivate* >( this );
        that->updateImplicitSize( false );
    }

    return implicitWidth;
}

qreal QskControlPrivate::getImplicitHeight() const
{
    if ( blockedImplicitSize )
    {
        auto that = const_cast< QskControlPrivate* >( this );
        that->updateImplicitSize( false );
    }

    return implicitHeight;
}

void QskControlPrivate::updateImplicitSize( bool doNotify )
{
    Q_Q( const QskControl );

    blockedImplicitSize = false;

    const auto m = q->margins();
    const auto dw = m.left() + m.right();
    const auto dh = m.top() + m.bottom();

    const auto hint = q->contentsSizeHint();

    const qreal w = ( hint.width() >= 0 ) ? dw + hint.width() : -1.0;
    const qreal h = ( hint.height() >= 0 ) ? dh + hint.height() : -1.0;

    setImplicitSize( w, h, doNotify );
}

void QskControlPrivate::setImplicitSize( qreal w, qreal h, bool doNotify )
{
    const bool doWidth = ( w != implicitWidth );
    const bool doHeight = ( h != implicitHeight );

    if ( !( doWidth || doHeight ) )
        return; // nothing to do

    implicitWidth = w;
    implicitHeight = h;

    if ( !( widthValid && heightValid ) )
    {
        // auto adjusting the size

        const qreal oldWidth = width;
        const qreal oldHeight = height;

        if ( doWidth && !widthValid )
            width = qMax( w, qreal( 0.0 ) );

        if ( doHeight && !heightValid )
            height = qMax( h, qreal( 0.0 ) );

        if ( ( width != oldWidth ) || ( height != oldHeight ) )
        {
            dirty( QQuickItemPrivate::Size );

            const QRectF oldRect( x, y, oldWidth, oldHeight );
            const QRectF newRect( x, y, width, height );

            Q_Q( QskControl );
            q->geometryChanged( newRect, oldRect );
        }
    }

    if ( doNotify )
    {
        if ( doWidth )
            QQuickItemPrivate::implicitWidthChanged();

        if ( doHeight )
            QQuickItemPrivate::implicitHeightChanged();
    }
}

void QskControlPrivate::implicitWidthChanged()
{
    QQuickItemPrivate::implicitWidthChanged();
    implicitSizeChanged();
}

void QskControlPrivate::implicitHeightChanged()
{
    QQuickItemPrivate::implicitWidthChanged();
    implicitSizeChanged();
}

inline void QskControlPrivate::setExplicitSizeHint(
    Qt::SizeHint whichHint, const QSizeF& size )
{
    if ( explicitSizeData == nullptr )
        explicitSizeData = new ExplicitSizeData;

    explicitSizeData->sizeHints[ whichHint ] = size;
}

inline QSizeF QskControlPrivate::explicitSizeHint( Qt::SizeHint whichHint ) const
{
    if ( explicitSizeData )
        return explicitSizeData->sizeHints[ whichHint ];

    return qskDefaultSizeHints[ whichHint ];
}

bool QskControlPrivate::maybeGesture( QQuickItem* child, QEvent* event )
{
    Q_Q( QskControl );

    switch ( event->type() )
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

void QskControlPrivate::updateControlFlags( QskControl::Flags flags )
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

// --------

QskControl::QskControl( QQuickItem* parent )
    : Inherited( *( new QskControlPrivate() ), parent )
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
        [ this ] { setSkinStateFlag( Disabled, !isEnabled() ); } );
#endif

    Q_D( QskControl );
    if ( d->controlFlags & QskControl::DeferredUpdate )
        qskFilterWindow( window() );

    qskRegistry->insert( this );
}

QskControl::~QskControl()
{
#if defined( QT_DEBUG )
    if ( auto w = window() )
    {
        // to catch suicide situations as a result of mouse clicks
        Q_ASSERT( this != w->mouseGrabberItem() );
    }
#endif

    if ( qskRegistry )
        qskRegistry->remove( this );

#if QT_VERSION < QT_VERSION_CHECK( 5, 10, 0 )
    disconnect( this, &QQuickItem::enabledChanged, nullptr, nullptr );
#endif

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

    if ( ( flag > std::numeric_limits< controlFlags_t >::max() ) ||
        ( bool( d->controlFlags & flag ) == on ) )
    {
        return;
    }

    if ( on )
        d->controlFlags |= flag;
    else
        d->controlFlags &= ~flag;

    switch ( flag )
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
                d->updateImplicitSize( false );
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
    return qskValidOrEmptyInnerRect( rect(), margins() );
}

QRectF QskControl::subControlRect( QskAspect::Subcontrol subControl ) const
{   
    return effectiveSkinlet()->subControlRect( this, contentsRect(), subControl );
}

QRectF QskControl::subControlRect(
    const QSizeF& size, QskAspect::Subcontrol subControl ) const
{
    QRectF rect( 0.0, 0.0, size.width(), size.height() );
    rect = qskValidOrEmptyInnerRect( rect, margins() );

    return effectiveSkinlet()->subControlRect( this, rect, subControl );
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

    if ( recursive != d->inheritMirrorFromItem )
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

    if ( d && !d->isMirrorImplicit )
    {
        d->isMirrorImplicit = true;
        // d->inheritMirrorFromItem = false;
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

    if ( ( d->sizePolicy.horizontalPolicy() != QskSizePolicy::Fixed ) ||
        ( size.width() != width ) )
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

    if ( ( d->sizePolicy.verticalPolicy() != QskSizePolicy::Fixed ) ||
        ( size.height() != height ) )
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
        /*
            Is there a way to block consecutive calls ?
            When the parent is requesting the preferred size, we could use
            d->blockedImplicitSize, but in case of dynamic constraints we don't
            have an indication when the event has been processed. TODO ...
         */
        d->blockedImplicitSize = true;
        layoutConstraintChanged();
    }
    else
    {
        d->updateImplicitSize( true );
    }
}

qreal QskControl::heightForWidth( qreal width ) const
{
    if ( !d_func()->autoLayoutChildren )
        return -1.0;

    using namespace QskLayoutConstraint;

    return constrainedMetric(
        HeightForWidth, this, width, constrainedChildrenMetric );
}

qreal QskControl::widthForHeight( qreal height ) const
{
    if ( !d_func()->autoLayoutChildren )
        return -1.0;

    using namespace QskLayoutConstraint;

    return constrainedMetric(
        WidthForHeight, this, height, constrainedChildrenMetric );
}

bool QskControl::event( QEvent* event )
{
    const int eventType = event->type();

    switch ( eventType )
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

    switch ( eventType )
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
#if QT_VERSION < QT_VERSION_CHECK( 5, 10, 0 )

    if ( event->type() == QEvent::MouseButtonPress )
    {
        auto me = static_cast< QMouseEvent* >( event );

        if ( me->source() == Qt::MouseEventSynthesizedByQt )
        {
            /*
                Unhandled touch events result in creating synthetic
                mouse events. For all versions < 5.10 those events are
                passed through childMouseEventFilter without doing the
                extra checks, that are done for real mouse events.
                Furthermore the coordinates are relative
                to this - not to item.

                To avoid having a different behavior between using
                mouse and touch, we do those checks here.
             */

            auto itm = item;
            auto pos = item->mapFromScene( me->windowPos() );

            for ( itm = item; itm != this; itm = itm->parentItem() )
            {
                if ( itm->acceptedMouseButtons() & me->button() )
                {
                    if ( itm->contains( pos ) )
                        break;
                }

                pos += QPointF( itm->x(), itm->y() );
            }

            if ( itm != item )
            {
                if ( itm == this )
                    return false;

                QScopedPointer<QMouseEvent> clonedEvent(
                    QQuickWindowPrivate::cloneMouseEvent( me, &pos ) );

                return d_func()->maybeGesture( itm, clonedEvent.data() );
            }
        }
    }

#endif

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

void QskControl::changeEvent( QEvent* )
{
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

    switch ( change )
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
#if 0
        case QQuickItem::ItemChildRemovedChange:
        {
            // TODO ...
            break;
        }
#endif
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
                    if ( d->dirtyAttributes && ( d->flags & QQuickItem::ItemHasContents ) )
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

#if 1
            auto oldWindow = qskReleasedWindowCounter->window();
            if ( oldWindow && ( oldWindow->activeFocusItem() == this ) )
            {
                /*
                    Removing an item from the scene might result in
                    changes of the active focus item. Unfortunately the corresponding
                    FocusIn/Out events are sent, while the item tree is in an
                    invalid state.
                    When having event handlers, that do modifications of the focus
                    ( f.e. assigning the local focus, inside of a focus scope )
                    we might end up with having a dangling pointer for
                    oldWindow->activeFocusItem().
                 */
                QQuickWindowPrivate::get( oldWindow )->clearFocusInScope(
                    qskNearestFocusScope( this ), this, Qt::OtherFocusReason );
            }
#endif

            QskWindowChangeEvent event( oldWindow, value.window );
            QCoreApplication::sendEvent( this, &event );

            break;
        }
        case QQuickItem::ItemActiveFocusHasChanged:
        {
            setSkinStateFlag( Focused, hasActiveFocus() );
            break;
        }
#if QT_VERSION >= QT_VERSION_CHECK( 5, 10, 0 )
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

void QskControl::windowDeactivateEvent()
{
    // stopping gesture recognition ???
    Inherited::windowDeactivateEvent();
}

void QskControl::layoutConstraintChanged()
{
    if ( auto item = parentItem() )
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

QRectF QskControl::layoutRectForSize( const QSizeF& size ) const
{
    const QRectF r( 0.0, 0.0, size.width(), size.height() );
    return qskValidOrEmptyInnerRect( r, margins() );
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

QSizeF QskControl::contentsSizeHint() const
{
    qreal w = -1; // no hint
    qreal h = -1;

    if ( d_func()->autoLayoutChildren )
    {
        const auto children = childItems();
        for ( const auto child : children )
        {
            if ( auto control = qskControlCast( child ) )
            {
                if ( !control->isTransparentForPositioner() )
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

    return ( d->dirtyAttributes & QQuickItemPrivate::ContentUpdateMask ) &&
           ( d->flags & QQuickItem::ItemHasContents );
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
