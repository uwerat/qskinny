/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_DIALOG_SUB_WINDOW_H
#define QSK_DIALOG_SUB_WINDOW_H 1

#include "QskDialog.h"
#include "QskSubWindow.h"

class QskDialogButtonBox;
class QskPushButton;

class QSK_EXPORT QskDialogSubWindow : public QskSubWindow
{
    Q_OBJECT

    Q_PROPERTY( QskDialog::Actions dialogActions
        READ dialogActions WRITE setDialogActions )

    using Inherited = QskSubWindow;

  public:
    QskDialogSubWindow( QQuickItem* parent = nullptr );
    ~QskDialogSubWindow() override;

    QskDialog::Actions dialogActions() const;
    void setDialogActions( QskDialog::Actions );

    void addDialogAction( QskDialog::Action );
    void addDialogButton( QskPushButton*, QskDialog::ActionRole );

    Q_INVOKABLE QskDialog::Action clickedAction() const;

    Q_INVOKABLE QskDialog::DialogCode result() const;
    Q_INVOKABLE QskDialog::DialogCode exec();

    void setDefaultDialogAction( QskDialog::Action );

    void setDefaultButton( QskPushButton* );
    QskPushButton* defaultButton() const;

    QskDialogButtonBox* buttonBox();
    const QskDialogButtonBox* buttonBox() const;

    void setContentItem( QQuickItem* );
    QQuickItem* contentItem() const;

    // padding around the contentItem
    void setContentPadding( const QMarginsF& );
    QMarginsF contentPadding() const;

  Q_SIGNALS:
    void finished( QskDialog::DialogCode );
    void accepted();
    void rejected();

  public Q_SLOTS:
    void accept();
    void reject();

    virtual void done( QskDialog::DialogCode );

  protected:
    void setResult( QskDialog::DialogCode );
    void keyPressEvent( QKeyEvent* ) override;

    void updateLayout() override;
    void aboutToShow() override;

    QSizeF layoutSizeHint( Qt::SizeHint, const QSizeF& ) const override;

    virtual QskDialogButtonBox* createButtonBox();

  private:
    void initButtonBox();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
