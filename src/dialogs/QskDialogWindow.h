/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_DIALOG_WINDOW_H
#define QSK_DIALOG_WINDOW_H 1

#include "QskGlobal.h"
#include "QskWindow.h"
#include "QskDialog.h"
#include <memory>

class QSK_EXPORT QskDialogWindow : public QskWindow
{
    Q_OBJECT

    using Inherited = QskWindow;

public:
    QskDialogWindow( QWindow* parent = nullptr );
    ~QskDialogWindow() override;

    Q_INVOKABLE QskDialog::DialogCode result() const;
    Q_INVOKABLE QskDialog::DialogCode exec();

Q_SIGNALS:
    void finished( QskDialog::DialogCode result );
    void accepted();
    void rejected();

public Q_SLOTS:
    void done( QskDialog::DialogCode );
    void accept();
    void reject();

protected:
    void setResult( QskDialog::DialogCode r );

    bool event( QEvent* ) override;
    void keyPressEvent( QKeyEvent* ) override;

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
