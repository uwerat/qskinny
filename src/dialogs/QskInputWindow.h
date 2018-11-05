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

    Q_PROPERTY( QskDialog::Actions dialogActions
        READ dialogActions WRITE setDialogActions )

    Q_PROPERTY( QskDialog::Action defaultDialogAction
        READ defaultDialogAction WRITE setDefaultDialogAction )

    using Inherited = QskDialogWindow;

  public:
    QskInputWindow( QWindow* parent = nullptr );
    ~QskInputWindow() override;

    QskDialog::Actions dialogActions() const;
    void setDialogActions( QskDialog::Actions );

    QskDialog::Action defaultDialogAction() const;
    void setDefaultDialogAction( QskDialog::Action );

    void setInfoTextOptions( const QskTextOptions& );
    QskTextOptions infoTextOptions() const;

    QString infoText() const;

    void setSymbolType( int );

    void setSymbolSource( const QUrl& );
    QUrl symbolSource() const;

    void setSymbol( const QskGraphic& );
    QskGraphic symbol() const;

    QskDialog::Action clickedAction() const;

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
