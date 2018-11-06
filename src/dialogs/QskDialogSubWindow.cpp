/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskDialogSubWindow.h"
#include "QskDialogButtonBox.h"
#include "QskLinearBox.h"
#include "QskPushButton.h"
#include "QskQuick.h"

#include <qeventloop.h>
#include <qquickwindow.h>
#include <qpointer.h>

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

class QskDialogSubWindow::PrivateData
{
  public:
    QskDialog::Actions actions = QskDialog::NoAction;

    QPointer< QQuickItem > contentItem;
    QskDialogButtonBox* buttonBox = nullptr;
    QskLinearBox* layoutBox;

    QskDialog::DialogCode result = QskDialog::Rejected;
};

QskDialogSubWindow::QskDialogSubWindow( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    // doing the layout manually instead ???
    setAutoLayoutChildren( true );
    m_data->layoutBox = new QskLinearBox( Qt::Vertical, this );

    qskSetRejectOnClose( this, true );
}

QskDialogSubWindow::~QskDialogSubWindow()
{
}

void QskDialogSubWindow::setDialogActions( QskDialog::Actions actions )
{
    if ( m_data->actions == actions )
        return;

    m_data->actions = actions;

    if ( actions == QskDialog::NoAction )
    {
        delete m_data->buttonBox;
        m_data->buttonBox = nullptr;
    }
    else
    {
        if ( m_data->buttonBox == nullptr )
        {
            m_data->buttonBox = createButtonBox();

            if ( m_data->buttonBox )
            {
                m_data->layoutBox->addItem( m_data->buttonBox );

                connect( m_data->buttonBox, &QskDialogButtonBox::accepted,
                    this, &QskDialogSubWindow::accept, Qt::UniqueConnection );

                connect( m_data->buttonBox, &QskDialogButtonBox::rejected,
                    this, &QskDialogSubWindow::reject, Qt::UniqueConnection );
            }
        }

        if ( m_data->buttonBox )
            m_data->buttonBox->setActions( actions );
    }
}

QskDialog::Actions QskDialogSubWindow::dialogActions() const
{
    if ( m_data->buttonBox )
        return m_data->buttonBox->actions();

    return QskDialog::NoAction;
}

void QskDialogSubWindow::setContentItem( QQuickItem* item )
{
    if ( item == m_data->contentItem )
        return;

    if ( m_data->contentItem )
    {
        m_data->layoutBox->removeAt( 0 );
        if ( m_data->contentItem->parent() == m_data->layoutBox )
            delete m_data->contentItem;
    }

    m_data->contentItem = item;

    if ( item )
        m_data->layoutBox->insertItem( 0, item );
}

QQuickItem* QskDialogSubWindow::contentItem() const
{
    return m_data->contentItem;
}

void QskDialogSubWindow::setDefaultDialogAction( QskDialog::Action action )
{
    QskPushButton* button = nullptr;
    
    if ( m_data->buttonBox )
        button = m_data->buttonBox->button( action );

    setDefaultButton( button );
}

void QskDialogSubWindow::setDefaultButton( QskPushButton* button )
{
    if ( !qskIsAncestorOf( m_data->buttonBox, button ) )
    {
#if defined( QT_DEBUG )
        qWarning( "Only buttons of the QskDialogButtonBox can be the default button." );
#endif
        return;
    }

    m_data->buttonBox->setDefaultButton( button );
}

QskPushButton* QskDialogSubWindow::defaultButton() const
{
    if ( m_data->buttonBox == nullptr )
        return nullptr;

    return m_data->buttonBox->defaultButton();
}

QskDialogButtonBox* QskDialogSubWindow::buttonBox()
{
    return m_data->buttonBox;
}   

const QskDialogButtonBox* QskDialogSubWindow::buttonBox() const
{
    return m_data->buttonBox;
}   

QskDialog::Action QskDialogSubWindow::clickedAction() const
{
    if ( m_data->buttonBox )
        return m_data->buttonBox->clickedAction();

    return QskDialog::NoAction;
}   

void QskDialogSubWindow::setResult( QskDialog::DialogCode result )
{
    m_data->result = result;
}

QskDialog::DialogCode QskDialogSubWindow::result() const
{
    return m_data->result;
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

    open();

    QEventLoop eventLoop;

    connect( this, &QskDialogSubWindow::finished, &eventLoop, &QEventLoop::quit );
    ( void ) eventLoop.exec( QEventLoop::DialogExec );

    return m_data->result;;
}

void QskDialogSubWindow::done( QskDialog::DialogCode result )
{
    m_data->result = result;

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
    if ( m_data->buttonBox &&
        QskDialogButtonBox::isDefaultButtonKeyEvent( event ) )
    {
        auto button = m_data->buttonBox->defaultButton();
        if ( button && button->isEnabled() )
            button->click();
    }

    if ( event->matches( QKeySequence::Cancel ) )
    {
        // using shortcuts instead ???

        reject();
        return;
    }

    Inherited::keyPressEvent( event );
}

QskDialogButtonBox* QskDialogSubWindow::createButtonBox()
{
    return new QskDialogButtonBox();
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
