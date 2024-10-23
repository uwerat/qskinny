/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_CHECK_BOX_H
#define QSK_CHECK_BOX_H

#include "QskAbstractButton.h"

class QSK_EXPORT QskCheckBox : public QskAbstractButton
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text WRITE setText NOTIFY textChanged FINAL )

    using Inherited = QskAbstractButton;

  public:
    QSK_SUBCONTROLS( Panel, Box, Indicator, Text, Halo )
    QSK_STATES( Error )

    QskCheckBox( QQuickItem* parent = nullptr );
    QskCheckBox( const QString&, QQuickItem* parent = nullptr );

    ~QskCheckBox() override;

    QString text() const;

    bool isCheckable() const override final;

  public Q_SLOTS:
    void setText( const QString& );

  Q_SIGNALS:
    void textChanged();

  protected:
    void changeEvent( QEvent* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
