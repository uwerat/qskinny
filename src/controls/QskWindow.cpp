/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskWindow.h"
#include "QskControl.h"
#include "QskEvent.h"
#include "QskQuick.h"
#include "QskSetup.h"

#include <qmath.h>
#include <qpointer.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
#include <private/qquickitemchangelistener_p.h>
#include <private/qquickwindow_p.h>
#include <private/qsgrenderer_p.h>
QSK_QT_PRIVATE_END

#include <qpa/qwindowsysteminterface.h>
#include <QGuiApplication>

// #define QSK_DEBUG_RENDER_TIMING

#ifdef QSK_DEBUG_RENDER_TIMING

#include <qelapsedtimer.h>
#include <qloggingcategory.h>
Q_LOGGING_CATEGORY( logTiming, "qsk.window.timing", QtCriticalMsg )

#endif

static void qskResolveLocale( QskWindow* );
static bool qskEnforcedSkin = false;

static inline void qskSendEventTo( QObject* object, QEvent::Type type )
{
    QEvent event( type );
    QCoreApplication::sendEvent( object, &event );
}

static QQuickItem* qskDefaultFocusItem( QQuickWindow* window )
{
    const auto children = qskPaintOrderChildItems( window->contentItem() );
    for ( auto it = children.crbegin(); it != children.crend(); ++it )
    {
        auto child = *it;

        if ( child->isFocusScope() && child->isVisible()
            && child->isEnabled() && child->activeFocusOnTab() )
        {
            return child;
        }
    }

    return window->contentItem()->nextItemInFocusChain( true );
}

namespace
{
    class ChildListener final : public QQuickItemChangeListener
    {
      public:
        void setEnabled( QQuickItem* contentItem, bool on )
        {
            m_contentItem = contentItem;

            const QQuickItemPrivate::ChangeTypes types = QQuickItemPrivate::Children;

            QQuickItemPrivate* p = QQuickItemPrivate::get( contentItem );
            if ( on )
                p->addItemChangeListener( this, types );
            else
                p->removeItemChangeListener( this, types );
        }

        void itemChildAdded( QQuickItem*, QQuickItem* ) override
        {
            QskWindow* window = static_cast< QskWindow* >( m_contentItem->window() );
            if ( window->isExposed() )
            {
                // the child is not fully constructed
                QCoreApplication::postEvent( window, new QEvent( QEvent::LayoutRequest ) );
            }
        }

      private:
        QQuickItem* m_contentItem = nullptr;
    };
}

static inline int qskToIntegerConstraint( qreal valueF )
{
    int value = -1;

    if ( valueF >= 0.0 )
    {
        if ( valueF >= std::numeric_limits< int >::max() )
            value = std::numeric_limits< int >::max();
        else
            value = ( int ) qCeil( valueF );
    }

    return value;
}

static inline void qskSetVisualizationMode(
    QQuickWindow* window, const QByteArray& mode )
{
    auto d = QQuickWindowPrivate::get( window );
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    d->visualizationMode = mode;
#else
    d->customRenderMode = mode;
#endif
}

static inline QByteArray qskVisualizationMode( const QQuickWindow* window )
{
    auto d = QQuickWindowPrivate::get( const_cast< QQuickWindow* >( window ) );
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    return d->visualizationMode;
#else
    return d->customRenderMode;
#endif
}
class QskWindowPrivate : public QQuickWindowPrivate
{
    Q_DECLARE_PUBLIC( QskWindow )

  public:
    QskWindowPrivate()
        : preferredSize( -1, -1 )
        , eventAcceptance( QskWindow::EventProcessed )
        , explicitLocale( false )
        , deleteOnClose( false )
        , autoLayoutChildren( true )
    {
    }

#ifdef QSK_DEBUG_RENDER_TIMING
    QElapsedTimer renderInterval;
#endif

    ChildListener contentItemListener;
    QLocale locale;

    // minimum/maximum constraints are offered by QWindow
    QSize preferredSize;

