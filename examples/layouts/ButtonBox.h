/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef _BUTTON_BOX_
#define _BUTTON_BOX_ 1

#include <QskLinearBox.h>
#include <functional>

class ButtonBox : public QskLinearBox
{
public:
    ButtonBox( QQuickItem* parent = nullptr );

    void addButton( const QString& text,
        std::function< void() > func, bool autoRepeat = false );
};

#endif
