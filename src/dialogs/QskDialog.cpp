/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskDialog.h"
#include "QskDialogButtonBox.h"

#include "QskMessageSubWindow.h"
#include "QskMessageWindow.h"

#include "QskSelectionSubWindow.h"
#include "QskSelectionWindow.h"

#include "QskFocusIndicator.h"
#include "QskStandardSymbol.h"

#include <qguiapplication.h>
#include <qpointer.h>
#include <qquickwindow.h>

static QskDialog::DialogCode qskExec( QskDialogWindow* dialogWindow )
{
#if 1
    QskFocusIndicator* focusIndicator = new QskFocusIndicator();
    focusIndicator->setObjectName( QStringLiteral( "DialogFocusIndicator" ) );
    dialogWindow->addItem( focusIndicator );
#endif

    return dialogWindow->exec();
}

static QQuickWindow* qskSomeQuickWindow()
{
    // not the best code ever, but as it is a fallback only
    // maybe we should also add the stacking order

    QWindowList windows = QGuiApplication::topLevelWindows();
    for ( auto window : windows )
    {
        if ( window->isVisible() )
        {
            if ( auto quickWindow = qobject_cast< QQuickWindow* >( window ) )
                return quickWindow;
        }
    }

    return nullptr;
}

static void qskSetupSubWindow(
    const QString& title, QskDialog::StandardButtons buttons,
    QskDialog::StandardButton defaultButton, QskInputSubWindow* subWindow )
{
    subWindow->setModal( true );
    subWindow->setTitle( title );
    subWindow->setStandardButtons( buttons );

    if ( defaultButton == QskDialog::NoButton )
        defaultButton = subWindow->buttonBox()->defaultButtonCandidate();

    subWindow->setDefaultButton( defaultButton );
}

static void qskSetupWindow(
    QWindow* transientParent, const QString& title,
    QskDialog::StandardButtons buttons, QskDialog::StandardButton defaultButton,
    QskInputWindow* window )
{
    window->setTransientParent( transientParent );

    window->setTitle( title );
    window->setStandardButtons( buttons );

    if ( defaultButton == QskDialog::NoButton )
        defaultButton = window->buttonBox()->defaultButtonCandidate();

    window->setDefaultButton( defaultButton );

    window->setModality( transientParent ? Qt::WindowModal : Qt::ApplicationModal );

    const QSize size = window->effectivePreferredSize();

    if ( window->parent() )
    {
        QRect r( QPoint(), size );
        r.moveCenter( QRect( QPoint(), window->parent()->size() ).center() );

        window->setGeometry( r );
    }

    if ( size.isValid() )
    {
        window->setFlags( window->flags() | Qt::MSWindowsFixedSizeDialogHint );
        window->setFixedSize( size );
    }

    window->setModality( Qt::ApplicationModal );
}

static QskDialog::StandardButton qskMessageSubWindow(
    QQuickWindow* window, const QString& title,
    const QString& text, int symbolType, QskDialog::StandardButtons buttons,
    QskDialog::StandardButton defaultButton )
{
    QskMessageSubWindow subWindow( window->contentItem() );
    subWindow.setSymbolType( symbolType );
    subWindow.setInfoText( text );

    qskSetupSubWindow( title, buttons, defaultButton, &subWindow );
    ( void ) subWindow.exec();

    QskDialog::StandardButton clickedButton = subWindow.clickedButton();
    if ( clickedButton == QskDialog::NoButton )
    {
        // dialog might have been closed by the window menu
        clickedButton = QskDialog::Cancel;
    }

    return clickedButton;
}

static QskDialog::StandardButton qskMessageWindow(
    QWindow* transientParent, const QString& title,
    const QString& text, int symbolType, QskDialog::StandardButtons buttons,
    QskDialog::StandardButton defaultButton )
{
    QskMessageWindow messageWindow;
    messageWindow.setSymbolType( symbolType );
    messageWindow.setInfoText( text );

    qskSetupWindow( transientParent, title, buttons, defaultButton, &messageWindow );
    ( void ) qskExec( &messageWindow );

    QskDialog::StandardButton clickedButton = messageWindow.clickedButton();
    if ( clickedButton == QskDialog::NoButton )
    {
        // dialog might have been closed by the window menu
        clickedButton = QskDialog::Cancel;
    }

    return clickedButton;
}

static QString qskSelectSubWindow(
    QQuickWindow* window, const QString& title, const QString& text,
    QskDialog::StandardButtons buttons, QskDialog::StandardButton defaultButton,
    const QStringList& entries, int selectedRow )
{
    QskSelectionSubWindow subWindow( window->contentItem() );
    subWindow.setInfoText( text );
    subWindow.setEntries( entries );
    subWindow.setSelectedRow( selectedRow );

    QString selectedEntry = subWindow.selectedEntry();

    qskSetupSubWindow( title, buttons, defaultButton, &subWindow );
    if ( subWindow.exec() == QskDialog::Accepted )
        selectedEntry = subWindow.selectedEntry();

    return selectedEntry;
}

