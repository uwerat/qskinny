/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskListView.h"
#include "QskAspect.h"
#include "QskColorFilter.h"
#include "QskEvent.h"

#include <qguiapplication.h>
#include <qstylehints.h>

#include <qmath.h>

QSK_SUBCONTROL( QskListView, Cell )
QSK_SUBCONTROL( QskListView, Text )

QSK_STATE( QskListView, Selected, QskAspect::FirstUserState )

class QskListView::PrivateData
{
  public:
    PrivateData()
        : preferredWidthFromColumns( false )
        , alternatingRowColors( false )
        , selectionMode( QskListView::SingleSelection )
        , selectedRow( -1 )
    {
    }

    bool preferredWidthFromColumns : 1;
    bool alternatingRowColors : 1;
    SelectionMode selectionMode : 4;

    int selectedRow;
};

QskListView::QskListView( QQuickItem* parent )
    : QskScrollView( parent )
    , m_data( new PrivateData() )
{
}

QskListView::~QskListView()
{
}

void QskListView::setPreferredWidthFromColumns( bool on )
{
    if ( on != m_data->preferredWidthFromColumns )
    {
        m_data->preferredWidthFromColumns = on;
        resetImplicitSize();

        Q_EMIT preferredWidthFromColumnsChanged();
    }
}

bool QskListView::preferredWidthFromColumns() const
{
    return m_data->preferredWidthFromColumns;
}

void QskListView::setAlternatingRowColors( bool on )
{
    if ( on != m_data->alternatingRowColors )
    {
        m_data->alternatingRowColors = on;
        update();

        Q_EMIT alternatingRowColorsChanged();
    }
}

bool QskListView::alternatingRowColors() const
{
    return m_data->alternatingRowColors;
}

void QskListView::setTextOptions( const QskTextOptions& textOptions )
{
    if ( setTextOptionsHint( Text, textOptions ) )
    {
        updateScrollableSize();
        Q_EMIT textOptionsChanged();
    }
}

QskTextOptions QskListView::textOptions() const
{
    return textOptionsHint( Text );
}

void QskListView::setSelectedRow( int row )
{
    if ( row < 0 )
        row = -1;

    if ( row >= rowCount() )
    {
        if ( !isComponentComplete() )
        {
            // when being called from Qml we delay the checks until
            // componentComplete
        }
        else
        {
            if ( row >= rowCount() )
                row = -1;
        }
    }

    if ( row != m_data->selectedRow )
    {
        m_data->selectedRow = row;
        Q_EMIT selectedRowChanged( row );

        update();
    }
}

int QskListView::selectedRow() const
{
    return m_data->selectedRow;
}

void QskListView::setSelectionMode( SelectionMode mode )
{
    if ( mode != m_data->selectionMode )
    {
        m_data->selectionMode = mode;

        if ( m_data->selectionMode == NoSelection )
            setSelectedRow( -1 );

        Q_EMIT selectionModeChanged();
    }
}

QskListView::SelectionMode QskListView::selectionMode() const
{
    return m_data->selectionMode;
}

QskColorFilter QskListView::graphicFilterAt( int row, int col ) const
{
    Q_UNUSED( row )
    Q_UNUSED( col )

    return QskColorFilter();
}

void QskListView::keyPressEvent( QKeyEvent* event )
{
    if ( m_data->selectionMode == NoSelection )
    {
        Inherited::keyPressEvent( event );
        return;
    }

    int row = selectedRow();

    switch ( event->key() )
    {
        case Qt::Key_Down:
        {
            if ( row < rowCount() - 1 )
                row++;
            break;
        }
        case Qt::Key_Up:
        {
            if ( row == -1 )
                row = rowCount() - 1;

            if ( row != 0 )
                row--;

            break;
        }
        case Qt::Key_Home:
        {
            row = 0;
            break;
        }
        case Qt::Key_End:
        {
            row = rowCount() - 1;
            break;
        }
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
        {
            // TODO ...
            Inherited::keyPressEvent( event );
            return;
        }
        default:
        {
            Inherited::keyPressEvent( event );
            return;
        }
    }

    const int r = selectedRow();

    setSelectedRow( row );

    row = selectedRow();

    if ( row != r )
    {
        auto pos = scrollPos();

        const qreal rowPos = row * rowHeight();
        if ( rowPos < scrollPos().y() )
        {
            pos.setY( rowPos );
        }
        else
        {
            const QRectF vr = viewContentsRect();

            const double scrolledBottom = scrollPos().y() + vr.height();
            if ( rowPos + rowHeight() > scrolledBottom )
            {
                const double y = rowPos + rowHeight() - vr.height();
                pos.setY( y );
            }
        }

        if ( pos != scrollPos() )
        {
            if ( event->isAutoRepeat() )
                setScrollPos( pos );
            else
                scrollTo( pos );
        }
    }
}

