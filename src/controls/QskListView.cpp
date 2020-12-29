/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskListView.h"
#include "QskAspect.h"
#include "QskColorFilter.h"

QSK_SUBCONTROL( QskListView, Cell )
QSK_SUBCONTROL( QskListView, Text )
QSK_SUBCONTROL( QskListView, CellSelected )
QSK_SUBCONTROL( QskListView, TextSelected )

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

    QskTextOptions textOptions;
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
    if ( textOptions != m_data->textOptions )
    {
        m_data->textOptions = textOptions;
        updateScrollableSize();

        Q_EMIT textOptionsChanged();
    }
}

QskTextOptions QskListView::textOptions() const
{
    return m_data->textOptions;
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
    Q_UNUSED( row );
    Q_UNUSED( col );
    return QskColorFilter();
}

QskAspect::Subcontrol QskListView::textSubControlAt( int row, int col ) const
{
    Q_UNUSED( col );
    return ( row == selectedRow() ) ? TextSelected : Text;
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
            return Inherited::keyPressEvent( event );
        }
        default:
        {
            return Inherited::keyPressEvent( event );
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
