/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_WINDOW_H
#define QSK_INPUT_WINDOW_H 1

#include "QskDialogWindow.h"

class QskGraphic;
class QskTextOptions;
class QskDialogButtonBox;
class QskControl;
class QskInputSubWindow;

class QSK_EXPORT QskInputWindow : public QskDialogWindow
{
    Q_OBJECT

    Q_PROPERTY( QString infoText READ infoText
        WRITE setInfoText NOTIFY infoTextChanged FINAL )

    Q_PROPERTY( QskTextOptions infoTextOptions READ infoTextOptions
        WRITE setInfoTextOptions NOTIFY infoTextOptionsChanged )

    Q_PROPERTY( QUrl symbolSource READ symbolSource
        WRITE setSymbolSource NOTIFY symbolSourceChanged FINAL )

    using Inherited = QskDialogWindow;

  public:
    QskInputWindow( QWindow* parent = nullptr );
    ~QskInputWindow() override;

    QskDialog::StandardButtons standardButtons() const;
    void setStandardButtons( QskDialog::StandardButtons );

    QskDialog::StandardButton defaultButton() const;
    void setDefaultButton( QskDialog::StandardButton );

    void setInfoTextOptions( const QskTextOptions& );
    QskTextOptions infoTextOptions() const;

    QString infoText() const;

    void setSymbolType( int symbolType );

    void setSymbolSource( const QUrl& url );
    QUrl symbolSource() const;

    void setSymbol( const QskGraphic& );
    QskGraphic symbol() const;

    QskDialog::StandardButton clickedButton() const;

    QskDialogButtonBox* buttonBox();
    const QskDialogButtonBox* buttonBox() const;

  public Q_SLOTS:
    void setInfoText( const QString& );

  Q_SIGNALS:
    void infoTextChanged();
    void infoTextOptionsChanged();
    void symbolSourceChanged();

  protected:
    void setSubWindow( QskInputSubWindow* );
    QskInputSubWindow* subWindow() const;

  private:
    QskInputSubWindow* m_subWindow;
};

#endif
