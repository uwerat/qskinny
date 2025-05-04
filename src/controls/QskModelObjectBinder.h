/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_MODEL_OBJECT_BINDER_H
#define QSK_MODEL_OBJECT_BINDER_H

#include "QskGlobal.h"

#include <qobject.h>
#include <qbytearray.h>

#include <memory>

class QAbstractItemModel;
class QMetaProperty;

class QSK_EXPORT QskModelObjectBinder : public QObject
{
    Q_OBJECT

    Q_PROPERTY( int currentRow READ currentRow
        WRITE setCurrentRow NOTIFY currentRowChanged )

  public:
    QskModelObjectBinder( QObject* parent = nullptr );
    QskModelObjectBinder( QAbstractItemModel*, QObject* parent = nullptr );

    ~QskModelObjectBinder() override;

    void setModel( QAbstractItemModel* );

    const QAbstractItemModel* model() const;
    QAbstractItemModel* model();

    void setCurrentRow( int row );
    int currentRow() const;

    void bindObject( QObject*, int column,
        const QByteArray& propertyName = QByteArray() );

    void unbindObject( QObject* );

    QMetaProperty boundProperty( const QObject* ) const;
    QObjectList boundObjects() const;

  Q_SIGNALS:
    void currentRowChanged( int );

  public Q_SLOTS:
    void submit();
    void revert();
    void clearBindings();

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
