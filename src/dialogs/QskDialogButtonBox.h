/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_DIALOG_BUTTON_BOX_H
#define QSK_DIALOG_BUTTON_BOX_H

#include "QskGlobal.h"
#include "QskDialog.h"
#include "QskBox.h"

class QskPushButton;

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

    virtual ~QskDialogButtonBox();

    void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const;

    void setCenteredButtons( bool center );
    bool centeredButtons() const;

    void addButton( QskPushButton* button, QskDialog::ButtonRole role );
    void addButton( QskDialog::StandardButton button );
    void removeButton( QskPushButton* button );
    void clear();

    QList< QskPushButton* > buttons() const;
    QskDialog::ButtonRole buttonRole( const QskPushButton* ) const;

    void setStandardButtons( QskDialog::StandardButtons buttons );
    QskDialog::StandardButtons standardButtons() const;
    QskDialog::StandardButton standardButton( const QskPushButton* ) const;

    QskDialog::StandardButton defaultButtonCandidate() const;

    QskPushButton* button( QskDialog::StandardButton ) const;
    QskPushButton* buttonFromRole( QskDialog::ButtonRole ) const;

    QskDialog::StandardButton clickedButton() const;

    virtual QSizeF contentsSizeHint() const override;

    virtual QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol ) const override;

    static bool isDefaultButtonKeyEvent( const QKeyEvent* );
    static QString buttonText( QskDialog::StandardButton );

Q_SIGNALS:
    void clicked( QskPushButton* button );
    void accepted();
    void rejected();

    void centeredButtonsChanged();
    void orientationChanged();

protected:
    virtual bool event( QEvent* event ) override;
    virtual void updateLayout() override;

    virtual QskPushButton* createButton( QskDialog::StandardButton ) const;

    void invalidateLayout();

private Q_SLOTS:
    void onButtonClicked();

private:
    void rearrangeButtons();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
