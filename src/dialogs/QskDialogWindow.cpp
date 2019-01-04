/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskDialogWindow.h"
#include "QskDialogButtonBox.h"
#include "QskLinearBox.h"
#include "QskPushButton.h"
#include "QskQuick.h"

#include <qeventloop.h>
#include <qpointer.h>

class QskDialogWindow::PrivateData
{
  public:
    PrivateData()
        : result( QskDialog::Rejected )
        , inDone( false )
    {
    }

    QskDialog::Actions actions = QskDialog::NoAction;

    QPointer< QQuickItem > dialogContentItem;
    QskDialogButtonBox* buttonBox = nullptr;
    QskLinearBox* layoutBox;

    QskDialog::DialogCode result : 1;
    bool inDone : 1; // flag blocking recursive done calls
};

QskDialogWindow::QskDialogWindow( QWindow* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint |
        Qt::WindowSystemMenuHint | Qt::WindowContextHelpButtonHint );

    // doing the layout manually instead ???
    setAutoLayoutChildren( true );
    m_data->layoutBox = new QskLinearBox( Qt::Vertical, contentItem() );
    m_data->layoutBox->setMargins( 5 );
}

QskDialogWindow::~QskDialogWindow()
{
}

void QskDialogWindow::setDialogActions( QskDialog::Actions actions )
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
                    this, &QskDialogWindow::accept, Qt::UniqueConnection );

                connect( m_data->buttonBox, &QskDialogButtonBox::rejected,
                    this, &QskDialogWindow::reject, Qt::UniqueConnection );
            }
        }

        if ( m_data->buttonBox )
            m_data->buttonBox->setActions( actions );
    }
}

QskDialog::Actions QskDialogWindow::dialogActions() const
{
    if ( m_data->buttonBox )
        return m_data->buttonBox->actions();

    return QskDialog::NoAction;
}

void QskDialogWindow::setDialogContentItem( QQuickItem* item )
{
    if ( item == m_data->dialogContentItem )
        return;

    if ( m_data->dialogContentItem )
    {
        m_data->layoutBox->removeAt( 0 );
        if ( m_data->dialogContentItem->parent() == m_data->layoutBox )
            delete m_data->dialogContentItem;
    }

    m_data->dialogContentItem = item;

    if ( item )
        m_data->layoutBox->insertItem( 0, item );
}

QQuickItem* QskDialogWindow::dialogContentItem() const
{
    return m_data->dialogContentItem;
}

void QskDialogWindow::setDefaultButton( QskPushButton* button )
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

QskPushButton* QskDialogWindow::defaultButton() const
{
    if ( m_data->buttonBox == nullptr )
        return nullptr;

    return m_data->buttonBox->defaultButton();
}

void QskDialogWindow::setDefaultDialogAction( QskDialog::Action action )
{
    QskPushButton* button = nullptr;

    if ( m_data->buttonBox )
        button = m_data->buttonBox->button( action );

    setDefaultButton( button );
}

QskDialogButtonBox* QskDialogWindow::buttonBox()
{
    return m_data->buttonBox;
}

const QskDialogButtonBox* QskDialogWindow::buttonBox() const
{
    return m_data->buttonBox;
}

QskDialog::Action QskDialogWindow::clickedAction() const
{
    if ( m_data->buttonBox )
        return m_data->buttonBox->clickedAction();

    return QskDialog::NoAction;
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

QskDialogButtonBox* QskDialogWindow::createButtonBox()
{
    return new QskDialogButtonBox();
}

#include "moc_QskDialogWindow.cpp"
