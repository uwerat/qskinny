/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_CONTEXT_H
#define QSK_INPUT_CONTEXT_H

#include "QskGlobal.h"
#include <QObject>
#include <Qt>
#include <memory>

class QskTextPredictor;
class QskInputPanel;
class QskPopup;
class QskWindow;
class QQuickItem;

class QSK_EXPORT QskInputContext : public QObject
{
    Q_OBJECT

    using Inherited = QObject;

public:
    QskInputContext();
    virtual ~QskInputContext();

    virtual QRectF panelRect() const;

    virtual void setActive( bool );
    virtual bool isActive() const;

    virtual QLocale locale() const;

    void registerPredictor( const QLocale&, QskTextPredictor* );
    QskTextPredictor* registeredPredictor( const QLocale& );

    virtual QQuickItem* inputItem() const;
    virtual QskInputPanel* inputPanel() const;

    static QskInputContext* instance();
    static void setInstance( QskInputContext* );

Q_SIGNALS:
    void activeChanged();
    void panelRectChanged();

protected:
    virtual bool eventFilter( QObject*, QEvent* ) override;

    virtual QskPopup* createEmbeddingPopup( QskInputPanel* );
    virtual QskWindow* createEmbeddingWindow( QskInputPanel* );

    virtual void showPanel();
    virtual void hidePanel();

private:
    friend class QskPlatformInputContext;

    // called from QskPlatformInputContext
    virtual void setFocusObject( QObject* );
    virtual void update( Qt::InputMethodQueries );
    virtual void processClickAt( int cursorPosition );
    virtual void commitPrediction( bool );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};


#endif
