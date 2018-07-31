/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_DIALOG_BUTTON_H
#define QSK_DIALOG_BUTTON_H

#include "QskGlobal.h"
#include "QskPushButton.h"
#include "QskDialog.h"

class QSK_EXPORT QskDialogButton : public QskPushButton
{
    Q_OBJECT

    Q_PROPERTY( QskDialog::StandardButton standardButton READ standardButton()
        WRITE setStandardButton NOTIFY standardButtonChanged() FINAL )

    using Inherited = QskPushButton;

public:
    QSK_SUBCONTROLS( Panel, Text, Graphic )

    QskDialogButton( QskDialog::StandardButton, QQuickItem* parent = nullptr );
    QskDialogButton( QQuickItem* parent = nullptr );

    ~QskDialogButton() override;

    void setStandardButton( QskDialog::StandardButton );
    QskDialog::StandardButton standardButton() const;

    QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol ) const override;

Q_SIGNALS:
    void standardButtonChanged();

protected:
    void changeEvent( QEvent* ) override;

private:
    QskDialog::StandardButton m_buttonType;
};

#endif
