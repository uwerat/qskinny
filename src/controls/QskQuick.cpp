/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskQuick.h"
#include "QskControl.h"
#include "QskFunctions.h"
#include <qquickitem.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qguiapplication_p.h>
#include <private/qquickitem_p.h>
QSK_QT_PRIVATE_END

#include <qpa/qplatforminputcontext.h>
#include <qpa/qplatformintegration.h>

QSizeF qskItemSize( const QQuickItem* item )
{
    // obsolete for Qt >= 5.10
    auto d = QQuickItemPrivate::get( item );
    return QSizeF( d->width, d->height );
}

QRectF qskItemRect( const QQuickItem* item )
{
    auto d = QQuickItemPrivate::get( item );
    return QRectF( 0, 0, d->width, d->height );
}

QRectF qskItemGeometry( const QQuickItem* item )
{
    auto d = QQuickItemPrivate::get( item );
    return QRectF( d->x, d->y, d->width, d->height );
}

void qskSetItemGeometry( QQuickItem* item, const QRectF& rect )
{
    if ( auto control = qskControlCast( item ) )
    {
        control->setGeometry( rect );
    }
    else
    {
        item->setPosition( rect.topLeft() );
        item->setSize( rect.size() );
    }
}

bool qskIsItemComplete( const QQuickItem* item )
{
    return QQuickItemPrivate::get( item )->componentComplete;
}

bool qskIsAncestorOf( const QQuickItem* item, const QQuickItem* child )
{
    if ( item == nullptr || child == nullptr )
        return false;

#if QT_VERSION >= QT_VERSION_CHECK( 5, 7, 0 )
    return item->isAncestorOf( child );
#else
    while ( child )
    {
        if ( child == item )
            return true;

        child = child->parentItem();
    }

    return false;
#endif
}

bool qskIsVisibleToParent( const QQuickItem* item )
{
    if ( item )
        return QQuickItemPrivate::get( item )->explicitVisible;

    return false;
}

bool qskIsVisibleTo( const QQuickItem* item, const QQuickItem* ancestor )
{
    if ( item == nullptr )
        return false;

    if ( ancestor == nullptr )
        return item->isVisible(); // like QWidget::isVisibleTo

    for ( auto it = item->parentItem();
        it != ancestor; it = it->parentItem() )
    {
        if ( it == nullptr )
            return false; // ancestor is no parent

        if ( !QQuickItemPrivate::get( it )->explicitVisible )
            return false;
    }

    return true;
}

bool qskIsTabFence( const QQuickItem* item )
{
    if ( item == nullptr )
        return false;

    return QQuickItemPrivate::get( item )->isTabFence;
}

bool qskIsPolishScheduled( const QQuickItem* item )
{
    if ( item )
        return QQuickItemPrivate::get( item )->polishScheduled;

    return false;
}

bool qskIsShortcutScope( const QQuickItem* item )
{
    if ( item == nullptr )
        return false;

    /*
        We might have something like CTRL+W to close a "window".
        But in Qt/Quick a window is not necessarily a QQuickWindow
        like we have f.e QskSubWindow.

        Maybe it's worth to introduce a shortcutScope flag but for
        the moment we simply use the isFocusScope/isTabFence combination,
        that should usually be set for those "windows".
     */

    return item->isFocusScope() && QQuickItemPrivate::get( item )->isTabFence;
}

void qskSetTransparentForPositioner( QQuickItem* item, bool on )
{
    if ( item )
        QQuickItemPrivate::get( item )->setTransparentForPositioner( on );
}

bool qskIsTransparentForPositioner( const QQuickItem* item )
{
    if ( item == nullptr )
        return true;

    return QQuickItemPrivate::get( item )->isTransparentForPositioner();
}

bool qskIsVisibleToLayout( const QQuickItem* item )
{
    if ( item )
    {
        const auto d = QQuickItemPrivate::get( item );
        return !d->isTransparentForPositioner()
            && ( d->explicitVisible || qskRetainSizeWhenHidden( item ) );
    }

    return false;
}

