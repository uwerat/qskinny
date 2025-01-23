/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskDialog.h"
#include "QskDialogButtonBox.h"

#include "QskLinearBox.h"

#include "QskMessageSubWindow.h"
#include "QskMessageWindow.h"

#include "QskSelectionSubWindow.h"
#include "QskSelectionWindow.h"

#include "QskPushButton.h"
#include "QskScrollArea.h"
#include "QskSimpleListBox.h"

#include "QskFocusIndicator.h"

#include <qfilesystemmodel.h>
#include <qguiapplication.h>
#include <qpointer.h>
#include <qquickwindow.h>

#include <qpa/qplatformdialoghelper.h>

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
    auto focusIndicator = new QskFocusIndicator();
    focusIndicator->setObjectName( QStringLiteral( "DialogFocusIndicator" ) );
    dialogWindow->addItem( focusIndicator );
#endif

    return dialogWindow->exec();
}

namespace
{
    template< typename W >
    class WindowOrSubWindow : public W
    {
      public:
        WindowOrSubWindow( QObject* parent, const QString& title,
            QskDialog::Actions actions, QskDialog::Action defaultAction )
            : W( parent, title, actions, defaultAction )
        {
        }
    };

    template<>
    class WindowOrSubWindow< QskDialogWindow > : public QskDialogWindow
    {
      public:
        WindowOrSubWindow( QObject* parent, const QString& title,
            QskDialog::Actions actions, QskDialog::Action defaultAction )
            : QskDialogWindow( static_cast< QWindow* >( parent ) )
        {
            auto* transientParent = static_cast< QWindow* >( parent );
            setTransientParent( transientParent );

            setTitle( title );
            setDialogActions( actions );

            if ( actions != QskDialog::NoAction && defaultAction == QskDialog::NoAction )
                defaultAction = qskActionCandidate( buttonBox() );

            setDefaultDialogAction( defaultAction );

            setModality( transientParent ? Qt::WindowModal : Qt::ApplicationModal );

            const QSize size = sizeConstraint();

            if ( this->parent() )
            {
                QRect r( QPoint(), size );
                r.moveCenter( QRect( QPoint(), this->parent()->size() ).center() );

                setGeometry( r );
            }

            if ( size.isValid() )
            {
                setFlags( flags() | Qt::MSWindowsFixedSizeDialogHint );
                setFixedSize( size );
            }

            setModality( Qt::ApplicationModal );
        }

        QskDialog::DialogCode exec()
        {
            return qskExec( this );
        }

        void setContentItem( QQuickItem* item )
        {
            QskDialogWindow::setDialogContentItem( item );
        }

        QQuickItem* rootItem()
        {
            return contentItem();
        }
    };

    template<>
    class WindowOrSubWindow< QskDialogSubWindow > : public QskDialogSubWindow
    {
      public:
        WindowOrSubWindow( QObject* parent, const QString& title,
            QskDialog::Actions actions, QskDialog::Action defaultAction )
            : QskDialogSubWindow( static_cast< QQuickWindow* >( parent )->contentItem() )
        {
            setPopupFlag( QskPopup::DeleteOnClose );
            setModal( true );
            setTitle( title );
            setDialogActions( actions );

            if ( actions != QskDialog::NoAction && defaultAction == QskDialog::NoAction )
                defaultAction = qskActionCandidate( buttonBox() );

            setDefaultDialogAction( defaultAction );
        }

        QskDialog::DialogCode exec()
        {
            return QskDialogSubWindow::exec();
        }

        void setContentItem( QQuickItem* item )
        {
            QskDialogSubWindow::setContentItem( item );
        }

        QQuickItem* rootItem()
        {
            return this;
        }
    };

    template< typename W >
    class FileSelectionWindow : public WindowOrSubWindow< W >
    {
        using Inherited = WindowOrSubWindow< W >;

      public:
        FileSelectionWindow( QObject* parent, const QString& title,
            QskDialog::Actions actions, QskDialog::Action defaultAction,const QString& directory )
            : WindowOrSubWindow< W >( parent, title, actions, defaultAction )
            , m_model( new QFileSystemModel( parent ) )
        {
            m_model->setRootPath( directory );

            auto* outerBox = new QskLinearBox( Qt::Vertical );
            outerBox->setMargins( 20 );
            outerBox->setSpacing( 20 );
#if 1
            outerBox->setFixedSize( 500, 500 );
#endif
            setupHeader( outerBox );
            setupListBox( outerBox );

            Inherited::setContentItem( outerBox );
        }

