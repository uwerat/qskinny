/******************************************************************************
 * QSkinny - Copyright (C) 2023 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_COMBO_BOX_H
#define QSK_COMBO_BOX_H

#include "QskControl.h"

class QskGraphic;

class QSK_EXPORT QskComboBox : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( int currentIndex READ currentIndex
        WRITE setCurrentIndex NOTIFY currentIndexChanged )

    Q_PROPERTY( int count READ count NOTIFY countChanged )

    Q_PROPERTY( QString placeholderText READ placeholderText
        WRITE setPlaceholderText NOTIFY placeholderTextChanged )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Panel, Graphic, Text, OpenMenuGraphic, Splash )
    QSK_STATES( Pressed, PopupOpen )

    QskComboBox( QQuickItem* parent = nullptr );

    ~QskComboBox() override;

    void setPressed( bool on );
    bool isPressed() const;

    void setPopupOpen( bool on );
    bool isPopupOpen() const;

    QskGraphic graphic() const;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    void addOption( const QUrl&, const QString& );

    void clear();

    int currentIndex() const;

    int count() const;

    QVariantList optionAt( int ) const;

    QString placeholderText() const;
    void setPlaceholderText( const QString& );

    QString text() const;

  public Q_SLOTS:
    void togglePopup();
    virtual void openPopup();
    virtual void closePopup();

    void setCurrentIndex( int );

  Q_SIGNALS:
    void currentIndexChanged( int );
    void countChanged();

    void pressed();
    void released();
    void clicked();

    void pressedChanged( bool );
    void popupOpenChanged( bool );

    void placeholderTextChanged( const QString& );

  protected:
    void mousePressEvent( QMouseEvent* ) override;
    void mouseUngrabEvent() override;
    void mouseReleaseEvent( QMouseEvent* ) override;

    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

  private:
    void releaseButton();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
