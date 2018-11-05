/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef WINDOW_H
#define WINDOW_H 1

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

#endif