        QString selectedFile() const
        {
            const auto index = m_model->index( m_model->rootPath() );
            return m_model->filePath( index );
        }

      private:
        void setupHeader( QQuickItem* parentItem )
        {
            m_scrollArea = new QskScrollArea( parentItem );
            m_scrollArea->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );
            m_scrollArea->setFlickableOrientations( Qt::Horizontal );

            m_headerBox = new QskLinearBox( Qt::Horizontal, m_scrollArea );
            m_scrollArea->setScrolledItem( m_headerBox );
        }

        static QStringList splitPath( const QString& path )
        {
            const auto cleanPath = QDir::cleanPath( path );

            QDir dir( cleanPath );

            QStringList result;

            do
            {
                if( dir != QDir::root() )
                {
                    result.prepend( dir.absolutePath() );
                }
            }
            while( dir.cdUp() );

            return result;
        }

        void updateHeader( const QString& path )
        {
            const auto dirPaths = splitPath( path );

            for( int i = 0; i < dirPaths.count(); ++i )
            {
                QskPushButton* b;

                if( m_breadcrumbsButtons.count() <= i )
                {
                    b = new QskPushButton( m_headerBox );
                    b->setStrutSizeHint( QskPushButton::Panel, { -1, -1 } );
                    m_breadcrumbsButtons.append( b );
                }
                else
                {
                    b = m_breadcrumbsButtons.at( i );
                    b->disconnect();
                }

                QFileInfo fi( dirPaths.at( i ) );
                b->setText( fi.baseName() );

                QObject::connect( b, &QskPushButton::clicked, this, [this, fi]()
                {
                    m_model->setRootPath( fi.filePath() );
                });
            }

            for( int i = dirPaths.count(); i < m_breadcrumbsButtons.count(); i++ )
            {
                m_breadcrumbsButtons.at( i )->deleteLater();
                m_breadcrumbsButtons.removeAt( i );
            }

            m_scrollArea->ensureItemVisible( m_breadcrumbsButtons.last() );
        }

        void setupListBox( QQuickItem* parentItem )
        {
            m_listBox = new QskSimpleListBox( parentItem );

            QObject::connect( m_model, &QFileSystemModel::directoryLoaded,
                this, &FileSelectionWindow< W >::loadContents );

            QObject::connect( m_model, &QFileSystemModel::rootPathChanged,
                this, &FileSelectionWindow< W >::loadContents );

            QObject::connect( m_listBox, &QskSimpleListBox::selectedEntryChanged,
                this, [this]( const QString& path )
            {
                QFileInfo fi( m_model->rootPath(), path );
                m_model->setRootPath( fi.absoluteFilePath() );
            });
        }

        void loadContents()
        {
            m_listBox->removeBulk( 0 );

            const auto index = m_model->index( m_model->rootPath() );

            for ( int row = 0; row < m_model->rowCount( index ); row++ )
            {
                auto idx = m_model->index( row, 0, index );
                m_listBox->append( m_model->fileName( idx ) );
            }

            updateHeader( QDir( selectedFile() ).path() );
        }

        QFileSystemModel* const m_model;

        QskSimpleListBox* m_listBox;
        QskLinearBox* m_headerBox;
        QskScrollArea* m_scrollArea;
        QVector< QskPushButton* > m_breadcrumbsButtons;
    };
}

static QQuickWindow* qskSomeQuickWindow()
{
    // not the best code ever, but as it is a fallback only
    // maybe we should also add the stacking order

    const auto windows = QGuiApplication::topLevelWindows();
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
    subWindow->setPopupFlag( QskPopup::DeleteOnClose );
    subWindow->setModal( true );
#if 0
    subWindow->setWindowTitle( ... );
#endif
    subWindow->setTitle( title );
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
    qDebug() << "sc:" << size;

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
    const QString& text, uint priority, QskDialog::Actions actions,
    QskDialog::Action defaultAction )
{
    auto subWindow = new QskMessageSubWindow( window->contentItem() );
    subWindow->setPriority( priority );
    subWindow->setText( text );

    qskSetupSubWindow( title, actions, defaultAction, subWindow );
    ( void ) subWindow->exec();

    auto clickedAction = subWindow->clickedAction();
    if ( clickedAction == QskDialog::NoAction )
    {
        // dialog might have been closed by the window menu
        clickedAction = QskDialog::Cancel;
    }

    return clickedAction;
}

