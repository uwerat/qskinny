/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSubWindowArea.h"
#include "QskSubWindow.h"
#include "QskGradient.h"
#include "QskSkinlet.h"
#include "QskFunctions.h"
#include "QskEvent.h"

#include <QTimer>

QSK_SUBCONTROL( QskSubWindowArea, Panel )

static void qskUpdateEventFilter( QskSubWindowArea* area )
{
    for ( auto child : area->children() )
    {
        if ( auto window = qobject_cast< QskSubWindow* >( child ) )
        {
            window->removeEventFilter( area );
            window->installEventFilter( area );
        }
    }
}

static Qt::Edges qskSelectedEdges( const QRectF& rect, const QPointF& pos )
{
    const qreal tolerance = qskDpiScaled( 10.0 );

    Qt::Edges edges;
    if ( pos.x() <= rect.left() + tolerance )
    {
        edges |= Qt::LeftEdge;
    }
    else if ( pos.x() >= rect.right() - tolerance )
    {
        edges |= Qt::RightEdge;
    }

    if ( pos.y() <= rect.top() + tolerance )
    {
        edges |= Qt::TopEdge;
    }
    else if ( pos.y() >= rect.bottom() - tolerance )
    {
        edges |= Qt::BottomEdge;
    }

    return edges;
}

void qskDragWindow( const QPointF& off, Qt::Edges edges, QskSubWindow* window )
{
    if ( edges == 0 )
    {
        window->setPosition( window->position() + off );
    }
    else
    {
        QRectF rect( window->position(), window->size() );

        if ( edges & Qt::LeftEdge )
            rect.setLeft( rect.left() + off.x() );

        if ( edges & Qt::RightEdge )
            rect.setRight( rect.right() + off.x() );

        if ( edges & Qt::TopEdge )
            rect.setTop( rect.top() + off.y() );

        if ( edges & Qt::BottomEdge )
            rect.setBottom( rect.bottom() + off.y() );

        window->setGeometry( rect );
    }
}

class QskSubWindowArea::PrivateData
{
public:
    PrivateData():
        isDraggableByHeaderOnly( false ),
        isDragging( false )
    {
    }

    QskGradient gradient;

    // data about the window being dragged

    bool isDraggableByHeaderOnly : 1;
    bool isDragging : 1;
    Qt::Edges draggedEdges;
    QPoint mousePos;
};

QskSubWindowArea::QskSubWindowArea( QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
    setMargins( 0 );
    //setAcceptedMouseButtons( Qt::AllButtons );
}

QskSubWindowArea::~QskSubWindowArea()
{
}

void QskSubWindowArea::setGradient( const QskGradient& gradient )
{
    if ( gradient != m_data->gradient )
    {
        m_data->gradient = gradient;

        update();
        Q_EMIT gradientChanged();
    }
}

QskGradient QskSubWindowArea::gradient() const
{
    return m_data->gradient;
}

void QskSubWindowArea::resetGradient()
{
    if ( m_data->gradient.isValid() )
    {
        m_data->gradient.invalidate();
        update();

        Q_EMIT gradientChanged();
    }
}

void QskSubWindowArea::geometryChangeEvent( QskGeometryChangeEvent* event )
{
    if ( event->isResized() )
    {
        // what to do with the windows ???
    }

    Inherited::geometryChangeEvent( event );
}

void QskSubWindowArea::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    switch( change )
    {
        case QQuickItem::ItemChildAddedChange:
        {
            // the child is not fully constructed
            // and we have delay checking for sub windows
            QTimer::singleShot( 0, this,
                [ = ] { qskUpdateEventFilter( this ); } );

            break;
        }
        case QQuickItem::ItemChildRemovedChange:
        {
            if ( qobject_cast< QskSubWindow* >( value.item ) )
                value.item->removeEventFilter( this );

            break;
        }
        default:
        {
            break;
        }

    }
}

bool QskSubWindowArea::eventFilter( QObject* object, QEvent* event )
{
    if ( QskSubWindow* window = qobject_cast< QskSubWindow* >( object ) )
    {
        switch ( event->type() )
        {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseMove:
            {
                bool ok = mouseEventFilter( window, static_cast< QMouseEvent* >( event ) );
                if ( ok )
                    return true;

                break;
            }
            case QEvent::Enter:
            {
                setActive( window, true );
                break;
            }
            case QEvent::Leave:
            {
                setActive( window, false );
                break;
            }
            default:
            {
                break;
            }
        }
    }

    return Inherited::eventFilter( object, event );
}

bool QskSubWindowArea::mouseEventFilter( QskSubWindow* window, const QMouseEvent* event )
{
    if ( window == nullptr || window->parentItem() != this )
        return false;

    const QRectF cr = window->contentsRect();

    switch( event->type() )
    {
        case QEvent::MouseButtonPress:
        {
            if ( !( cr.contains( event->localPos() )
                && event->button() == Qt::LeftButton ) )
            {
                return false;
            }

#if 0
            // how to handle not to be process visual
            // changes for double click events ???
            if ( window->titleBarRect().contains( event->localPos() ) )
            {
                // block button press until we know it is no double click
                ///QGuiApplication::styleHints()->mouseDoubleClickInterval()
            }
#endif
            bool doDrag = !m_data->isDraggableByHeaderOnly;
            if ( !doDrag )
            {
                // dragging by title bar only
                doDrag = window->titleBarRect().contains( event->localPos() );
            }

            if ( doDrag )
            {
                m_data->isDragging = true;
                m_data->draggedEdges = qskSelectedEdges( cr, event->localPos() );
                m_data->mousePos = event->globalPos();

                setDragging( window, true );
                return true;
            }

            break;
        }
        case QEvent::MouseMove:
        {
            if ( m_data->isDragging )
            {
                qskDragWindow( event->globalPos() - m_data->mousePos,
                    m_data->draggedEdges, window );

                m_data->mousePos = event->globalPos();
                return true;
            }
            break;
        }
        case QEvent::MouseButtonRelease:
        {
            if ( m_data->isDragging )
            {
                setDragging( window, false );

                m_data->mousePos = QPoint();
                m_data->isDragging = false;

                return true;
            }
            break;
        }
        default:
        {
            break;
        }
    }

    return false;
}

void QskSubWindowArea::setDragging( QskSubWindow* window, bool on )
{
    window->setOpacity( on ? 0.6 : 1.0 );
}

void QskSubWindowArea::setActive( QskSubWindow* window, bool on )
{
    qDebug() << window << on;
}

#include "moc_QskSubWindowArea.cpp"