QskSizePolicy qskSizePolicy( const QQuickItem* item )
{
    if ( auto control = qskControlCast( item ) )
        return control->sizePolicy();

    if ( item )
    {
        const QVariant v = item->property( "sizePolicy" );
        if ( v.canConvert< QskSizePolicy >() )
            return qvariant_cast< QskSizePolicy >( v );
    }

    return QskSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Preferred );
}

Qt::Alignment qskLayoutAlignmentHint( const QQuickItem* item )
{
    if ( auto control = qskControlCast( item ) )
        return control->layoutAlignmentHint();

    if ( item )
    {
        const QVariant v = item->property( "layoutAlignmentHint" );
        if ( v.canConvert< Qt::Alignment >() )
            return v.value< Qt::Alignment >();
    }

    return Qt::Alignment();
}

bool qskRetainSizeWhenHidden( const QQuickItem* item )
{
    if ( auto control = qskControlCast( item ) )
        return control->layoutHints() & QskControl::RetainSizeWhenHidden;

    if ( item )
    {
        const QVariant v = item->property( "retainSizeWhenHidden" );
        if ( v.canConvert< bool >() )
            return v.value< bool >();
    }

    return false;
}

QQuickItem* qskNearestFocusScope( const QQuickItem* item )
{
    if ( item )
    {
        for ( auto scope = item->parentItem();
            scope != nullptr; scope = scope->parentItem() )
        {
            if ( scope->isFocusScope() )
                return scope;
        }

        /*
            As the default setting of the root item is to be a focus scope
            we usually never get here - beside the flag has been explicitely
            disabled in application code.
         */
    }

    return nullptr;
}

void qskForceActiveFocus( QQuickItem* item, Qt::FocusReason reason )
{
    /*
        For unknown reasons Qt::PopupFocusReason is blocked inside of
        QQuickItem::setFocus and so we can't use QQuickItem::forceActiveFocus
     */

    if ( item == nullptr || item->window() == nullptr )
        return;


#if QT_VERSION >= QT_VERSION_CHECK( 6, 1, 0 )
    auto wp = QQuickItemPrivate::get( item )->deliveryAgentPrivate();
#else
    auto wp = QQuickWindowPrivate::get( item->window() );
#endif
    while ( const auto scope = qskNearestFocusScope( item ) )
    {
        wp->setFocusInScope( scope, item, reason );
        item = scope;
    }
}

void qskUpdateInputMethod( const QQuickItem* item, Qt::InputMethodQueries queries )
{
    if ( item == nullptr || !( item->flags() & QQuickItem::ItemAcceptsInputMethod ) )
        return;

    static QPlatformInputContext* context = nullptr;
    static int methodId = -1;

    /*
        We could also get the inputContext from QInputMethodPrivate
        but for some reason the gcc sanitizer reports errors
        when using it. So let's go with QGuiApplicationPrivate.
     */

    auto inputContext = QGuiApplicationPrivate::platformIntegration()->inputContext();
    if ( inputContext == nullptr )
    {
        context = nullptr;
        methodId = -1;

        return;
    }

    if ( inputContext != context )
    {
        context = inputContext;
        methodId = inputContext->metaObject()->indexOfMethod(
            "update(const QQuickItem*,Qt::InputMethodQueries)" );
    }

    if ( methodId >= 0 )
    {
        /*
            The protocol for input methods does not fit well for a
            virtual keyboard as it is tied to the focus.
            So we try to bypass QInputMethod calling the
            inputContext directly.
         */

        const auto method = inputContext->metaObject()->method( methodId );
        method.invoke( inputContext, Qt::DirectConnection,
            Q_ARG( const QQuickItem*, item ), Q_ARG( Qt::InputMethodQueries, queries ) );
    }
    else
    {
        QGuiApplication::inputMethod()->update( queries );
    }
}