static QString qskSelectWindow(
    QWindow* transientParent, const QString& title, const QString& text,
    QskDialog::StandardButtons buttons, QskDialog::StandardButton defaultButton,
    const QStringList& entries, int selectedRow )
{
    QskSelectionWindow window;
    window.setInfoText( text );
    window.setEntries( entries );
    window.setSelectedRow( selectedRow );

    QString selectedEntry = window.selectedEntry();

    qskSetupWindow( transientParent, title, buttons, defaultButton, &window );
    if ( qskExec( &window ) == QskDialog::Accepted )
        selectedEntry = window.selectedEntry();

    return selectedEntry;
}

class QskDialog::PrivateData
{
  public:
    PrivateData()
        : policy( QskDialog::TopLevelWindow )
    {
    }

    QPointer< QWindow > transientParent;
    QskDialog::Policy policy : 2;
};

QskDialog::QskDialog()
    : m_data( new PrivateData )
{
}

QskDialog::~QskDialog()
{
}

QskDialog* QskDialog::instance()
{
    static QskDialog* s_instance = nullptr;
    if ( s_instance == nullptr )
        s_instance = new QskDialog();

    return s_instance;
}

void QskDialog::setPolicy( Policy policy )
{
    if ( policy != m_data->policy )
    {
        m_data->policy = policy;
        Q_EMIT policyChanged();
    }
}

QskDialog::Policy QskDialog::policy() const
{
    return m_data->policy;
}

void QskDialog::setTransientParent( QWindow* window )
{
    if ( m_data->transientParent != window )
    {
        m_data->transientParent = window;
        Q_EMIT transientParentChanged();
    }
}

QWindow* QskDialog::transientParent() const
{
    return m_data->transientParent;
}

QskDialog::StandardButton QskDialog::message(
    const QString& title, const QString& text, int symbolType,
    StandardButtons buttons, StandardButton defaultButton ) const
{
    if ( m_data->policy == EmbeddedBox )
    {
        auto quickWindow = qobject_cast< QQuickWindow* >( m_data->transientParent );

        if ( quickWindow == nullptr )
            quickWindow = qskSomeQuickWindow();

        if ( quickWindow )
        {
            return qskMessageSubWindow( quickWindow,
                title, text, symbolType, buttons, defaultButton );
        }
    }

    return qskMessageWindow( m_data->transientParent,
        title, text, symbolType, buttons, defaultButton );
}

QskDialog::StandardButton QskDialog::information(
    const QString& title, const QString& text,
    StandardButtons buttons, StandardButton defaultButton ) const
{
    return QskDialog::message( title, text,
        QskStandardSymbol::Information, buttons, defaultButton );
}

QskDialog::StandardButton QskDialog::warning(
    const QString& title, const QString& text,
    StandardButtons buttons, StandardButton defaultButton ) const
{
    return QskDialog::message( title, text,
        QskStandardSymbol::Warning, buttons, defaultButton );
}

QskDialog::StandardButton QskDialog::critical(
    const QString& title, const QString& text,
    StandardButtons buttons, StandardButton defaultButton ) const
{
    return QskDialog::message( title, text,
        QskStandardSymbol::Critical, buttons, defaultButton );
}

QskDialog::StandardButton QskDialog::question(
    const QString& title, const QString& text,
    StandardButtons buttons, StandardButton defaultButton ) const
{
    return QskDialog::message( title, text,
        QskStandardSymbol::Question, buttons, defaultButton );
}

QString QskDialog::select(
    const QString& title, const QString& text,
    const QStringList& entries, int selectedRow ) const
{
#if 1
    // should be parameters
    const QskDialog::StandardButtons buttons( QskDialog::Ok | QskDialog::Cancel );
    const QskDialog::StandardButton defaultButton = QskDialog::Ok;
#endif

    if ( m_data->policy == EmbeddedBox )
    {
        auto quickWindow = qobject_cast< QQuickWindow* >( m_data->transientParent );

        if ( quickWindow == nullptr )
            quickWindow = qskSomeQuickWindow();

        if ( quickWindow )
        {
            return qskSelectSubWindow( quickWindow,
                title, text, buttons, defaultButton, entries, selectedRow );
        }
    }

    return qskSelectWindow( m_data->transientParent, title, text,
        buttons, defaultButton, entries, selectedRow );

}

#include "moc_QskDialog.cpp"
