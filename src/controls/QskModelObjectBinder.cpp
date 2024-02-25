/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskModelObjectBinder.h"

#include <qmetaobject.h>
#include <qabstractitemmodel.h>
#include <qpointer.h>
#include <qassert.h>

static inline QMetaProperty qskMetaProperty(
    const QMetaObject* metaObject, const QByteArray& name )
{
    if ( name.isEmpty() )
        return metaObject->userProperty();

    const auto idx = metaObject->indexOfProperty( name );
    return ( idx >= 0 ) ? metaObject->property( idx ) : QMetaProperty();
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
    void updateProperties(const QModelIndex& topLeft,
        const QModelIndex& bottomRight, const QVector< int >& roles )
    {
        if ( !roles.contains( Qt::EditRole ) )
            return;

        const int row = currentRowIndex.row();

        if ( topLeft.row() <= row && row <= bottomRight.row() )
        {
            for ( auto it = bindings.begin(); it != bindings.end(); ++it )
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
    bindModel( model );
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
        m_data->bindings.insert( object, { column, metaProperty } );
        connect( object, &QObject::destroyed,
            this, &QskModelObjectBinder::unbindObject );
    }
}

void QskModelObjectBinder::unbindObject( QObject* object )
{
    auto it = m_data->bindings.find( object );
    if ( it != m_data->bindings.end() )
    {
        disconnect( object, &QObject::destroyed,
            this, &QskModelObjectBinder::unbindObject );

        m_data->bindings.erase( it );
    }
}

void QskModelObjectBinder::bindModel( QAbstractItemModel* model )
{
    if ( m_data->model )
        disconnect( m_data->model, nullptr, this, nullptr );

    m_data->model = model;
    m_data->currentRowIndex = QModelIndex();

    if( model )
    {
        auto updateProperties = [this](
            const QModelIndex& topLeft, const QModelIndex& bottomRight,
            const QVector< int >& roles )
        {
            m_data->updateProperties( topLeft, bottomRight, roles  );
        };

        connect( m_data->model, &QAbstractItemModel::dataChanged,
            this, updateProperties );
    }
}

void QskModelObjectBinder::setCurrentRow( int row )
{
    Q_ASSERT( m_data->model != nullptr );

    if ( m_data->model == nullptr )
        return;

    m_data->currentRowIndex = m_data->model->index( row, 0 );

    for ( auto it = m_data->bindings.begin(); it != m_data->bindings.end(); ++it )
        m_data->updateObjectProperty( it.key(), it.value() );
}

int QskModelObjectBinder::currentRow() const
{
    return m_data->currentRowIndex.row();
}

void QskModelObjectBinder::updateModel()
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

#include "moc_QskModelObjectBinder.cpp"
