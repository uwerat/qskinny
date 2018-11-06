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

    Q_PROPERTY( QString text READ text
        WRITE setText NOTIFY textChanged )

    Q_PROPERTY( QskTextOptions textOptions READ textOptions
        WRITE setTextOptions NOTIFY textOptionsChanged )

    Q_PROPERTY( QUrl symbolSource READ symbolSource WRITE setSymbolSource )

    using Inherited = QskDialogWindow;

  public:
    QskMessageWindow( QWindow* parent = nullptr );
    ~QskMessageWindow() override;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    QString text() const;

    void setSymbolSource( const QUrl& url );
    QUrl symbolSource() const;

    void setSymbolType( int symbolType );

    void setSymbol( const QskGraphic& );
    QskGraphic symbol() const;

  public Q_SLOTS:
    void setText( const QString& );

  Q_SIGNALS:
    void textChanged( const QString& );
    void textOptionsChanged( const QskTextOptions& );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