    QskWindow::EventAcceptance eventAcceptance;

    bool explicitLocale : 1;
    bool deleteOnClose : 1;
    bool autoLayoutChildren : 1;
};

QskWindow::QskWindow( QWindow* parent )
    : Inherited( *( new QskWindowPrivate() ), parent )
{
    QSurfaceFormat fmt = format();
    fmt.setSamples( 4 );
#if 0
    // for QOpenGLDebugLogger
    fmt.setOption( QSurfaceFormat::DebugContext );
#endif
    setFormat( fmt );

    /*
        So that inheriting/resolving of the locale works
        over all windows and items.
     */
    contentItem()->setProperty( "locale", locale() );

    if ( parent )
    {
        // also when the parent changes TODO ...
        qskResolveLocale( this );
    }

    d_func()->contentItemListener.setEnabled( contentItem(), true );

    if ( !qskEnforcedSkin )
        connect( this, &QQuickWindow::afterAnimating, this, &QskWindow::enforceSkin );
}

QskWindow::QskWindow( QQuickRenderControl* renderControl, QWindow* parent )
    : QskWindow( parent )
{
    Q_D( QskWindow );

    d->renderControl = renderControl;
    d->init( this, renderControl );
}

QskWindow::~QskWindow()
{
#if QT_VERSION < QT_VERSION_CHECK( 5, 12, 0 )
    // When being used from Qml the item destruction would run in the most
    // unefficient way, leading to lots of QQuickItem::ItemChildRemovedChange
    // depending operations.

    QVector< QPointer< QQuickItem > > items;

    const auto children = contentItem()->childItems();
    for ( auto child : children )
    {
        if ( child->parent() == contentItem() )
            items += child;
    }

    for ( auto& item : qskAsConst( items ) )
        delete item;
#endif
}

void QskWindow::setScreen( const QString& name )
{
    if ( !name.isEmpty() )
    {
        for ( auto screen : QGuiApplication::screens() )
        {
            if ( screen->name() == name )
            {
                setScreen( screen );
                return;
            }
        }
    }

    setScreen( QGuiApplication::primaryScreen() );
}

void QskWindow::resizeF( const QSizeF& size )
{
    const int w = static_cast< int >( qCeil( size.width() ) );
    const int h = static_cast< int >( qCeil( size.height() ) );

    resize( w, h );
}

bool QskWindow::deleteOnClose() const
{
    Q_D( const QskWindow );
    return d->deleteOnClose;
}

void QskWindow::setDeleteOnClose( bool on )
{
    Q_D( QskWindow );

    if ( on != d->deleteOnClose )
    {
        d->deleteOnClose = on;
        Q_EMIT deleteOnCloseChanged();
    }
}

void QskWindow::setAutoLayoutChildren( bool on )
{
    Q_D( QskWindow );

    if ( on != d->autoLayoutChildren )
    {
        d->autoLayoutChildren = on;
        if ( on )
            qskSendEventTo( this, QEvent::LayoutRequest );

        Q_EMIT autoLayoutChildrenChanged();
    }
}

bool QskWindow::autoLayoutChildren() const
{
    Q_D( const QskWindow );
    return d->autoLayoutChildren;
}

void QskWindow::addItem( QQuickItem* item )
{
    if ( item == nullptr )
        return;

    item->setParent( contentItem() );
    item->setParentItem( contentItem() );
}

void QskWindow::polishItems()
{
    Q_D( QskWindow );
    d->polishItems();
}

