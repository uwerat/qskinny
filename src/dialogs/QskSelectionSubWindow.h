/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SELECTION_SUB_WINDOW_H
#define QSK_SELECTION_SUB_WINDOW_H 1

#include "QskInputSubWindow.h"

class QskSelectionDialogBox;

class QSK_EXPORT QskSelectionSubWindow : public QskInputSubWindow
{
    Q_OBJECT

    Q_PROPERTY( QStringList entries READ entries
        WRITE setEntries NOTIFY entriesChanged FINAL )

    Q_PROPERTY( int selectedRow READ selectedRow
        WRITE setSelectedRow NOTIFY selectedRowChanged FINAL )

    using Inherited = QskInputSubWindow;

  public:
    QskSelectionSubWindow( QQuickItem* parent = nullptr );
    ~QskSelectionSubWindow() override;

    Q_INVOKABLE QStringList entries() const;
    Q_INVOKABLE int selectedRow() const;
    Q_INVOKABLE QString selectedEntry() const;

  public Q_SLOTS:
    void setEntries( const QStringList& );
    void setSelectedRow( int row );

  Q_SIGNALS:
    void selectedRowChanged( int );
    void selectedEntryChanged( const QString& );
    void entriesChanged();
};

#endif
