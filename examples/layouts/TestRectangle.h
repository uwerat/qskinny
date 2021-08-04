/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskTextLabel.h>

class TestRectangle : public QskTextLabel
{
    Q_OBJECT

    Q_PROPERTY( QString color READ colorName WRITE setColorName )

  public:
    TestRectangle( QQuickItem* parent = nullptr );
    TestRectangle( const char* colorName, QQuickItem* parent = nullptr );

    void setColorName( const QString& );
    QString colorName() const;

  private:
    QString m_colorName;
};
