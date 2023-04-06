/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
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