bool QskWindow::event( QEvent* event )
{
    /*
        Qt/Quick is lacking a flag like Qt::WA_NoMousePropagation, so
        the only way to stop propagating of input events is to accept
        the event. Then the window can't decide anymore if someone was
        interested and when to do some fallback handling.
        To improve the situation we add an extra flag in QskWindow,
        while the right class to solve this shortcoming would be QQuickWindow.
     */

    Q_D( QskWindow );
    d->eventAcceptance = EventProcessed;

    switch ( event->type() )
    {
        case QEvent::Show:
        {
            if ( size().isEmpty() )
            {
                QSize sz = sizeConstraint();
                if ( !sz.isEmpty() )
                {
                    sz = sz.expandedTo( minimumSize() );
                    sz = sz.boundedTo( maximumSize() );

                    resize( sz );
                }
            }

            break;
        }
        case QEvent::LayoutRequest:
        {
            if ( isExposed() )
                layoutItems();
            break;
        }
        case QEvent::LocaleChange:
        {
            Q_EMIT localeChanged( locale() );
            break;
        }
        case QEvent::Close:
        {
            if ( event->isAccepted() )
            {
                if ( d->deleteOnClose )
                    deleteLater();
            }
            break;
        }
        case QEvent::UpdateRequest:
        {
#ifdef QSK_DEBUG_RENDER_TIMING
            if ( logTiming().isDebugEnabled() )
            {
                if ( !d->renderInterval.isValid() )
                    d->renderInterval.start();

                qCDebug( logTiming() ) << "update timer - elapsed"
                    << d->renderInterval.restart() << objectName();
            }
#endif
            break;
        }

        default:
            break;
    }

    return Inherited::event( event );
}

void QskWindow::keyPressEvent( QKeyEvent* event )
{
    if ( qskFocusChainIncrement( event ) != 0 )
    {
        auto focusItem = activeFocusItem();
        if ( focusItem == nullptr || focusItem == contentItem() )
        {
            /*
                The Qt/Quick implementation for navigating along the
                focus tab chain gives unsufficient results, when the
                starting point is the root item. In this specific
                situation we also have to include all items being
                tab fences into consideration.

                In certain situations Qt/Quick gets even stuck in a non
                terminating loop: see Qt-Bug 65943

                So we better block the focus navigation and find the
                next focus item on our own.
             */
            ensureFocus( Qt::TabFocusReason );
            event->accept();

            return;
        }
    }

    Inherited::keyPressEvent( event );
}

void QskWindow::keyReleaseEvent( QKeyEvent* event )
{
    Inherited::keyReleaseEvent( event );
}

void QskWindow::exposeEvent( QExposeEvent* event )
{
    ensureFocus( Qt::OtherFocusReason );
    layoutItems();

    Inherited::exposeEvent( event );
}

void QskWindow::resizeEvent( QResizeEvent* event )
{
    Inherited::resizeEvent( event );

    if ( isExposed() )
        layoutItems();
}

QLocale QskWindow::locale() const
{
    Q_D( const QskWindow );
    return d->locale;
}

void QskWindow::setLocale( const QLocale& locale )
{
    Q_D( QskWindow );

    d->explicitLocale = true;

    if ( d->locale != locale )
    {
        d->locale = locale;
        qskSendEventTo( this, QEvent::LocaleChange );
        qskSetup->inheritLocale( this, locale );
    }
}

void QskWindow::resetLocale()
{
    Q_D( QskWindow );

    d->explicitLocale = false;
    qskResolveLocale( this );
}

bool qskInheritLocale( QskWindow* window, const QLocale& locale )
{
    auto d = static_cast< QskWindowPrivate* >( QQuickWindowPrivate::get( window ) );

    if ( d->explicitLocale || d->locale == locale )
        return false;

    d->locale = locale;
    qskSendEventTo( window, QEvent::LocaleChange );

    return true;
}

static void qskResolveLocale( QskWindow* window )
{
    auto d = static_cast< QskWindowPrivate* >( QQuickWindowPrivate::get( window ) );

    const QLocale locale = qskSetup->inheritedLocale( window );

    if ( d->locale != locale )
    {
        d->locale = locale;
        qskSendEventTo( window, QEvent::LocaleChange );

        qskSetup->inheritLocale( window, locale );
    }
}

void QskWindow::setPreferredSize( const QSize& size )
{
    Q_D( QskWindow );
    d->preferredSize = size;
}

QSize QskWindow::preferredSize() const
{
    Q_D( const QskWindow );
    return d->preferredSize;
}

