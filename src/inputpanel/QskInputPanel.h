/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_PANEL_H
#define QSK_INPUT_PANEL_H

#include "QskControl.h"
#include <memory>

class QskTextPredictor;
class QString;

class QSK_EXPORT QskInputPanel : public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

  public:
    QskInputPanel( QQuickItem* parent = nullptr );
    ~QskInputPanel() override;

    void attachInputItem( QQuickItem* );
    void updateInputPanel( Qt::InputMethodQueries );

    virtual QQuickItem* inputProxy() const;
    QQuickItem* inputItem() const;

    virtual Qt::Alignment alignment() const;

    QStringList candidates() const;

  public Q_SLOTS:
    void commitKey( int keyCode );
    void commitPredictiveText( int index );

  Q_SIGNALS:
    void keySelected( int keyCode );
    void predictiveTextSelected( int );
    void inputItemDestroyed();

    void predictionReset();
    void predictionRequested( const QString& text );

  public Q_SLOTS:
    virtual void setPrompt( const QString& );
    virtual void setPrediction( const QStringList& );
    virtual void setPredictionEnabled( bool );

  protected:
    virtual void attachItem( QQuickItem* ) = 0;

  private:
    void updatePrediction( const QString &text, const QStringList &candidates );
    void resetPredictor( const QLocale& );
    void updateLocale( const QLocale& );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
