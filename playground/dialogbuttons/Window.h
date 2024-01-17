/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskDialog.h>
#include <QskWindow.h>

class QskDialogButtonBox;
class QskLinearBox;

class Window : public QskWindow
{
    Q_OBJECT

  public:
    Window( Qt::Orientation );

  private Q_SLOTS:
    void flipOrientation();
    void centerButtons();

  private:
    void addBox( QskDialog::Actions );
    void addActionBox();

    QVector< QskDialogButtonBox* > dialogBoxes() const;
    Qt::Orientation invertedOrientation() const;

  private:
    QskLinearBox* m_layoutBox;
    Qt::Orientation m_orientation;
};
