/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_DIALOG_WINDOW_H
#define QSK_DIALOG_WINDOW_H 1

#include "QskDialog.h"
#include "QskWindow.h"

class QskDialogButtonBox;
class QskPushButton;

class QSK_EXPORT QskDialogWindow : public QskWindow
{
    Q_OBJECT

    Q_PROPERTY( QskDialog::Actions dialogActions
        READ dialogActions WRITE setDialogActions )

    using Inherited = QskWindow;

  public:
    QskDialogWindow( QWindow* parent = nullptr );
    ~QskDialogWindow() override;

    QskDialog::Actions dialogActions() const;
    void setDialogActions( QskDialog::Actions );

    Q_INVOKABLE QskDialog::Action clickedAction() const;

    Q_INVOKABLE QskDialog::DialogCode result() const;
    Q_INVOKABLE QskDialog::DialogCode exec();

    void setDefaultDialogAction( QskDialog::Action );

    void setDefaultButton( QskPushButton* );
    QskPushButton* defaultButton() const;

    QskDialogButtonBox* buttonBox();
    const QskDialogButtonBox* buttonBox() const;

    void setDialogContentItem( QQuickItem* );
    QQuickItem* dialogContentItem() const;

  Q_SIGNALS:
    void finished( QskDialog::DialogCode result );
    void accepted();
    void rejected();

  public Q_SLOTS:
    void accept();
    void reject();

    virtual void done( QskDialog::DialogCode );

  protected:
    void setResult( QskDialog::DialogCode r );
    virtual QskDialogButtonBox* createButtonBox();

    bool event( QEvent* ) override;
    void keyPressEvent( QKeyEvent* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
