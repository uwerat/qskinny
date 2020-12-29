/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef MY_TOGGLE_BUTTON_H
#define MY_TOGGLE_BUTTON_H

#include <QskAbstractButton.h>

class QskGraphic;
class QskTextOptions;

class MyToggleButton : public QskAbstractButton
{
    Q_OBJECT
    using Inherited = QskAbstractButton;

  public:
    QSK_SUBCONTROLS( Panel, Cursor, CheckedPanel, CheckedLabel, CheckedIcon,
        UncheckedPanel, UncheckedLabel, UncheckedIcon )

    MyToggleButton( QQuickItem* parent = nullptr );
    ~MyToggleButton() override;

    void setTextAt( int index, const QString& );
    QString textAt( int index ) const;

    void setIconAt( int index, const QString& icon );
    QString iconAt( int index ) const;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    QskGraphic graphicAt( int index ) const;

    void setInverted( bool );
    bool isInverted() const;

  Q_SIGNALS:
    void invertedChanged( bool );

  protected:
    void updateResources() override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
