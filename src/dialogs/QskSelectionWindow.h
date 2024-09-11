/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SELECTION_WINDOW_H
#define QSK_SELECTION_WINDOW_H

#include "QskDialogWindow.h"

class QSK_EXPORT QskSelectionWindow : public QskDialogWindow
{
    Q_OBJECT

    Q_PROPERTY( QStringList entries READ entries
        WRITE setEntries NOTIFY entriesChanged FINAL )

    Q_PROPERTY( int selectedRow READ selectedRow
        WRITE setSelectedRow NOTIFY selectedRowChanged FINAL )

    using Inherited = QskDialogWindow;

  public:
    QskSelectionWindow( QWindow* parent = nullptr );
    ~QskSelectionWindow() override;

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
