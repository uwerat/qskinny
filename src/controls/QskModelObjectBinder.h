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

class QSK_EXPORT QskModelObjectBinder : public QObject
{
    Q_OBJECT

  public:
    QskModelObjectBinder( QObject* parent = nullptr );
    QskModelObjectBinder( QAbstractItemModel*, QObject* parent = nullptr );

    void bindObject( QObject*, int column,
        const QByteArray& propertyName = QByteArray() );

    void unbindObject( QObject* );
    void bindModel( QAbstractItemModel* );

    void setCurrentRow( int row );
    [[nodiscard]] int currentRow() const;

    void updateModel();

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