void qskInputMethodSetVisible( const QQuickItem* item, bool on )
{
    static QPlatformInputContext* context = nullptr;
    static int methodId = -1;

    auto inputContext = QGuiApplicationPrivate::platformIntegration()->inputContext();
    if ( inputContext == nullptr )
    {
        context = nullptr;
        methodId = -1;

        return;
    }

    if ( inputContext != context )
    {
        context = inputContext;
        methodId = inputContext->metaObject()->indexOfMethod(
            "setInputPanelVisible(const QQuickItem*,bool)" );
    }

    if ( methodId >= 0 )
    {
        const auto method = inputContext->metaObject()->method( methodId );
        method.invoke( inputContext, Qt::DirectConnection,
            Q_ARG( const QQuickItem*, item ), Q_ARG( bool, on ) );
    }
    else
    {
        if ( on )
            QGuiApplication::inputMethod()->show();
        else
            QGuiApplication::inputMethod()->hide();
    }
}

QList< QQuickItem* > qskPaintOrderChildItems( const QQuickItem* item )
{
    if ( item )
        return QQuickItemPrivate::get( item )->paintOrderChildItems();

    return QList< QQuickItem* >();
}

const QSGNode* qskItemNode( const QQuickItem* item )
{
    if ( item == nullptr )
        return nullptr;

    return QQuickItemPrivate::get( item )->itemNodeInstance;
}

const QSGNode* qskPaintNode( const QQuickItem* item )
{
    if ( item == nullptr )
        return nullptr;

    return QQuickItemPrivate::get( item )->paintNode;
}

QSizeF qskEffectiveSizeHint( const QQuickItem* item,
    Qt::SizeHint whichHint, const QSizeF& constraint )
{
    if ( auto control = qskControlCast( item ) )
        return control->effectiveSizeHint( whichHint, constraint );

    if ( constraint.width() >= 0.0 || constraint.height() >= 0.0 )
    {
        // QQuickItem does not support dynamic constraints
        return constraint;
    }

    if ( item == nullptr )
        return QSizeF();

    /*
        Trying to retrieve something useful for non QskControls:

        First are checking some properties, that usually match the
        names for the explicit hints. For the implicit hints we only
        have the implicitSize, what is interpreted as the implicit
        preferred size.
     */
    QSizeF hint;

    static const char* properties[] =
    {
        "minimumSize",
        "preferredSize",
        "maximumSize"
    };

    const auto v = item->property( properties[ whichHint ] );
    if ( v.canConvert< QSizeF >() )
        hint = v.toSizeF();

    if ( whichHint == Qt::PreferredSize )
    {
        if ( hint.width() < 0 )
            hint.setWidth( item->implicitWidth() );

        if ( hint.height() < 0 )
            hint.setHeight( item->implicitHeight() );
    }

    return hint;
}

static QSizeF qskBoundedConstraint( const QQuickItem* item,
    const QSizeF& constraint, QskSizePolicy policy )
{
    Qt::Orientation orientation;

    if ( constraint.width() >= 0.0 )
    {
        orientation = Qt::Horizontal;
    }
    else if ( constraint.height() >= 0.0 )
    {
        orientation = Qt::Vertical;
    }
    else
    {
        return constraint;
    }

    const auto whichMin = policy.effectiveSizeHintType( Qt::MinimumSize, orientation );
    const auto whichMax = policy.effectiveSizeHintType( Qt::MaximumSize, orientation );

    const auto hintMin = qskEffectiveSizeHint( item, whichMin );
    const auto hintMax = ( whichMax == whichMin )
        ? hintMin : qskEffectiveSizeHint( item, whichMax );

    QSizeF size = constraint;

    if ( orientation == Qt::Horizontal )
    {
        if ( hintMax.width() >= 0.0 )
            size.rwidth() = qMin( size.width(), hintMax.width() );

        size.rwidth() = qMax( size.width(), hintMin.width() );
    }
    else
    {
        if ( hintMax.height() >= 0.0 )
            size.rheight() = qMin( size.height(), hintMax.height() );

        size.rheight() = qMax( size.height(), hintMin.height() );
    }

    return size;
}

