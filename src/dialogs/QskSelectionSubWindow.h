/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SELECTION_SUB_WINDOW_H
#define QSK_SELECTION_SUB_WINDOW_H

#include "QskDialogSubWindow.h"

class QSK_EXPORT QskSelectionSubWindow : public QskDialogSubWindow
{
    Q_OBJECT

    Q_PROPERTY( QStringList entries READ entries
        WRITE setEntries NOTIFY entriesChanged FINAL )

    Q_PROPERTY( int selectedRow READ selectedRow
        WRITE setSelectedRow NOTIFY selectedRowChanged FINAL )

    using Inherited = QskDialogSubWindow;

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
