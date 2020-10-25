/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputGrabber.h"
#include "QskWindow.h"
#include "QskEvent.h"

#include <qpointer.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
#include <private/qquickitemchangelistener_p.h>
QSK_QT_PRIVATE_END

class QskInputGrabber::PrivateData final : public QQuickItemChangeListener
{
  public:
    PrivateData( QskInputGrabber* grabber )
        : m_grabber( grabber )
    {
    }

    inline QRectF grabberRect() const
    {
        if ( itemBelow && itemAbove )
        {
            const auto pos = itemBelow->mapToItem( itemAbove, QPointF() );
            return QRectF( pos.x(), pos.y(), itemBelow->width(), itemBelow->height() );
        }

        return QRectF();
    }

    void enableListener( QQuickItem* item,
        QQuickItemPrivate::ChangeTypes types, bool on )
    {
        if ( item )
        {
            auto d = QQuickItemPrivate::get( item );

            if ( on )
                d->addItemChangeListener( this, types );
            else
                d->removeItemChangeListener( this, types );
        }
    }

    void setup( QQuickItem* item )
    {
        QQuickItem* newItemBelow = item ? item->parentItem() : nullptr;
        QQuickItem* newItemAbove = item;

        if ( newItemBelow != itemBelow )
        {
            const auto changeTypes = QQuickItemPrivate::Geometry;

            enableListener( itemBelow, changeTypes, false );
            enableListener( newItemBelow, changeTypes, true );

            itemBelow = newItemBelow;
        }

        if ( newItemAbove != itemAbove )
        {
            const auto changeTypes = QQuickItemPrivate::Geometry | QQuickItemPrivate::Parent;

            enableListener( itemAbove, changeTypes, false );
            enableListener( newItemAbove, changeTypes, true );

            itemAbove = newItemAbove;
        }
    }

  private:
#if QT_VERSION >= QT_VERSION_CHECK( 5, 8, 0 )
    void itemGeometryChanged( QQuickItem* item,
        QQuickGeometryChange change, const QRectF& ) override
    {
        bool doUpdate = false;

        if ( item == itemAbove )
            doUpdate = change.positionChange();
        else
            doUpdate = change.sizeChange();

        if ( doUpdate )
            m_grabber->updateGeometry();
    }
#else
    void itemGeometryChanged(
        QQuickItem* item, const QRectF& newRect, const QRectF& oldRect ) override
    {
        bool doUpdate = false;

        if ( item == itemAbove )
            doUpdate = newRect.topLeft() != oldRect.topLeft();
        else
            doUpdate = newRect.size() != oldRect.size();

        if ( doUpdate )
            m_grabber->updateGeometry();
    }
#endif

    void itemParentChanged( QQuickItem* item, QQuickItem* parentItem ) override
    {
        if ( item == m_grabber && parentItem )
        {
            setup( parentItem );
            m_grabber->updateGeometry();
        }
    }

    QskInputGrabber* m_grabber;

  public:
    QPointer< QQuickItem > itemAbove;
    QPointer< QQuickItem > itemBelow;
};

QskInputGrabber::QskInputGrabber( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData( this ) )
{
    setAcceptedMouseButtons( Qt::AllButtons );
#if QT_VERSION >= QT_VERSION_CHECK( 5, 10, 0 )
    setAcceptTouchEvents( true );
#endif
    setAcceptHoverEvents( true );

    setTransparentForPositioner( true );
    setFlag( QQuickItem::ItemHasContents, false );

    m_data->setup( parent );
    updateGeometry();
}

QskInputGrabber::~QskInputGrabber()
{
    m_data->setup( nullptr );
}

bool QskInputGrabber::event( QEvent* event )
{
    bool doBlock = false;

    switch ( static_cast< int >( event->type() ) )
    {
        case QEvent::MouseButtonPress:
        case QEvent::MouseMove:
        case QEvent::MouseButtonRelease:
        {
            const auto mouseEvent = static_cast< QMouseEvent* >( event );
            doBlock = isBlocking( qskMousePosition( mouseEvent ) );
            break;
        }
        case QEvent::Wheel:
        {
            const auto wheelEvent = static_cast< QWheelEvent* >( event );
            doBlock = isBlocking( qskWheelPosition( wheelEvent ) );
            break;
        }
        case QEvent::HoverEnter:
        case QEvent::HoverLeave:
        {
            const auto ev = static_cast< QHoverEvent* >( event );
            doBlock = isBlocking( qskHoverPosition( ev ) );
            break;
        }
    }

    if ( doBlock )
    {
        event->accept();

        if ( auto w = qobject_cast< QskWindow* >( window() ) )
            w->setEventAcceptance( QskWindow::EventPropagationStopped );

        return true;
    }

    return Inherited::event( event );
}

bool QskInputGrabber::isBlocking( const QPointF& pos ) const
{
    if ( const auto item = parentItem() )
        return !item->contains( position() + pos );

    return true;
}

QRectF QskInputGrabber::grabberRect() const
{
    return m_data->grabberRect();
}

void QskInputGrabber::updateGeometry()
{
    const QRectF rect = grabberRect();
    if ( rect != geometry() )
        setGeometry( rect );
}

#include "moc_QskInputGrabber.cpp"
