/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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
    QSK_SUBCONTROLS( Panel, Text, Graphic )

    QskDialogButton( QskDialog::Action, QQuickItem* parent = nullptr );
    QskDialogButton( QQuickItem* parent = nullptr );

    ~QskDialogButton() override;

    void setAction( QskDialog::Action );
    QskDialog::Action action() const;

    QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol ) const override;

  Q_SIGNALS:
    void actionChanged();

  protected:
    void changeEvent( QEvent* ) override;

  private:
    QskDialog::Action m_action;
};

#endif
