/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef DEFAULTSKIN_H
#define DEFAULTSKIN_H

#include <QskSkin.h>

class Palette;

class DefaultSkin : public QskSkin
{
  public:
    DefaultSkin( const QString& name, QObject* parent = nullptr );
    ~DefaultSkin() override;

    void toggleScheme();
    void resetHints();

    enum Scheme
    {
        Daylight,
        Nighttime
    };

  private:
    void initHints();

    QString m_name;
    Palette* m_palette;
    Scheme m_scheme;
};

#endif // DEFAULTSKIN_H
