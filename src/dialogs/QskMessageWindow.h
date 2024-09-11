/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_MESSAGE_WINDOW_H
#define QSK_MESSAGE_WINDOW_H

#include "QskDialogWindow.h"

class QskTextOptions;

class QSK_EXPORT QskMessageWindow : public QskDialogWindow
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text
        WRITE setText NOTIFY textChanged )

    Q_PROPERTY( QskTextOptions textOptions READ textOptions
        WRITE setTextOptions NOTIFY textOptionsChanged )

    using Inherited = QskDialogWindow;

  public:
    QskMessageWindow( QWindow* parent = nullptr );
    ~QskMessageWindow() override;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    QString text() const;

  public Q_SLOTS:
    void setText( const QString& );

  Q_SIGNALS:
    void textChanged( const QString& );
    void textOptionsChanged( const QskTextOptions& );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
