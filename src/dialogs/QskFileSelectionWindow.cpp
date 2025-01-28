/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFileSelectionWindow.h"

#include "QskEvent.h"
#include "QskFunctions.h"
#include "QskLinearBox.h"
#include "QskListView.h"
#include "QskScrollArea.h"
#include "QskPushButton.h"

#include <QFileSystemModel>

// copied from QskListView.cpp:
static inline int qskRowAt( const QskListView* listView, const QPointF& pos )
{
    const auto rect = listView->viewContentsRect();
    if ( rect.contains( pos ) )
    {
        const auto y = pos.y() - rect.top() + listView->scrollPos().y();

        const int row = y / listView->rowHeight();
        if ( row >= 0 && row < listView->rowCount() )
            return row;
    }

    return -1;
}

// copied from QskGestureRecognizer.cpp:
static QMouseEvent* qskClonedMouseEvent( const QMouseEvent* event )
{
    QMouseEvent* clonedEvent;

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    clonedEvent = QQuickWindowPrivate::cloneMouseEvent(
        const_cast< QMouseEvent* >( event ), nullptr );
#else
    clonedEvent = event->clone();
#endif
    clonedEvent->setAccepted( false );

    return clonedEvent;
}

namespace
{
    class FileSystemView : public QskListView
    {
        using Inherited = QskListView;

      public:
        FileSystemView( const QString& directory, QDir::Filters filters, QQuickItem* parent = nullptr )
            : QskListView( parent )
            , m_model( new QFileSystemModel( this ) )
        {
            const auto defaultWidth = 50;

            connect( m_model, &QFileSystemModel::directoryLoaded, this, [this]()
            {
                m_columnWidths.fill( defaultWidth );
                updateScrollableSize();
                setScrollPos( { 0, 0 } );
                setSelectedRow( -1 );
            });

            m_model->setFilter( filters );
            m_model->setRootPath( {} ); // invalidate to make sure to get an update
            m_model->setRootPath( directory );

            m_columnWidths.fill( defaultWidth, m_model->columnCount() );
        }

        virtual int rowCount() const override
        {
            const auto index = m_model->index( m_model->rootPath() );
            return m_model->rowCount( index );
        }

        virtual int columnCount() const override
        {
            const auto index = m_model->index( m_model->rootPath() );
            return m_model->columnCount( index );
        }

        virtual qreal columnWidth( int col ) const override
        {
            auto w = m_columnWidths.at( col );

            if( col == 0 )
            {
                w = qMax( 250, w ); // min width for the name
            }

            return w + 15; // spacing
        }

        virtual qreal rowHeight() const override
        {
            const auto hint = strutSizeHint( Cell );
            const auto padding = paddingHint( Cell );

            qreal h = effectiveFontHeight( Text );
            h += padding.top() + padding.bottom();

            return qMax( h, hint.height() );
        }

        virtual QVariant valueAt( int row, int col ) const override
        {
            const auto rootIndex = m_model->index( m_model->rootPath() );

            const auto index = m_model->index( row, col, rootIndex );
            const auto v = m_model->data( index );

            const auto w = qskHorizontalAdvance( effectiveFont( Text ), v.toString() );

            if( w > m_columnWidths.at( col ) )
            {
                m_columnWidths[ col ] = w;
            }

            return v;
        }

        QFileSystemModel* model()
        {
            return m_model;
        }

      protected:

        void mousePressEvent( QMouseEvent* event ) override
        {
            if( m_doubleClickEvent && m_doubleClickEvent->timestamp() == event->timestamp() )
            {
                // do not select rows from double click mouse events
                m_doubleClickEvent = nullptr;
            }
            else
            {
                Inherited::mousePressEvent( event );
            }
        }

        void mouseDoubleClickEvent( QMouseEvent* event ) override
        {
            m_doubleClickEvent = qskClonedMouseEvent( event );

            const int row = qskRowAt( this, qskMousePosition( event ) );
            const auto path = valueAt( row, 0 ).toString();

            QFileInfo fi( m_model->rootPath(), path );

            if( fi.isDir() )
            {
                m_model->setRootPath( fi.absoluteFilePath() );
            }

            Inherited::mouseDoubleClickEvent( event );
        }

      private:
        QFileSystemModel* const m_model;
        mutable QVector< int > m_columnWidths;
        QMouseEvent* m_doubleClickEvent = nullptr;
    };
}

