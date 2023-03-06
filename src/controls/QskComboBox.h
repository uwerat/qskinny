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

    Q_PROPERTY( QString currentText READ currentText )

    Q_PROPERTY( int count READ count NOTIFY countChanged )

    Q_PROPERTY( QString placeholderText READ placeholderText
        WRITE setPlaceholderText NOTIFY placeholderTextChanged )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Panel, Graphic, Text, PopupIndicator )
    QSK_STATES( PopupOpen )

    QskComboBox( QQuickItem* parent = nullptr );

    ~QskComboBox() override;

    void setPopupOpen( bool );
    bool isPopupOpen() const;

    QskGraphic graphic() const;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    void addOption( const QUrl&, const QString& );

    void clear();

    int currentIndex() const;
    QString currentText() const;

    int count() const;
    QVariantList optionAt( int ) const;

    QString placeholderText() const;
    void setPlaceholderText( const QString& );

  public Q_SLOTS:
    void setCurrentIndex( int );

  Q_SIGNALS:
    void currentIndexChanged( int );

    void countChanged();
    void placeholderTextChanged( const QString& );

  protected:
    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;

    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

    void wheelEvent( QWheelEvent* ) override;

    virtual void openPopup();
    virtual void closePopup();

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