static QskDialog::Action qskMessageWindow(
    QWindow* transientParent, const QString& title,
    const QString& text, uint priority, QskDialog::Actions actions,
    QskDialog::Action defaultAction )
{
    Q_UNUSED( priority ); // can we do something with it ?

    QskMessageWindow messageWindow;
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
    QQuickWindow* window, const QString& title,
    QskDialog::Actions actions, QskDialog::Action defaultAction,
    const QStringList& entries, int selectedRow )
{
    auto subWindow = new QskSelectionSubWindow( window->contentItem() );
    subWindow->setEntries( entries );
    subWindow->setSelectedRow( selectedRow );

    QString selectedEntry;

    qskSetupSubWindow( title, actions, defaultAction, subWindow );
    if ( subWindow->exec() == QskDialog::Accepted )
        selectedEntry = subWindow->selectedEntry();

    return selectedEntry;
}

static QString qskSelectWindow(
    QWindow* transientParent, const QString& title,
    QskDialog::Actions actions, QskDialog::Action defaultAction,
    const QStringList& entries, int selectedRow )
{
    QskSelectionWindow window;
    window.setEntries( entries );
    window.setSelectedRow( selectedRow );

    QString selectedEntry = window.selectedEntry();

    qskSetupWindow( transientParent, title, actions, defaultAction, &window );
    if ( qskExec( &window ) == QskDialog::Accepted )
        selectedEntry = window.selectedEntry();

    return selectedEntry;
}

template< typename W >
static QString qskSelectFile( FileSelectionWindow< W >& window )
{
    QString selectedFile = window.selectedFile();

    if( window.exec() == QskDialog::Accepted )
        selectedFile = window.selectedFile();

    return selectedFile;
}

class QskDialog::PrivateData
{
  public:
    QPointer< QWindow > transientParent;
    QskDialog::Policy policy = QskDialog::TopLevelWindow;
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
    static QskDialog instance;
    return &instance;
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
    const QString& title, const QString& text, uint priority,
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
                title, text, priority, actions, defaultAction );
        }
    }

    return qskMessageWindow( m_data->transientParent,
        title, text, priority, actions, defaultAction );
}

QskDialog::Action QskDialog::information(
    const QString& title, const QString& text,
    Actions actions, Action defaultAction ) const
{
    return QskDialog::message( title, text, 0, actions, defaultAction );
}

QskDialog::Action QskDialog::question(
    const QString& title, const QString& text,
    Actions actions, Action defaultAction ) const
{
    return QskDialog::message( title, text, 0, actions, defaultAction );
}

QString QskDialog::select( const QString& title,
    const QStringList& entries, int selectedRow ) const
{
#if 1
    // should be parameters
    const auto actions = QskDialog::Ok | QskDialog::Cancel;
    const auto defaultAction = QskDialog::Ok;
#endif

    if ( m_data->policy == EmbeddedBox )
    {
        auto quickWindow = qobject_cast< QQuickWindow* >( m_data->transientParent );

        if ( quickWindow == nullptr )
            quickWindow = qskSomeQuickWindow();

        if ( quickWindow )
        {
            return qskSelectSubWindow( quickWindow,
                title, actions, defaultAction, entries, selectedRow );
        }
    }

    return qskSelectWindow( m_data->transientParent, title,
        actions, defaultAction, entries, selectedRow );

}

QString QskDialog::selectFile(
    const QString& title, const QString& directory ) const
{
#if 1
    // should be parameters
    const auto actions = QskDialog::Ok | QskDialog::Cancel;
    const auto defaultAction = QskDialog::Ok;
#endif

    if ( m_data->policy == EmbeddedBox )
    {
        auto quickWindow = qobject_cast< QQuickWindow* >( m_data->transientParent );

        if ( quickWindow == nullptr )
            quickWindow = qskSomeQuickWindow();

        if ( quickWindow )
        {
            FileSelectionWindow< QskDialogSubWindow > window( quickWindow, title, actions, defaultAction, directory );
            return qskSelectFile< QskDialogSubWindow >( window );
        }
    }

    FileSelectionWindow< QskDialogWindow > window( m_data->transientParent, title, actions, defaultAction, directory );
    return qskSelectFile< QskDialogWindow >( window );
}

QskDialog::ActionRole QskDialog::actionRole( Action action )
{
    using Q = QPlatformDialogHelper;

    const auto role = Q::buttonRole( static_cast< Q::StandardButton >( action ) );
    return static_cast< ActionRole >( role );
}

#include "moc_QskDialog.cpp"