template< typename W >
class QskFileSelectionWindow< W >::PrivateData
{
  public:
    QskScrollArea* headerScrollArea;
    QskLinearBox* headerBox;
    QVector< QskPushButton* > breadcrumbsButtons;

    FileSystemView* fileView;
};

template< typename W >
QskFileSelectionWindow< W >::QskFileSelectionWindow( QObject* parent, const QString& title,
    QskDialog::Actions actions, QskDialog::Action defaultAction,
    const QString& directory, QDir::Filters filters )
    : QskWindowOrSubWindow< W >( parent, title, actions, defaultAction )
    , m_data( new PrivateData )
{
    auto* outerBox = new QskLinearBox( Qt::Vertical );
    outerBox->setMargins( 20 );
    outerBox->setSpacing( 20 );
#if 1
    outerBox->setFixedSize( 700, 500 );
#endif
    setupHeader( outerBox );
    setupFileSystemView( directory, filters, outerBox );

    updateHeader( directory );

    Inherited::setContentItem( outerBox );
}

template< typename W >
QskFileSelectionWindow< W >::~QskFileSelectionWindow() = default;

template< typename W >
QString QskFileSelectionWindow< W >::selectedPath() const
{
    if( m_data->fileView->selectedRow() != -1 )
    {
        const auto path = m_data->fileView->valueAt( m_data->fileView->selectedRow(), 0 ).toString();
        QFileInfo fi( m_data->fileView->model()->rootPath(), path );
        return fi.absoluteFilePath();
    }

    return {};
}

template< typename W >
void QskFileSelectionWindow< W >::setupHeader( QQuickItem* parentItem )
{
    m_data->headerScrollArea = new QskScrollArea( parentItem );
    m_data->headerScrollArea->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );
    m_data->headerScrollArea->setFlickableOrientations( Qt::Horizontal );

    m_data->headerBox = new QskLinearBox( Qt::Horizontal, m_data->headerScrollArea );
    m_data->headerScrollArea->setScrolledItem( m_data->headerBox );
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

template< typename W >
void QskFileSelectionWindow< W >::updateHeader( const QString& path )
{
    const auto dirPaths = ::splitPath( path );

    for( int i = 0; i < dirPaths.count(); ++i )
    {
        QskPushButton* b;

        if( m_data->breadcrumbsButtons.count() <= i )
        {
            b = new QskPushButton( m_data->headerBox );
            b->setStrutSizeHint( QskPushButton::Panel, { -1, -1 } );
            m_data->breadcrumbsButtons.append( b );
        }
        else
        {
            b = m_data->breadcrumbsButtons.at( i );
            b->disconnect();
        }

        QFileInfo fi( dirPaths.at( i ) );
        b->setText( fi.baseName() );

        QObject::connect( b, &QskPushButton::clicked, this, [this, fi]()
        {
            m_data->fileView->model()->setRootPath( fi.filePath() );
        });
    }

    for( int i = dirPaths.count(); i < m_data->breadcrumbsButtons.count(); i++ )
    {
        m_data->breadcrumbsButtons.at( i )->deleteLater();
    }

    m_data->breadcrumbsButtons.remove( dirPaths.count(), m_data->breadcrumbsButtons.count() - dirPaths.count() );

    if( !m_data->breadcrumbsButtons.isEmpty() )
    {
        auto* b = m_data->breadcrumbsButtons.last();

        // button might just have been created and not be layed out yet:
        QObject::connect( b, &QskPushButton::widthChanged, this, [this, b]()
        {
            m_data->headerScrollArea->ensureItemVisible( b );
        } );
    }
}

template< typename W >
void QskFileSelectionWindow< W >::setupFileSystemView( const QString& directory, QDir::Filters filters, QQuickItem* parentItem )
{
    m_data->fileView = new FileSystemView( directory, filters, parentItem );

    QObject::connect( m_data->fileView->model(), &QFileSystemModel::rootPathChanged,
        this, &QskFileSelectionWindow< W >::updateHeader );

    QObject::connect( m_data->fileView, &QskListView::selectedRowChanged, this, [this]()
    {
        if( m_data->fileView->model()->filter() & QDir::Files )
        {
            QFileInfo fi( selectedPath() );
            W::defaultButton()->setEnabled( !fi.isDir() );
        }
    } );
}

template class QskFileSelectionWindow< QskDialogWindow >;
template class QskFileSelectionWindow< QskDialogSubWindow >;
