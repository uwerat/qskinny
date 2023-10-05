#include "QskDrawer.h"
#include "QskAspect.h"
#include "QskControl.h"

#include <QskPopup.h>
#include <QskBox.h>
#include <QskAnimationHint.h>
#include <QskQuick.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
#include <private/qquickitemchangelistener_p.h>
QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskDrawer, Panel )
QSK_SUBCONTROL( QskDrawer, Overlay )

static QRectF qskDrawerRect( const QRectF& rect,
    Qt::Edge edge, qreal pos, const QSizeF& size )
{
    QRectF r( 0.0, 0.0, size.width(), size.height() );

    const auto progress = pos = 1.0 - pos;

    switch( edge )
    {
        case Qt::LeftEdge:
        {
            r.moveRight( rect.left() + progress * size.width() );
            break;
        }
        case Qt::RightEdge:
        {
            r.moveLeft( rect.right() - progress * size.width() );
            break;
        }

        case Qt::TopEdge:
        {
            r.moveBottom( rect.top() + progress * size.height() );
            break;
        }

        case Qt::BottomEdge:
        {
            r.moveTop( rect.bottom() - progress * size.height() );
            break;
        }
    }

    return r;
}

namespace
{
    class GeometryListener final : public QQuickItemChangeListener
    {
      public:
        GeometryListener( QskDrawer* drawer )
            : m_drawer( drawer )
        {
            if ( drawer->parentItem() )
            {
                auto d = QQuickItemPrivate::get( drawer->parentItem() );
                d->addItemChangeListener( this, QQuickItemPrivate::Geometry );
            }
        }

      private:
        void itemGeometryChanged( QQuickItem*, 
            QQuickGeometryChange, const QRectF& ) override
        {
            m_drawer->polish();
        }

      private:
        QskDrawer* m_drawer = nullptr;
    };

    class Box : public QskBox
    {
        using Inherited = QskBox;

      public:
        Box( QskDrawer* drawer )
            : QskBox( drawer )
        {
            setSubcontrolProxy( QskBox::Panel, Panel );
            setPanel( true );
            setAutoLayoutChildren( true );

#if 0
            setBackgroundColor( Qt::darkCyan );
            setMargins( 20 );
#endif
        }
    };
}

class QskDrawer::PrivateData
{
  public:
    QskControl* content = nullptr;
    Box* contentBox = nullptr;
    Qt::Edge edge = Qt::LeftEdge;

    GeometryListener* listener = nullptr;
};

QskDrawer::QskDrawer( QQuickItem* parentItem )
    : Inherited ( parentItem )
    , m_data( new PrivateData )
{
    setZ( 1 );

    setPopupFlag( PopupFlag::CloseOnPressOutside, true );

    m_data->contentBox = new Box(this);

    setSubcontrolProxy( Inherited::Overlay, Overlay );

    setFaderAspect( Panel | QskAspect::Position | QskAspect::Metric );

    connect( this, &QskDrawer::closed,
        this, [this]() { startFading( false ); } );

    /*
        The drawer wants to be on top of the parent - not being
        layouted into its layoutRect(). So we opt out and do
        the layout updates manually.
     */
    setPlacementPolicy( QskPlacementPolicy::Ignore );
    if ( parentItem )
        m_data->listener = new GeometryListener( this );
}

QskDrawer::~QskDrawer()
{
    delete m_data->listener;
}

Qt::Edge QskDrawer::edge() const
{
    return m_data->edge;
}

void QskDrawer::setEdge( Qt::Edge edge )
{
    if( m_data->edge == edge )
        return;

    update();
    m_data->edge = edge;
    edgeChanged( edge );
}

void QskDrawer::setContent( QskControl* content )
{
    content->setParentItem( m_data->contentBox );
    if ( content->parent() == nullptr )
        content->setParent( m_data->contentBox );

    m_data->content = content;
}

QSizeF QskDrawer::layoutSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    // we need to handle QEvent::LayoutRequest
    return m_data->contentBox->effectiveSizeHint( which, constraint );
}

QRectF QskDrawer::layoutRectForSize( const QSizeF& size ) const
{
    return Inherited::layoutRectForSize( size );
}

void QskDrawer::updateLayout()
{
    if ( !( isOpen() || isFading() ) )
        return;

    const auto targetRect = qskItemRect( parentItem() );
    const auto size = qskConstrainedItemSize( this, targetRect.size() );

    const auto rect = qskDrawerRect( targetRect,
        m_data->edge, metric( faderAspect() ), size );

    qskSetItemGeometry( m_data->contentBox, rect );
    Inherited::updateLayout();
}

void QskDrawer::aboutToShow()
{
    startFading( true );
    Inherited::aboutToShow();
}

void QskDrawer::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    if ( change == QQuickItem::ItemParentHasChanged )
    {
        delete m_data->listener;
        m_data->listener = new GeometryListener( this );
    }
}

void QskDrawer::startFading( bool open )
{
    const auto from = open ? 1.0 : 0.0;
    const auto to = open ? 0.0 : 1.0;

    const auto hint = animationHint( Panel | QskAspect::Position );
    startTransition( faderAspect(), hint, from, to );
}

#include "moc_QskDrawer.cpp"