QSizeF qskSizeConstraint( const QQuickItem* item,
    Qt::SizeHint which, const QSizeF& constraint )
{
    if ( item == nullptr )
        return QSizeF( 0, 0 );

    if ( constraint.isValid() )
        return constraint;

    auto policy = qskSizePolicy( item );

    bool ignoreWidth = false;
    bool ignoreHeight = false;

    if ( which == Qt::PreferredSize )
    {
        /*
            First we are checking the IgnoreFlag, to avoid doing
            pointless calculations.
         */
        ignoreWidth = policy.policy( Qt::Horizontal ) & QskSizePolicy::IgnoreFlag;
        ignoreHeight = policy.policy( Qt::Vertical ) & QskSizePolicy::IgnoreFlag;

        if ( ( ignoreWidth && ignoreHeight )
            || ( ignoreWidth && constraint.height() >= 0.0 )
            || ( ignoreHeight && constraint.width() >= 0.0 ) )
        {
            return QSizeF();
        }
    }

    const auto whichH = policy.effectiveSizeHintType( which, Qt::Horizontal );
    const auto whichV = policy.effectiveSizeHintType( which, Qt::Vertical );

    QSizeF size;

    int constraintType = QskSizePolicy::Unconstrained;

    /*
        We apply a constraint - even, when the policy is unconstrained.
        Do we really want to do this ???
     */
    if ( constraint.height() >= 0.0 )
    {
        const auto c = qskBoundedConstraint( item, constraint, policy );
        size = qskEffectiveSizeHint( item, whichV, c );

        if ( ( whichH != whichV ) || ( size.height() != c.height() ) )
            constraintType = QskSizePolicy::WidthForHeight;
    }
    else if ( constraint.width() >= 0.0 )
    {
        const auto c = qskBoundedConstraint( item, constraint, policy );
        size = qskEffectiveSizeHint( item, whichH, c );

        if ( ( whichV != whichH ) || ( size.width() != c.height() ) )
            constraintType = QskSizePolicy::HeightForWidth;
    }
    else
    {
        constraintType = policy.constraintType();

        switch( constraintType )
        {
            case QskSizePolicy::WidthForHeight:
            {
                size = qskEffectiveSizeHint( item, whichV );
                break;
            }
            case QskSizePolicy::HeightForWidth:
            {
                size = qskEffectiveSizeHint( item, whichH );
                break;
            }
            default:
            {
                if ( whichV != whichH )
                {
                    if ( !ignoreWidth )
                        size.rwidth() = qskEffectiveSizeHint( item, whichH ).width();

                    if ( !ignoreHeight )
                        size.rheight() = qskEffectiveSizeHint( item, whichV ).height();
                }
                else
                {
                    size = qskEffectiveSizeHint( item, whichH );
                }
            }
        }
    }

    switch( constraintType )
    {
        case QskSizePolicy::HeightForWidth:
        {
            const QSizeF c( size.width(), -1.0 );
            size.rheight() = qskEffectiveSizeHint( item, whichV, c ).height();
            break;
        }
        case QskSizePolicy::WidthForHeight:
        {
            const QSizeF c( -1.0, size.height() );
            size.rwidth() = qskEffectiveSizeHint( item, whichH, c ).width();
            break;
        }
    }

    if ( ignoreWidth || constraint.width() >= 0.0 )
        size.rwidth() = -1.0;

    if ( ignoreHeight || constraint.height() >= 0.0 )
        size.rheight() = -1.0;

    return size;
}

