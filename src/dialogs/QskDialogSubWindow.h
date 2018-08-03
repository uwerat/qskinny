/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_DIALOG_SUB_WINDOW_H
#define QSK_DIALOG_SUB_WINDOW_H 1

#include "QskDialog.h"
#include "QskSubWindow.h"

class QSK_EXPORT QskDialogSubWindow : public QskSubWindow
{
    Q_OBJECT

    using Inherited = QskSubWindow;

  public:
    QskDialogSubWindow( QQuickItem* parent = nullptr );
    ~QskDialogSubWindow() override;

    Q_INVOKABLE QskDialog::DialogCode result() const;
    Q_INVOKABLE QskDialog::DialogCode exec();

  Q_SIGNALS:
    void finished( QskDialog::DialogCode );
    void accepted();
    void rejected();

  public Q_SLOTS:
    void done( QskDialog::DialogCode );
    void accept();
    void reject();

  protected:
    void setResult( QskDialog::DialogCode r );
    void keyPressEvent( QKeyEvent* ) override;

    void aboutToShow() override;

  private:
    QskDialog::DialogCode m_result;
};

#endif
