/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GridQuick.h"
#include <QQuickItem>
#include <QtQml>

static QQuickItem* createQml( const char* qmlCode )
{
    QQmlEngine engine( nullptr );

    QQmlComponent component( &engine );
    component.setData( qmlCode, QUrl() );

    return qobject_cast< QQuickItem* >( component.create() );
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
    const auto children = grid->childItems();

    if ( ( index >= 0 ) && ( index < children.count() ) )
        return attachedProperties( children.at( index ) );

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
}

GridQuick::~GridQuick()
{
}

void GridQuick::insert( const QByteArray& colorName,
    int row, int column, int rowSpan, int columnSpan )
{
    auto rectangle = createQml( "import QtQuick 2.0\nRectangle {}" );
    rectangle->setProperty( "color", QColor( colorName.constData() ) );
    rectangle->setParent( m_grid );
    rectangle->setParentItem( m_grid );

    rectangle->setImplicitWidth( 50 );
    rectangle->setImplicitHeight( 50 );

    auto props = attachedProperties( rectangle );
    props->setProperty( "row", row );
    props->setProperty( "column", column );
    props->setProperty( "rowSpan", rowSpan );
    props->setProperty( "columnSpan", columnSpan );
    props->setProperty( "fillWidth", true );
    props->setProperty( "fillHeight", true );
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
                    props->setProperty( "maximumWidth", size );

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

void GridQuick::resizeEvent( QResizeEvent* event )
{
    QQuickWidget::resizeEvent( event );

    const qreal margin = 10.0;

    m_grid->setX( margin );
    m_grid->setY( margin );
    m_grid->setWidth( width() - 2 * margin );
    m_grid->setHeight( height() - 2 * margin );
}

