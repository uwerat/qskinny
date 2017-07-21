/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskDialogSubWindow.h"
#include <QEventLoop>
#include <QQuickWindow>
#include <QPointer>

QskDialogSubWindow::QskDialogSubWindow( QQuickItem* parent ):
    Inherited( parent ),
    m_result( QskDialog::Rejected ),
    m_deleteOnDone( false )
{
}

QskDialogSubWindow::~QskDialogSubWindow()
{
}

void QskDialogSubWindow::setDeleteOnDone( bool on )
{
    m_deleteOnDone = on;
}

bool QskDialogSubWindow::deleteOnDone() const
{
    return m_deleteOnDone;
}

void QskDialogSubWindow::setResult( QskDialog::DialogCode result )
{
    m_result = result;
}

QskDialog::DialogCode QskDialogSubWindow::result() const
{
    return m_result;
}

QskDialog::DialogCode QskDialogSubWindow::exec()
{
    if ( window() == nullptr )
    {
        qWarning( "trying to exec a subwindow without window. " );
        return QskDialog::Rejected;
    }

    if ( QQuickItem* mouseGrabber = window()->mouseGrabberItem() )
    {
        // when being called from QQuickWindow::mouseReleaseEvent
        // the mouse grabber has not yet been released.

        mouseGrabber->ungrabMouse();
    }

    QPointer< QQuickItem > focusItem = window()->activeFocusItem();

    show();

    QEventLoop eventLoop;

    connect( this, &QskDialogSubWindow::finished, &eventLoop, &QEventLoop::quit );
    ( void ) eventLoop.exec( QEventLoop::DialogExec );

    if ( focusItem )
        focusItem->setFocus( true );

    return m_result;
}

void QskDialogSubWindow::done( QskDialog::DialogCode result )
{
    m_result = result;

    Q_EMIT finished( result );

    if ( result == QskDialog::Accepted )
        Q_EMIT accepted();
    else
        Q_EMIT rejected();

    hide();

    if ( m_deleteOnDone )
        deleteLater();
}

void QskDialogSubWindow::accept()
{
    done( QskDialog::Accepted );
}

void QskDialogSubWindow::reject()
{
    done( QskDialog::Rejected );
}

void QskDialogSubWindow::keyPressEvent( QKeyEvent* event )
{
    if ( event->matches( QKeySequence::Cancel ) )
    {
        // using shortcuts instead ???

        reject();
        return;
    }

    Inherited::keyPressEvent( event );
}

void QskDialogSubWindow::updateLayout()
{
    if ( !isInitiallyPainted() && size().isEmpty() )
    {
        // setting an initial size from the hint, centered inside the window

        QRectF rect;
        rect.setSize( sizeHint() );
        rect.moveCenter( QPoint( 0.5 * parentItem()->width(), 0.5 * parentItem()->height() ) );

        setGeometry( rect );
    }

    Inherited::updateLayout();
}

#include "moc_QskDialogSubWindow.cpp"
