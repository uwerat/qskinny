/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_MESSAGE_WINDOW_H
#define QSK_MESSAGE_WINDOW_H 1

#include "QskDialogWindow.h"

class QskTextOptions;
class QskGraphic;

class QSK_EXPORT QskMessageWindow : public QskDialogWindow
{
    Q_OBJECT

    Q_PROPERTY( QString infoText READ infoText
        WRITE setInfoText NOTIFY infoTextChanged )

    Q_PROPERTY( QskTextOptions infoTextOptions READ infoTextOptions
        WRITE setInfoTextOptions NOTIFY infoTextOptionsChanged )

    Q_PROPERTY( QUrl symbolSource READ symbolSource WRITE setSymbolSource )

    using Inherited = QskDialogWindow;

  public:
    QskMessageWindow( QWindow* parent = nullptr );
    ~QskMessageWindow() override;

    void setInfoTextOptions( const QskTextOptions& );
    QskTextOptions infoTextOptions() const;

    QString infoText() const;

    void setSymbolSource( const QUrl& url );
    QUrl symbolSource() const;

    void setSymbolType( int symbolType );

    void setSymbol( const QskGraphic& );
    QskGraphic symbol() const;

  public Q_SLOTS:
    void setInfoText( const QString& );

  Q_SIGNALS:
    void infoTextChanged( const QString& );
    void infoTextOptionsChanged( const QskTextOptions& );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;

};

#endif
