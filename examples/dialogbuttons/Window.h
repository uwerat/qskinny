/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef _WINDOW_H_
#define _WINDOW_H_ 1

#include <QskWindow.h>
#include <QskDialog.h>

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
    void addBox( QskDialog::StandardButtons );
    void addActionBox();

    QVector< QskDialogButtonBox* > dialogBoxes() const;
    Qt::Orientation invertedOrientation() const;

private:
    QskLinearBox* m_layoutBox;
    Qt::Orientation m_orientation;
};

#endif
