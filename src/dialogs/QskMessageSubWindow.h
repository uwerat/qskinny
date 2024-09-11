/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_MESSAGE_SUB_WINDOW_H
#define QSK_MESSAGE_SUB_WINDOW_H

#include "QskDialogSubWindow.h"

class QskTextOptions;

class QSK_EXPORT QskMessageSubWindow : public QskDialogSubWindow
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text
        WRITE setText NOTIFY textChanged )

    Q_PROPERTY( QskTextOptions textOptions READ textOptions
        WRITE setTextOptions NOTIFY textOptionsChanged )

    using Inherited = QskDialogSubWindow;

  public:
    QskMessageSubWindow( QQuickItem* parent = nullptr );
    ~QskMessageSubWindow() override;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    QString text() const;

  public Q_SLOTS:
    void setText( const QString& );

  Q_SIGNALS:
    void textChanged( const QString& );
    void textOptionsChanged( const QskTextOptions& );
};

#endif
