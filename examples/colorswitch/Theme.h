/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef THEME_H
#define THEME_H 1

#include <QColor>
#include <QObject>

class Theme : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QColor accent READ accent WRITE setAccent NOTIFY accentChanged )
    Q_PROPERTY( QString skin READ skin WRITE setSkin NOTIFY skinChanged )

  public:
    Theme( QObject* parent = nullptr );

    void setAccent( QColor color );
    QColor accent() const;

    void setSkin( const QString& );
    QString skin() const;

  Q_SIGNALS:
    void accentChanged();
    void skinChanged();

  private:
    void updateColors();

    QColor m_accent;
};

#endif
