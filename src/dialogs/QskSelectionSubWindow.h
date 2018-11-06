/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SELECTION_SUB_WINDOW_H
#define QSK_SELECTION_SUB_WINDOW_H 1

#include "QskDialogSubWindow.h"

class QskTextOptions;

class QSK_EXPORT QskSelectionSubWindow : public QskDialogSubWindow
{
    Q_OBJECT

    Q_PROPERTY( QString infoText READ infoText
        WRITE setInfoText NOTIFY infoTextChanged )

    Q_PROPERTY( QskTextOptions infoTextOptions READ infoTextOptions
        WRITE setInfoTextOptions NOTIFY infoTextOptionsChanged )

    Q_PROPERTY( QStringList entries READ entries
        WRITE setEntries NOTIFY entriesChanged FINAL )

    Q_PROPERTY( int selectedRow READ selectedRow
        WRITE setSelectedRow NOTIFY selectedRowChanged FINAL )

    using Inherited = QskDialogSubWindow;

  public:
    QskSelectionSubWindow( QQuickItem* parent = nullptr );
    ~QskSelectionSubWindow() override;

    void setInfoTextOptions( const QskTextOptions& );
    QskTextOptions infoTextOptions() const;

    QString infoText() const;

    Q_INVOKABLE QStringList entries() const;
    Q_INVOKABLE int selectedRow() const;
    Q_INVOKABLE QString selectedEntry() const;

  public Q_SLOTS:
    void setInfoText( const QString& );
    void setEntries( const QStringList& );
    void setSelectedRow( int row );

  Q_SIGNALS:
    void infoTextChanged( const QString& );
    void infoTextOptionsChanged( const QskTextOptions& );
    void selectedRowChanged( int );
    void selectedEntryChanged( const QString& );
    void entriesChanged();

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
