/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_DIALOG_BUTTON_BOX_H
#define QSK_DIALOG_BUTTON_BOX_H

#include "QskBox.h"
#include "QskDialog.h"

class QskPushButton;
template <typename T> class QVector;

class QSK_EXPORT QskDialogButtonBox : public QskBox
{
    Q_OBJECT

    Q_PROPERTY( Qt::Orientation orientation READ orientation
        WRITE setOrientation NOTIFY orientationChanged )

    Q_PROPERTY( bool centeredButtons READ centeredButtons
        WRITE setCenteredButtons NOTIFY centeredButtonsChanged )

    using Inherited = QskBox;

  public:
    QSK_SUBCONTROLS( Panel )

    QskDialogButtonBox( QQuickItem* parent = nullptr );
    QskDialogButtonBox( Qt::Orientation orientation, QQuickItem* parent = nullptr );

    ~QskDialogButtonBox() override;

    void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const;

    void setCenteredButtons( bool center );
    bool centeredButtons() const;

    void addButton( QskPushButton*, QskDialog::ActionRole );
    void addButton( QskDialog::Action );
    void removeButton( QskPushButton* );
    void clear();

    QVector< QskPushButton* > buttons() const;
    QskDialog::ActionRole actionRole( const QskPushButton* ) const;

    void setActions( QskDialog::Actions );
    QskDialog::Actions actions() const;
    QskDialog::Action action( const QskPushButton* ) const;

    QskDialog::Action defaultActionCandidate() const;

    QskPushButton* button( QskDialog::Action ) const;
    QskPushButton* buttonFromRole( QskDialog::ActionRole ) const;

    QskDialog::Action clickedAction() const;

    QSizeF contentsSizeHint() const override;

    QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol ) const override;

    static bool isDefaultButtonKeyEvent( const QKeyEvent* );
    static QString buttonText( QskDialog::Action );

  Q_SIGNALS:
    void clicked( QskPushButton* button );
    void accepted();
    void rejected();

    void centeredButtonsChanged();
    void orientationChanged();

  protected:
    bool event( QEvent* event ) override;
    void updateLayout() override;

    virtual QskPushButton* createButton( QskDialog::Action ) const;

    void invalidateLayout();

  private:
    void onButtonClicked();
    void rearrangeButtons();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
