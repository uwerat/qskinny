/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskDialogSubWindow.h"

#include <qeventloop.h>
#include <qquickwindow.h>

static inline void qskSetRejectOnClose( QskDialogSubWindow* subWindow, bool on )
{
    if ( on )
    {
        QObject::connect( subWindow, &QskPopup::closed,
            subWindow, &QskDialogSubWindow::reject );
    }
    else
    {
        QObject::disconnect( subWindow, &QskPopup::closed,
            subWindow, &QskDialogSubWindow::reject );
    }
}

QskDialogSubWindow::QskDialogSubWindow( QQuickItem* parent ):
    Inherited( parent ),
    m_result( QskDialog::Rejected )
{
    qskSetRejectOnClose( this, true );
}

QskDialogSubWindow::~QskDialogSubWindow()
{
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

    show();

    QEventLoop eventLoop;

    connect( this, &QskDialogSubWindow::finished, &eventLoop, &QEventLoop::quit );
    ( void ) eventLoop.exec( QEventLoop::DialogExec );

    return m_result;
}

void QskDialogSubWindow::done( QskDialog::DialogCode result )
{
    m_result = result;

    if ( !isOpen() )
        return;

    qskSetRejectOnClose( this, false );
    close();

    Q_EMIT finished( result );

    if ( result == QskDialog::Accepted )
        Q_EMIT accepted();
    else
        Q_EMIT rejected();
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

void QskDialogSubWindow::aboutToShow()
{
    if ( size().isEmpty() )
    {
        // setting an initial size from the hint, centered inside the window

        QRectF rect;
        rect.setSize( sizeHint() );
        rect.moveCenter( QPointF( 0.5 * parentItem()->width(), 0.5 * parentItem()->height() ) );

        setGeometry( rect );
    }

    Inherited::aboutToShow();
}

#include "moc_QskDialogSubWindow.cpp"
