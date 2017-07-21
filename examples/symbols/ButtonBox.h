/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef _BUTTON_BOX_H_
#define _BUTTON_BOX_H_

#include <QskLinearBox.h>
#include <SkinnyShapeFactory.h>

class ButtonBox : public QskLinearBox
{
    Q_OBJECT

public:
    ButtonBox( QQuickItem* parent = nullptr );
    virtual ~ButtonBox();

public Q_SLOTS:
    void onButtonClick();

private:
    void insertButton( const QColor&, SkinnyShapeFactory::Shape );
};

#endif
