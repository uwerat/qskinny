/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputWindow.h"
#include "QskInputSubWindow.h"
#include "QskTextOptions.h"
#include "QskGraphic.h"

QskInputWindow::QskInputWindow( QWindow* parent ):
    QskDialogWindow( parent ),
    m_subWindow( nullptr )
{
    setFlags( Qt::Dialog | Qt::WindowTitleHint
        | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint );

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
        m_subWindow->setModal( false );
        m_subWindow->setDecorated( false );
        m_subWindow->setOverlay( false );

        addItem( m_subWindow );

        connect( m_subWindow, &QskInputSubWindow::finished,
            this, &QskInputWindow::done );
    }
}

QskInputSubWindow* QskInputWindow::subWindow() const
{
    return m_subWindow;
}

QskDialog::StandardButtons QskInputWindow::standardButtons() const
{
    return m_subWindow ? m_subWindow->standardButtons() : QskDialog::NoButton;
}

void QskInputWindow::setStandardButtons( QskDialog::StandardButtons buttons )
{
    if ( m_subWindow )
        m_subWindow->setStandardButtons( buttons );
}

QskDialog::StandardButton QskInputWindow::defaultButton() const
{
    return m_subWindow ? m_subWindow->defaultButton() : QskDialog::NoButton;
}

void QskInputWindow::setDefaultButton( QskDialog::StandardButton button )
{
    if ( m_subWindow )
        m_subWindow->setDefaultButton( button );
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
    return m_subWindow ? m_subWindow->infoText() : QString::null;
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

QskDialog::StandardButton QskInputWindow::clickedButton() const
{
    return m_subWindow->clickedButton();
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