QSize QskWindow::sizeConstraint() const
{
    Q_D( const QskWindow );

    QSizeF constraint = d->preferredSize;

    if ( !constraint.isValid() )
    {
        const bool doWidth = constraint.width() < 0;
        const bool doHeight = constraint.height() < 0;

        const auto children = contentItem()->childItems();
        for ( auto child : children )
        {
            if ( auto control = qskControlCast( child ) )
            {
                const QSizeF itemConstraint = control->sizeConstraint();

                if ( doWidth )
                    constraint.setWidth( qMax( constraint.width(), itemConstraint.width() ) );

                if ( doHeight )
                    constraint.setHeight( qMax( constraint.height(), itemConstraint.height() ) );
            }
        }
    }

    // QWindow geometries are in integers

    return QSize( qskToIntegerConstraint( constraint.width() ),
        qskToIntegerConstraint( constraint.height() ) );
}

void QskWindow::setFixedSize( const QSize& size )
{
    // ????
    setMinimumSize( size );
    setMaximumSize( size );
}

void QskWindow::layoutItems()
{
    Q_D( QskWindow );

    if ( !d->autoLayoutChildren )
        return;

    const QRectF rect = qskItemGeometry( contentItem() );

    const auto children = contentItem()->childItems();
    for ( auto child : children )
    {
        if ( !qskIsTransparentForPositioner( child ) )
        {
            const auto r = qskConstrainedItemRect( child, rect );
            qskSetItemGeometry( child, r );
        }
    }
}

void QskWindow::ensureFocus( Qt::FocusReason reason )
{
    auto focusItem = contentItem()->scopedFocusItem();

    if ( focusItem == nullptr )
    {
        focusItem = qskDefaultFocusItem( this );
        if ( focusItem )
            focusItem->setFocus( true, reason );
    }
}

void QskWindow::setCustomRenderMode( const char* mode )
{
    class RenderJob final : public QRunnable
    {
      public:
        RenderJob( QQuickWindow* window, const QByteArray mode )
            : m_window( window )
            , m_mode( mode )
        {
        }

        void run() override
        {
            qskSetVisualizationMode( m_window, m_mode );

            auto d = QQuickWindowPrivate::get( m_window );
            if ( d->renderer )
            {
                delete d->renderer->rootNode();
                delete d->renderer;
                d->renderer = nullptr;

                QMetaObject::invokeMethod( m_window, "update" );
            }
        }

      private:
        QQuickWindow* m_window;
        const QByteArray m_mode;
    };

    const QByteArray newMode( mode );

    const auto oldMode = qskVisualizationMode( this );

    if ( newMode != oldMode )
    {
        /*
            batch renderer uses an optimized memory allocation strategy,
            that is disabled, when a customRenderMode is enabled.
            This seems to be the reason for crashes, when changing the mode
            at runtime. The code above tries to get rid of all memory
            from the previous allocation strategy by deleting the renderer
            after swapping.
         */

        if ( newMode.isEmpty() != oldMode.isEmpty() )
            scheduleRenderJob( new RenderJob( this, newMode ), AfterSwapStage );
        else
            qskSetVisualizationMode( this, newMode );

        update();
    }
}

const char* QskWindow::customRenderMode() const
{
    return qskVisualizationMode( this );
}

void QskWindow::enforceSkin()
{
    if ( !qskEnforcedSkin )
    {
        // usually the skin is set in the application startup code, but if not
        // let's create a default skin on the GUI thread - whatever it is
        // good for.

        ( void ) qskSetup->skin();
        qskEnforcedSkin = true;
    }

    disconnect( this, &QQuickWindow::afterAnimating, this, &QskWindow::enforceSkin );
}

void QskWindow::setEventAcceptance( EventAcceptance acceptance )
{
    d_func()->eventAcceptance = acceptance;
}

QskWindow::EventAcceptance QskWindow::eventAcceptance() const
{
    return d_func()->eventAcceptance;
}

#include "moc_QskWindow.cpp"
