/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_CHECK_BOX_H
#define QSK_CHECK_BOX_H

#include "QskAbstractButton.h"

class QSK_EXPORT QskCheckBox : public QskAbstractButton
{
    Q_OBJECT

    Q_PROPERTY( Qt::CheckState checkState READ checkState
        WRITE setCheckState NOTIFY checkStateChanged FINAL )

    Q_PROPERTY( bool tristate READ isTristate
        WRITE setTristate NOTIFY tristateChanged FINAL )

    using Inherited = QskAbstractButton;

  public:
    QSK_SUBCONTROLS( Box, Tick )
    QSK_STATES( PartiallyChecked )

    QskCheckBox( QQuickItem* parent = nullptr );
    ~QskCheckBox() override;

    Qt::CheckState checkState() const;
    bool isTristate() const;
    bool isCheckable() const override final;

    void addToGroup( QskCheckBox* );
    void removeFromGroup( QskCheckBox* );

  public Q_SLOTS:
    void setCheckState( Qt::CheckState );
    void setTristate( bool triState = true );

  Q_SIGNALS:
    void checkStateChanged( Qt::CheckState );
    void tristateChanged( bool );
    void removeFromAllGroupsRequested();

  private:
    void setCheckStateInternal( Qt::CheckState );
    void updated();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
