/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_DIALOG_BUTTON_H
#define QSK_DIALOG_BUTTON_H

#include "QskDialog.h"
#include "QskPushButton.h"

class QSK_EXPORT QskDialogButton : public QskPushButton
{
    Q_OBJECT

    Q_PROPERTY( QskDialog::Action action READ action()
        WRITE setAction NOTIFY actionChanged() FINAL )

    using Inherited = QskPushButton;

  public:

    QskDialogButton( QskDialog::Action, QQuickItem* parent = nullptr );
    QskDialogButton( QQuickItem* parent = nullptr );

    ~QskDialogButton() override;

    void setAction( QskDialog::Action );
    QskDialog::Action action() const;

  Q_SIGNALS:
    void actionChanged();

  protected:
    void changeEvent( QEvent* ) override;

  private:
    void resetButton();

    QskDialog::Action m_action;
};

#endif
