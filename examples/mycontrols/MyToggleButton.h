/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskAbstractButton.h>

class QskGraphic;
class QskTextOptions;

class MyToggleButton : public QskAbstractButton
{
    Q_OBJECT
    using Inherited = QskAbstractButton;

  public:
    QSK_SUBCONTROLS( Panel, Cursor, CheckedPanel, CheckedText, CheckedGraphic,
        UncheckedPanel, UncheckedText, UncheckedGraphic )

    MyToggleButton( QQuickItem* parent = nullptr );
    ~MyToggleButton() override;

    bool isCheckable() const override final;

    void setText( bool isChecked, const QString& );
    QString text( bool isChecked ) const;

    void setIcon( bool isChecked, const QString& icon );
    QString icon( bool isChecked ) const;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    QskGraphic graphic( bool isChecked ) const;

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
