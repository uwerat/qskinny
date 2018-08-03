/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LIST_BOX_H
#define QSK_LIST_BOX_H

#include "QskListView.h"
#include <qstringlist.h>

class QSK_EXPORT QskSimpleListBox : public QskListView
{
    Q_OBJECT

    Q_PROPERTY( QStringList entries READ entries
        WRITE setEntries NOTIFY entriesChanged )

    using Inherited = QskListView;

  public:
    QskSimpleListBox( QQuickItem* parent = nullptr );
    ~QskSimpleListBox() override;

    void setColumnWidthHint( int column, qreal width );
    qreal columnWidthHint( int column ) const;

    // not sure if we will keep this, but
    // at least it is worth trying to avoid models

    void insert( const QStringList&, int index );
    void insert( const QString&, int index );

    void append( const QStringList& );
    void append( const QString& );

    void removeAt( int index );
    void removeBulk( int from, int to = -1 );

    int rowCount() const override final;
    int columnCount() const override final;

    qreal columnWidth( int col ) const override;
    qreal rowHeight() const override;

    QString entryAt( int row ) const;
    QString selectedEntry() const;
    QStringList entries() const;

    QVariant valueAt( int row, int col ) const override final;

  public Q_SLOTS:
    void setEntries( const QStringList& );
    void clear();

  Q_SIGNALS:
    void entriesChanged();
    void selectedEntryChanged( const QString& );

  private:
    void propagateEntries();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline void QskSimpleListBox::append( const QStringList& entries )
{
    insert( entries, -1 );
}

inline void QskSimpleListBox::append( const QString& entry )
{
    insert( entry, -1 );
}

inline QString QskSimpleListBox::selectedEntry() const
{
    return entryAt( selectedRow() );
}

#endif
