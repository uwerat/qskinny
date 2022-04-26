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

    using Inherited = QskAbstractButton;

  public:
    QSK_SUBCONTROLS( Panel, Indicator )

    QskCheckBox( QQuickItem* parent = nullptr );
    ~QskCheckBox() override;

    bool isCheckable() const override final;
};

#endif
