/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskDialogWindow.h"
#include <qeventloop.h>
#include <qpointer.h>

class QskDialogWindow::PrivateData
{
public:
    PrivateData():
        result( QskDialog::Rejected ),
        inDone( false )
    {
    }

    QskDialog::DialogCode result : 1;
    bool inDone : 1; // flag blocking recursive done calls
};

QskDialogWindow::QskDialogWindow( QWindow* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
    setFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint
        | Qt::WindowSystemMenuHint | Qt::WindowContextHelpButtonHint );
}

QskDialogWindow::~QskDialogWindow()
{
}

void QskDialogWindow::setResult( QskDialog::DialogCode result )
{
    m_data->result = result;
}

QskDialog::DialogCode QskDialogWindow::result() const
{
    return m_data->result;
}

QskDialog::DialogCode QskDialogWindow::exec()
{
    const bool deleteOnClose = this->deleteOnClose();
    setDeleteOnClose( false );

    show();

    QEventLoop eventLoop;

    connect( this, &QskDialogWindow::finished, &eventLoop, &QEventLoop::quit );
    ( void ) eventLoop.exec( QEventLoop::DialogExec );

    const QskDialog::DialogCode result =
        static_cast< QskDialog::DialogCode >( m_data->result );

    if ( deleteOnClose )
        delete this;

    return result;
}

void QskDialogWindow::done( QskDialog::DialogCode result )
{
    m_data->result = result;

    Q_EMIT finished( result );

    if ( result == QskDialog::Accepted )
        Q_EMIT accepted();
    else
        Q_EMIT rejected();

    const QPointer< QObject > that = this;

    m_data->inDone = true;

    close();

    if ( that )
    {
        // we might be deleted during close
        m_data->inDone = false;
    }
}

void QskDialogWindow::accept()
{
    done( QskDialog::Accepted );
}

void QskDialogWindow::reject()
{
    done( QskDialog::Rejected );
}

bool QskDialogWindow::event( QEvent* event )
{
    if ( event->type() == QEvent::Close )
    {
        if ( !m_data->inDone )
            done( QskDialog::Rejected );
    }

    return Inherited::event( event );
}

void QskDialogWindow::keyPressEvent( QKeyEvent* event )
{
    if ( event->matches( QKeySequence::Cancel ) )
    {
        // using shortcuts instead ???

        reject();
        return;
    }

    Inherited::keyPressEvent( event );
}

#include "moc_QskDialogWindow.cpp"
