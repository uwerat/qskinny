/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputWindow.h"
#include "QskGraphic.h"
#include "QskInputSubWindow.h"
#include "QskTextOptions.h"

QskInputWindow::QskInputWindow( QWindow* parent )
    : QskDialogWindow( parent )
    , m_subWindow( nullptr )
{
    setFlags( Qt::Dialog | Qt::WindowTitleHint |
        Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint );

    setAutoLayoutChildren( true );
}

QskInputWindow::~QskInputWindow()
{
}

void QskInputWindow::setSubWindow( QskInputSubWindow* subWindow )
{
    if ( subWindow == m_subWindow )
        return;

    if ( m_subWindow )
        delete m_subWindow;

    m_subWindow = subWindow;

    if ( m_subWindow )
    {
#if 1
        // we shoudn't have a subwindow here TODO ...
        m_subWindow->setModal( false );
        m_subWindow->setDecorated( false );
        m_subWindow->setOverlay( false );
        m_subWindow->setFaderEffect( false );
        m_subWindow->open();
#endif

        addItem( m_subWindow );

        connect( m_subWindow, &QskInputSubWindow::finished,
            this, &QskInputWindow::done );
    }
}

QskInputSubWindow* QskInputWindow::subWindow() const
{
    return m_subWindow;
}

QskDialog::Actions QskInputWindow::actions() const
{
    return m_subWindow ? m_subWindow->actions() : QskDialog::NoAction;
}

void QskInputWindow::setActions( QskDialog::Actions actions )
{
    if ( m_subWindow )
        m_subWindow->setActions( actions );
}

QskDialog::Action QskInputWindow::defaultAction() const
{
    return m_subWindow ? m_subWindow->defaultAction() : QskDialog::NoAction;
}

void QskInputWindow::setDefaultAction( QskDialog::Action action )
{
    if ( m_subWindow )
        m_subWindow->setDefaultAction( action );
}

void QskInputWindow::setInfoText( const QString& text )
{
    if ( m_subWindow && text != m_subWindow->infoText() )
    {
        m_subWindow->setInfoText( text );
        Q_EMIT infoTextChanged();
    }
}

QString QskInputWindow::infoText() const
{
    return m_subWindow ? m_subWindow->infoText() : QString();
}

void QskInputWindow::setInfoTextOptions( const QskTextOptions& options )
{
    if ( m_subWindow && options != m_subWindow->infoTextOptions() )
    {
        m_subWindow->setInfoTextOptions( options );
        Q_EMIT infoTextOptionsChanged();
    }
}

QskTextOptions QskInputWindow::infoTextOptions() const
{
    return m_subWindow ? m_subWindow->infoTextOptions() : QskTextOptions();
}

void QskInputWindow::setSymbolSource( const QUrl& url )
{
    if ( m_subWindow && url != m_subWindow->symbolSource() )
    {
        m_subWindow->setSymbolSource( url );
        Q_EMIT symbolSourceChanged();
    }
}

QUrl QskInputWindow::symbolSource() const
{
    return m_subWindow ? m_subWindow->symbolSource() : QUrl();
}

void QskInputWindow::setSymbolType( int symbolType )
{
    if ( m_subWindow )
        m_subWindow->setSymbolType( symbolType );
}

void QskInputWindow::setSymbol( const QskGraphic& symbol )
{
    if ( m_subWindow )
        m_subWindow->setSymbol( symbol );
}

QskGraphic QskInputWindow::symbol() const
{
    return m_subWindow ? m_subWindow->symbol() : QskGraphic();
}

QskDialog::Action QskInputWindow::clickedAction() const
{
    return m_subWindow->clickedAction();
}

QskDialogButtonBox* QskInputWindow::buttonBox()
{
    return m_subWindow ? m_subWindow->buttonBox() : nullptr;
}

const QskDialogButtonBox* QskInputWindow::buttonBox() const
{
    return m_subWindow ? m_subWindow->buttonBox() : nullptr;
}

#include "moc_QskInputWindow.cpp"
