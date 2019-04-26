/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_CONTEXT_H
#define QSK_INPUT_CONTEXT_H

#include "QskGlobal.h"

#include <qinputmethod.h>
#include <qnamespace.h>
#include <qobject.h>

#include <memory>

class QskTextPredictor;
class QskInputPanel;
class QskInputEngine;
class QskPopup;
class QskWindow;
class QQuickItem;

class QSK_EXPORT QskInputContextFactory : public QObject
{
    Q_OBJECT

  public:
    QskInputContextFactory( QObject* parent = nullptr );
    ~QskInputContextFactory() override;

    virtual QskTextPredictor* createPredictor( const QLocale& ) const;
    virtual QskInputPanel* createPanel() const;
};

class QSK_EXPORT QskInputContext : public QObject
{
    Q_OBJECT

    using Inherited = QObject;

  public:
    QskInputContext();
    virtual ~QskInputContext();

    void setFactory( QskInputContextFactory* );
    QskInputContextFactory* factory() const;

    QRectF panelRect() const;

    void setInputPanelVisible( const QQuickItem*, bool );
    bool isInputPanelVisible() const;

    QLocale locale() const;

    static QskInputContext* instance();
    static void setInstance( QskInputContext* );

    QskTextPredictor* textPredictor( const QLocale& locale );

  Q_SIGNALS:
    void activeChanged();
    void panelRectChanged();

  protected:
    virtual void showPanel( const QQuickItem* );
    virtual void hidePanel( const QQuickItem* );

  private:
    void hideChannel( const QskInputPanel* );

    // called from QskPlatformInputContext
    friend class QskPlatformInputContext;
    virtual void setFocusObject( QObject* );
    virtual void update( const QQuickItem*, Qt::InputMethodQueries );
    virtual void invokeAction( QInputMethod::Action, int cursorPosition );

    void commitPrediction( bool );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
