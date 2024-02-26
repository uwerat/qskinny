/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskModelObjectBinder.h"

#include <qmetaobject.h>
#include <qabstractitemmodel.h>
#include <qpointer.h>

static inline QMetaProperty qskMetaProperty(
    const QMetaObject* metaObject, const QByteArray& name )
{
    if ( name.isEmpty() )
        return metaObject->userProperty();

    const auto idx = metaObject->indexOfProperty( name );
    return ( idx >= 0 ) ? metaObject->property( idx ) : QMetaProperty();
}

static void qskEnableConnections( QObject* object,
    QskModelObjectBinder* binder, bool on )
{
    if ( on )
    {
        QObject::connect( object, &QObject::destroyed,
            binder, &QskModelObjectBinder::unbindObject );
    }
    else
    {
        QObject::disconnect( object, &QObject::destroyed,
            binder, &QskModelObjectBinder::unbindObject );
    }
}

namespace
{
    struct Binding
    {
        Binding( int column, const QMetaProperty& property )
            : property( property )
            , column( column )
        {
        }

        QMetaProperty property;
        int column;
    };
}

class QskModelObjectBinder::PrivateData
{
  public:
    void updateProperties()
    {
        if ( model && currentRowIndex.isValid() )
        {
            for ( auto it = bindings.constBegin(); it != bindings.constEnd(); ++it )
                updateObjectProperty( it.key(), it.value() );
        }
    }

    void updateProperties( const QModelIndex& topLeft,
        const QModelIndex& bottomRight, const QVector< int >& roles )
    {
        if ( !( model && currentRowIndex.isValid() ) )
            return;

        if ( !( roles.isEmpty() || roles.contains( Qt::EditRole ) ) )
            return;

        const int row = currentRowIndex.row();

        if ( topLeft.row() <= row && row <= bottomRight.row() )
        {
            for ( auto it = bindings.constBegin(); it != bindings.constEnd(); ++it )
            {
                const int col = it.value().column;

                if( topLeft.column() <= col && col <= bottomRight.column() )
                    updateObjectProperty( it.key(), it.value() );
            }
        }
    }

    void updateObjectProperty( QObject* object, const Binding& binding )
    {
        const auto index = model->index( currentRowIndex.row(), binding.column );
        binding.property.write( object, index.data( Qt::EditRole ) );
    }

  public:
    QMap< QObject*, Binding > bindings;
    QPointer< QAbstractItemModel > model;
    QPersistentModelIndex currentRowIndex;
};

QskModelObjectBinder::QskModelObjectBinder( QObject* parent )
    : QObject( parent )
    , m_data( new PrivateData() )
{
}

QskModelObjectBinder::QskModelObjectBinder( QAbstractItemModel* model, QObject* parent )
    : QskModelObjectBinder( parent )
{
    setModel( model );
}

void QskModelObjectBinder::bindObject(
    QObject* object, int column, const QByteArray& propertyName )
{
    if( object == nullptr )
        return;

    // does not work with dynamic properties ...

    const auto metaProperty = qskMetaProperty( object->metaObject(), propertyName );
    Q_ASSERT( metaProperty.isValid() );

    if ( metaProperty.isValid() )
    {
        const Binding binding = { column, metaProperty };

        m_data->bindings.insert( object, binding );
        qskEnableConnections( object, this, true );

        if ( m_data->model && m_data->currentRowIndex.isValid() )
            m_data->updateObjectProperty( object, binding );
    }
}

void QskModelObjectBinder::unbindObject( QObject* object )
{
    auto it = m_data->bindings.find( object );
    if ( it != m_data->bindings.end() )
    {
        qskEnableConnections( object, this, false );
        m_data->bindings.erase( it );
    }
}

void QskModelObjectBinder::clearBindings()
{
    auto& bindings = m_data->bindings;

    for ( auto it = bindings.constBegin(); it != bindings.constEnd(); ++it )
        qskEnableConnections( it.key(), this, false );

    bindings.clear();
}

void QskModelObjectBinder::setModel( QAbstractItemModel* model )
{
    if ( model == m_data->model )
        return;

    if ( m_data->model )
    {
        disconnect( m_data->model, &QAbstractItemModel::dataChanged, this, nullptr );

        m_data->model = nullptr;
        m_data->currentRowIndex = QModelIndex();

        clearBindings();
    }

    m_data->model = model;

    if( model )
    {
        auto updateProperties = [this](
            const QModelIndex& topLeft, const QModelIndex& bottomRight,
            const QVector< int >& roles )
        {
            m_data->updateProperties( topLeft, bottomRight, roles );
        };

        connect( m_data->model, &QAbstractItemModel::dataChanged,
            this, updateProperties );

        connect( m_data->model, &QObject::destroyed,
            this, &QskModelObjectBinder::clearBindings );

        setCurrentRow( 0 );
    }
}

const QAbstractItemModel* QskModelObjectBinder::model() const
{
    return m_data->model;
}

QAbstractItemModel* QskModelObjectBinder::model()
{
    return m_data->model;
}

void QskModelObjectBinder::setCurrentRow( int row )
{
    auto model = m_data->model.data();
    auto& bindings = m_data->bindings;

    Q_ASSERT( model != nullptr );

    if ( model && row >= 0 && row < model->rowCount() )
    {
        m_data->currentRowIndex = model->index( row, 0 );

        for ( auto it = bindings.constBegin(); it != bindings.constEnd(); ++it )
            m_data->updateObjectProperty( it.key(), it.value() );

        Q_EMIT currentRowChanged( row );
    }
}

int QskModelObjectBinder::currentRow() const
{
    return m_data->currentRowIndex.row();
}

void QskModelObjectBinder::submit()
{
    if ( auto model = m_data->model )
    {
        const auto& bindings = m_data->bindings;

        for ( auto it = bindings.begin(); it != bindings.end(); ++it )
        {
            const auto value = it.value().property.read( it.key() );

            const auto row = m_data->currentRowIndex.row();
            const auto index = model->index( row, it.value().column );

            model->setData( index, value );
        }
    }
}

void QskModelObjectBinder::revert()
{
    m_data->updateProperties();
}

#include "moc_QskModelObjectBinder.cpp"
