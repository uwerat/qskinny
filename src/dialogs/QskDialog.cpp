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

static QskDialog::Action qskActionCandidate( const QskDialogButtonBox* buttonBox )
{
    // not the fastest code ever, but usually we always
    // have a AcceptRole or YesRole button

    const QskDialog::ActionRole candidates[] =
    {
        QskDialog::AcceptRole, QskDialog::YesRole,
        QskDialog::RejectRole, QskDialog::NoRole, QskDialog::DestructiveRole,
        QskDialog::UserRole, QskDialog::ResetRole,
        QskDialog::ApplyRole, QskDialog::HelpRole
    };

    for ( auto role : candidates )
    {
        const auto& buttons = buttonBox->buttons( role );
        if ( !buttons.isEmpty() )
            return buttonBox->action( buttons.first() );
    }

    return QskDialog::NoAction;
}

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
    const QString& title, QskDialog::Actions actions,
    QskDialog::Action defaultAction, QskDialogSubWindow* subWindow )
{
    subWindow->setModal( true );
    subWindow->setWindowTitle( title );
    subWindow->setDialogActions( actions );

    if ( actions != QskDialog::NoAction && defaultAction == QskDialog::NoAction )
        defaultAction = qskActionCandidate( subWindow->buttonBox() );

    subWindow->setDefaultDialogAction( defaultAction );
}

static void qskSetupWindow(
    QWindow* transientParent, const QString& title,
    QskDialog::Actions actions, QskDialog::Action defaultAction,
    QskDialogWindow* window )
{
    window->setTransientParent( transientParent );

    window->setTitle( title );
    window->setDialogActions( actions );

    if ( actions != QskDialog::NoAction && defaultAction == QskDialog::NoAction )
        defaultAction = qskActionCandidate( window->buttonBox() );

    window->setDefaultDialogAction( defaultAction );

    window->setModality( transientParent ? Qt::WindowModal : Qt::ApplicationModal );

    const QSize size = window->sizeConstraint();

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

static QskDialog::Action qskMessageSubWindow(
    QQuickWindow* window, const QString& title,
    const QString& text, int symbolType, QskDialog::Actions actions,
    QskDialog::Action defaultAction )
{
    QskMessageSubWindow subWindow( window->contentItem() );
    subWindow.setSymbolType( symbolType );
    subWindow.setText( text );

    qskSetupSubWindow( title, actions, defaultAction, &subWindow );
    ( void ) subWindow.exec();

    auto clickedAction = subWindow.clickedAction();
    if ( clickedAction == QskDialog::NoAction )
    {
        // dialog might have been closed by the window menu
        clickedAction = QskDialog::Cancel;
    }

    return clickedAction;
}

static QskDialog::Action qskMessageWindow(
    QWindow* transientParent, const QString& title,
    const QString& text, int symbolType, QskDialog::Actions actions,
    QskDialog::Action defaultAction )
{
    QskMessageWindow messageWindow;
    messageWindow.setSymbolType( symbolType );
    messageWindow.setText( text );

    qskSetupWindow( transientParent, title, actions, defaultAction, &messageWindow );
    ( void ) qskExec( &messageWindow );

    auto clickedAction = messageWindow.clickedAction();
    if ( clickedAction == QskDialog::NoAction )
    {
        // dialog might have been closed by the window menu
        clickedAction = QskDialog::Cancel;
    }

    return clickedAction;
}

static QString qskSelectSubWindow(
    QQuickWindow* window, const QString& title, const QString& text,
    QskDialog::Actions actions, QskDialog::Action defaultAction,
    const QStringList& entries, int selectedRow )
{
    QskSelectionSubWindow subWindow( window->contentItem() );
    subWindow.setInfoText( text );
    subWindow.setEntries( entries );
    subWindow.setSelectedRow( selectedRow );

    QString selectedEntry = subWindow.selectedEntry();

    qskSetupSubWindow( title, actions, defaultAction, &subWindow );
    if ( subWindow.exec() == QskDialog::Accepted )
        selectedEntry = subWindow.selectedEntry();

    return selectedEntry;
}

static QString qskSelectWindow(
    QWindow* transientParent, const QString& title, const QString& text,
    QskDialog::Actions actions, QskDialog::Action defaultAction,
    const QStringList& entries, int selectedRow )
{
    QskSelectionWindow window;
    window.setInfoText( text );
    window.setEntries( entries );
    window.setSelectedRow( selectedRow );

    QString selectedEntry = window.selectedEntry();

    qskSetupWindow( transientParent, title, actions, defaultAction, &window );
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

QskDialog::Action QskDialog::message(
    const QString& title, const QString& text, int symbolType,
    Actions actions, Action defaultAction ) const
{
    if ( m_data->policy == EmbeddedBox )
    {
        auto quickWindow = qobject_cast< QQuickWindow* >( m_data->transientParent );

        if ( quickWindow == nullptr )
            quickWindow = qskSomeQuickWindow();

        if ( quickWindow )
        {
            return qskMessageSubWindow( quickWindow,
                title, text, symbolType, actions, defaultAction );
        }
    }

    return qskMessageWindow( m_data->transientParent,
        title, text, symbolType, actions, defaultAction );
}

QskDialog::Action QskDialog::information(
    const QString& title, const QString& text,
    Actions actions, Action defaultAction ) const
{
    return QskDialog::message( title, text,
        QskStandardSymbol::Information, actions, defaultAction );
}

QskDialog::Action QskDialog::warning(
    const QString& title, const QString& text,
    Actions actions, Action defaultAction ) const
{
    return QskDialog::message( title, text,
        QskStandardSymbol::Warning, actions, defaultAction );
}

QskDialog::Action QskDialog::critical(
    const QString& title, const QString& text,
    Actions actions, Action defaultAction ) const
{
    return QskDialog::message( title, text,
        QskStandardSymbol::Critical, actions, defaultAction );
}

QskDialog::Action QskDialog::question(
    const QString& title, const QString& text,
    Actions actions, Action defaultAction ) const
{
    return QskDialog::message( title, text,
        QskStandardSymbol::Question, actions, defaultAction );
}

QString QskDialog::select(
    const QString& title, const QString& text,
    const QStringList& entries, int selectedRow ) const
{
#if 1
    // should be parameters
    const QskDialog::Actions actions( QskDialog::Ok | QskDialog::Cancel );
    const QskDialog::Action defaultAction = QskDialog::Ok;
#endif

    if ( m_data->policy == EmbeddedBox )
    {
        auto quickWindow = qobject_cast< QQuickWindow* >( m_data->transientParent );

        if ( quickWindow == nullptr )
            quickWindow = qskSomeQuickWindow();

        if ( quickWindow )
        {
            return qskSelectSubWindow( quickWindow,
                title, text, actions, defaultAction, entries, selectedRow );
        }
    }

    return qskSelectWindow( m_data->transientParent, title, text,
        actions, defaultAction, entries, selectedRow );

}

#include "moc_QskDialog.cpp"
