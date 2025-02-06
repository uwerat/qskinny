/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_DIALOG_SUB_WINDOW_H
#define QSK_DIALOG_SUB_WINDOW_H

#include "QskDialog.h"
#include "QskSubWindow.h"

class QskDialogButtonBox;
class QskPushButton;
class QskTextLabel;

class QSK_EXPORT QskDialogSubWindow : public QskSubWindow
{
    Q_OBJECT

    Q_PROPERTY( QskDialog::Actions dialogActions
        READ dialogActions WRITE setDialogActions )

    Q_PROPERTY( QString title READ title
        WRITE setTitle NOTIFY titleChanged )

    using Inherited = QskSubWindow;

  public:
    QSK_SUBCONTROLS( DialogTitle )

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

    void setTitle( const QString& );
    QString title() const;

    QskTextLabel* titleLabel();
    const QskTextLabel* titleLabel() const;

    // padding around the contentItem
    void setContentPadding( const QMarginsF& );
    QMarginsF contentPadding() const;

  Q_SIGNALS:
    void titleChanged( const QString& );

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

    virtual void updateGeometry();

    void updateLayout() override;

    QSizeF layoutSizeHint( Qt::SizeHint, const QSizeF& ) const override;

  private:
    void initButtonBox();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
