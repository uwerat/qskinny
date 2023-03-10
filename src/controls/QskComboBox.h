/******************************************************************************
 * QSkinny - Copyright (C) 2023 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_COMBO_BOX_H
#define QSK_COMBO_BOX_H

#include "QskControl.h"
#include <qstringlist.h>

class QskTextOptions;
class QskLabelData;
class QUrl;

class QSK_EXPORT QskComboBox : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( QVector< QskLabelData > options READ options
        WRITE setOptions NOTIFY optionsChanged )

    Q_PROPERTY( int currentIndex READ currentIndex
        WRITE setCurrentIndex NOTIFY currentIndexChanged )

    Q_PROPERTY( QString currentText READ currentText
        NOTIFY currentIndexChanged )

    Q_PROPERTY( int count READ count )

    Q_PROPERTY( QString placeholderText READ placeholderText
        WRITE setPlaceholderText NOTIFY placeholderTextChanged )

    Q_PROPERTY( int indexInPopup READ indexInPopup
        NOTIFY indexInPopupChanged )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Panel, Icon, Text, StatusIndicator )
    QSK_STATES( PopupOpen )

    QskComboBox( QQuickItem* parent = nullptr );

    ~QskComboBox() override;

    void setPopupOpen( bool );
    bool isPopupOpen() const;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    int addOption( const QUrl&, const QString& );
    int addOption( const QString&, const QString& );
    int addOption( const QskLabelData& );
    
    void setOptions( const QVector< QskLabelData >& );
    void setOptions( const QStringList& );
    
    QVector< QskLabelData > options() const;
    QskLabelData optionAt( int ) const;

    void clear();

    int currentIndex() const;
    QString currentText() const;

    // "highlightedIndex" ( see Qt's combo boxes ) is not very intuitive
    virtual int indexInPopup() const;

    int count() const;
    QString textAt( int ) const;

    QString placeholderText() const;
    void setPlaceholderText( const QString& );

  public Q_SLOTS:
    void setCurrentIndex( int );

  Q_SIGNALS:
    void currentIndexChanged( int );
    void indexInPopupChanged( int );

    void optionsChanged();
    void placeholderTextChanged( const QString& );

  protected:
    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;

    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

    void wheelEvent( QWheelEvent* ) override;

    /*
        open/close a menu - needs to be overloaded when using a custom popup
        don't forget to modify indexInPopup/indexInPopupChanged as well
     */
    virtual void openPopup();
    virtual void closePopup();

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