void QskListView::keyReleaseEvent( QKeyEvent* event )
{
    Inherited::keyReleaseEvent( event );
}

void QskListView::mousePressEvent( QMouseEvent* event )
{
    if ( m_data->selectionMode != NoSelection )
    {
        const QRectF vr = viewContentsRect();
        if ( vr.contains( event->pos() ) )
        {
            const int row = ( event->pos().y() - vr.top() + scrollPos().y() ) / rowHeight();
            if ( row >= 0 && row < rowCount() )
                setSelectedRow( row );

            return;
        }
    }

    Inherited::mousePressEvent( event );
}

void QskListView::mouseReleaseEvent( QMouseEvent* event )
{
    Inherited::mouseReleaseEvent( event );
}

#ifndef QT_NO_WHEELEVENT

static qreal qskAlignedToRows( const qreal y0, qreal dy,
    qreal rowHeight, qreal viewHeight )
{
    qreal y = y0 - dy;

    if ( dy > 0 )
    {
        y = qFloor( y / rowHeight ) * rowHeight;
    }
    else
    {
        y += viewHeight;
        y = qCeil( y / rowHeight ) * rowHeight;
        y -= viewHeight;
    }

    return y;
}

QPointF QskListView::scrollOffset( const QWheelEvent* event ) const
{
    QPointF offset;

    const auto pos = qskWheelPosition( event );

    if ( subControlRect( VerticalScrollBar ).contains( pos ) )
    {
        const auto steps = qskWheelSteps( event );
        offset.setY( steps );
    }
    else if ( subControlRect( HorizontalScrollBar ).contains( pos ) )
    {
        const auto steps = qskWheelSteps( event );
        offset.setX( steps );
    }
    else if ( viewContentsRect().contains( pos ) )
    {
        offset = event->angleDelta() / QWheelEvent::DefaultDeltasPerStep;
    }

    if ( offset.x() != 0.0 )
    {
        offset.rx() *= viewContentsRect().width(); // pagewise
    }
    else if ( offset.y() != 0.0 )
    {
        const qreal y0 = scrollPos().y();
        const auto viewHeight = viewContentsRect().height();
        const qreal rowHeight = this->rowHeight();

        const int numLines = QGuiApplication::styleHints()->wheelScrollLines();

        qreal dy = numLines * rowHeight;
        if ( event->modifiers() & ( Qt::ControlModifier | Qt::ShiftModifier ) )
            dy = qMax( dy, viewHeight );

        dy *= offset.y(); // multiplied by the wheelsteps

        // aligning rows that enter the view
        dy = qskAlignedToRows( y0, dy, rowHeight, viewHeight );

        offset.setY( y0 - dy );
    }

    // TODO using the animated scrollTo instead ?

    return offset;
}

#endif

void QskListView::updateScrollableSize()
{
    const double h = rowCount() * rowHeight();

    qreal w = 0.0;
    for ( int col = 0; col < columnCount(); col++ )
        w += columnWidth( col );

    const QSizeF sz = scrollableSize();

    setScrollableSize( QSizeF( w, h ) );

    if ( m_data->preferredWidthFromColumns &&
        sz.width() != scrollableSize().width() )
    {
        resetImplicitSize();
    }
}

void QskListView::componentComplete()
{
    Inherited::componentComplete();

    if ( m_data->selectedRow >= 0 )
    {
        // during Qml instantiation we might have set an invalid
        // row selection

        if ( m_data->selectedRow >= rowCount() )
            setSelectedRow( -1 );
    }
}

#include "moc_QskListView.cpp"