QSizeF qskConstrainedItemSize( const QQuickItem* item, const QSizeF& size )
{
    if ( item == nullptr || ( size.width() <= 0.0 && size.height() <= 0.0 ) )
        return QSizeF( 0.0, 0.0 );

    QSizeF min, max;

    const auto policy = qskSizePolicy( item );

    switch( policy.constraintType() )
    {
        case QskSizePolicy::WidthForHeight:
        {
            const auto constraint = qskBoundedConstraint( item,
                QSizeF( -1.0, size.height() ), policy );

            min = qskSizeConstraint( item, Qt::MinimumSize, constraint );
            max = qskSizeConstraint( item, Qt::MaximumSize, constraint );

            min.rheight() = max.rheight() = constraint.height();
            break;
        }
        case QskSizePolicy::HeightForWidth:
        {
            const auto constraint = qskBoundedConstraint( item,
                QSizeF( size.width(), -1.0 ), policy );

            min = qskSizeConstraint( item, Qt::MinimumSize, constraint );
            max = qskSizeConstraint( item, Qt::MaximumSize, constraint );

            min.rwidth() = max.rwidth() = constraint.width();
            break;
        }
        default:
        {
            min = qskSizeConstraint( item, Qt::MinimumSize, QSizeF() );
            max = qskSizeConstraint( item, Qt::MaximumSize, QSizeF() );
        }
    }

    qreal width = size.width();
    qreal height = size.height();

    if ( max.width() >= 0.0 )
        width = qMin( width, max.width() );

    if ( max.height() >= 0.0 )
        height = qMin( height, max.height() );

    width = qMax( width, min.width() );
    height = qMax( height, min.height() );

    return QSizeF( width, height );
}

QRectF qskConstrainedItemRect( const QQuickItem* item,
    const QRectF& rect, Qt::Alignment alignment )
{
    const auto size = qskConstrainedItemSize( item, rect.size() );
    return qskAlignedRectF( rect, size.width(), size.height(), alignment );
}

void qskItemUpdateRecursive( QQuickItem* item )
{
    if ( item == nullptr )
        return;

    if ( item->flags() & QQuickItem::ItemHasContents )
        item->update();

    const auto& children = QQuickItemPrivate::get( item )->childItems;
    for ( auto child : children )
        qskItemUpdateRecursive( child );
}

#if QT_VERSION >= QT_VERSION_CHECK( 5, 8, 0 ) && QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

static const QQuickPointerTouchEvent* qskPointerPressEvent( const QQuickWindowPrivate* wd )
{
    for ( const auto event : wd->pointerEventInstances )
    {
        if ( auto touchEvent = event->asPointerTouchEvent() )
        {
            if ( touchEvent->isPressEvent() )
                return touchEvent;
        }
    }

    return nullptr;
}

#endif

bool qskGrabMouse( QQuickItem* item )
{
    if ( item == nullptr || item->window() == nullptr )
        return false;

    if ( const auto mouseGrabber = item->window()->mouseGrabberItem() )
    {
        if ( mouseGrabber == item )
            return true;

        if ( mouseGrabber->keepMouseGrab() )
        {
            // we respect this
            return false;
        }
    }

    item->setKeepMouseGrab( true );

#if QT_VERSION >= QT_VERSION_CHECK( 5, 8, 0 ) && QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

    auto wd = QQuickWindowPrivate::get( item->window() );
    if ( wd->touchMouseDevice == nullptr )
    {
        /*
            For synthesized mouse events QQuickWindow sends
            an initial QEvent::MouseButtonPress before setting
            touchMouseDevice/touchMouseId. As the mouse grabber is
            stored depending on these attributes the following
            mouse event callbacks will look for the grabber at a
            a different place as it was stored.
        */

        if ( const auto event = qskPointerPressEvent( wd ) )
        {
            if ( const auto p = event->point( 0 ) )
            {
                wd->touchMouseDevice = event->device();
                wd->touchMouseId = p->pointId();

                item->grabMouse();

                wd->touchMouseDevice = nullptr;
                wd->touchMouseId = -1;

                return true;
            }
        }
    }
#endif

    item->grabMouse();
    return true;
}

void qskUngrabMouse( QQuickItem* item )
{
    if ( item )
    {
        item->setKeepMouseGrab( false );

        if ( qskIsMouseGrabber( item ) )
            item->ungrabMouse();

    }
}

bool qskIsMouseGrabber( const QQuickItem* item )
{
    if ( item )
    {
        if ( const auto window = item->window() )
            return window->mouseGrabberItem() == item;
    }

    return false;
}
