/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GridQuick.h"
#include <QQuickItem>
#include <QtQml>
#include <QDebug>

static QQuickItem* createQml( const char* qmlCode )
{
    QQmlEngine engine( nullptr );

    QQmlComponent component( &engine );
    component.setData( qmlCode, QUrl() );

    if ( component.status() != QQmlComponent::Ready )
        qWarning() << component.errorString();

    return qobject_cast< QQuickItem* >( component.create() );
}

static QQuickItem* itemAt( const QQuickItem* grid, int index )
{
    const auto children = grid->childItems();
    if ( ( index >= 0 ) && ( index < children.count() ) )
        return children.at( index );

    return nullptr;
}

static QObject* attachedProperties( const QQuickItem* item )
{
    for ( auto child : item->children() )
    {
        if ( child->inherits( "QQuickLayoutAttached" ) )
            return child;
    }

    return nullptr;
}

static QObject* attachedPropertiesAt( const QQuickItem* grid, int index )
{
    if ( auto item = itemAt( grid, index ) )
        return attachedProperties( item );

    return nullptr;
}

GridQuick::GridQuick( QWidget* parent )
    : QQuickWidget( parent )
{
    setContentsMargins( QMargins() );
    setResizeMode( QQuickWidget::SizeRootObjectToView );

    auto contentItem =
        createQml( "import QtQuick 2.0\nimport QtQuick.Layouts 1.1\nItem { GridLayout {} }" );
    setContent( QUrl(), nullptr, contentItem );

    m_grid = contentItem->childItems().first();
    m_grid->setProperty( "rowSpacing", 5 );
    m_grid->setProperty( "columnSpacing", 5 );
}

GridQuick::~GridQuick()
{
}

void GridQuick::insert( const QByteArray& colorName,
    int row, int column, int rowSpan, int columnSpan )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 15, 0 )
    /*
        We need to create a temporary layout in QML, so that the
        object for the attachedProperties is created early
     */
    auto layout = createQml( "import QtQuick 2.0\nimport QtQuick.Layouts 1.15\nGridLayout { Rectangle {} }" );

    auto rectangle = layout->childItems().first();
    rectangle->setParent( nullptr );

    delete layout;
#else
    auto rectangle = createQml( "import QtQuick 2.0\nRectangle {}" );
#endif

    rectangle->setParent( m_grid );
    rectangle->setParentItem( m_grid );

    rectangle->setImplicitWidth( 50 );
    rectangle->setImplicitHeight( 50 );
    rectangle->setProperty( "color", QColor( colorName.constData() ) );

    if ( auto props = attachedProperties( rectangle ) )
    {
        props->setProperty( "row", row );
        props->setProperty( "column", column );
        props->setProperty( "rowSpan", rowSpan );
        props->setProperty( "columnSpan", columnSpan );
        props->setProperty( "fillWidth", true );
        props->setProperty( "fillHeight", true );
    }
}

void GridQuick::setSpacing( Qt::Orientations orientations, int spacing )
{
    if ( orientations & Qt::Vertical )
        m_grid->setProperty( "rowSpacing", spacing );

    if ( orientations & Qt::Horizontal )
        m_grid->setProperty( "columnSpacing", spacing );
}

void GridQuick::setSizeHint( int, Qt::Orientation, Qt::SizeHint, int )
{
    qWarning() << "setSizeHint is not supported by Quick Layouts.";
}

void GridQuick::setStretchFactor( int, Qt::Orientation, int )
{
    qWarning() << "setStretchFactor is not supported by Quick Layouts.";
}

void GridQuick::setSizeHintAt( int index, Qt::Orientation orientation,
    Qt::SizeHint which, int hint )
{
    if ( auto props = attachedPropertiesAt( m_grid, index ) )
    {
        const qreal size = hint;

        switch( static_cast< int >( which ) )
        {
            case Qt::MinimumSize:
            {
                if ( orientation == Qt::Horizontal )
                    props->setProperty( "minimumWidth", size );
                else
                    props->setProperty( "minimumHeight", size );

                break;
            }
            case Qt::PreferredSize:
            {
                if ( orientation == Qt::Horizontal )
                    props->setProperty( "preferredWidth", size );
                else
                    props->setProperty( "preferredHeight", size );

                break;
            }
            case Qt::MaximumSize:
            {
                if ( orientation == Qt::Horizontal )
                    props->setProperty( "maximumWidth", size );
                else
                    props->setProperty( "maximumHeight", size );

                break;
            }
        }
    }
}

void GridQuick::setSizePolicyAt(
    int index, Qt::Orientation orientation, int policy )
{
    const auto qPolicy = static_cast< QSizePolicy::Policy >( policy & 0xF );

#if 0
    const bool isConstrained = policy & ( 1 << 4 );
#endif

    const bool doFill = ( qPolicy & QSizePolicy::GrowFlag )
        || ( qPolicy & QSizePolicy::ExpandFlag );

    if ( auto props = attachedPropertiesAt( m_grid, index ) )
    {
        if ( orientation == Qt::Horizontal )
            props->setProperty( "fillWidth", doFill );
        else
            props->setProperty( "fillHeight", doFill );
    }
}

void GridQuick::setAlignmentAt( int index, Qt::Alignment alignment )
{
    if ( auto props = attachedPropertiesAt( m_grid, index ) )
        props->setProperty( "alignment", QVariant::fromValue( alignment ) );
}

void GridQuick::setRetainSizeWhenHiddenAt( int, bool )
{
    qWarning() << "setRetainSizeWhenHidden is not supported by Quick Layouts.";
}

void GridQuick::setVisibleAt( int index, bool on )
{
    if ( auto item = itemAt( m_grid, index ) )
        item->setVisible( on );
}

static const qreal margin = 10.0;

QSize GridQuick::preferredSize() const
{
    return QSize(
        m_grid->implicitWidth() + 2 * margin,
        m_grid->implicitHeight() + 2 * margin );
}

void GridQuick::resizeEvent( QResizeEvent* event )
{
    QQuickWidget::resizeEvent( event );

    m_grid->setX( margin );
    m_grid->setY( margin );
    m_grid->setWidth( width() - 2 * margin );
    m_grid->setHeight( height() - 2 * margin );
}

