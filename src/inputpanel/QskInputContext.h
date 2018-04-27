/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_CONTEXT_H
#define QSK_INPUT_CONTEXT_H

#include "QskGlobal.h"
#include <qpa/qplatforminputcontext.h>
#include <memory>

class QskTextPredictor;
class QskInputPanel;
class QQuickItem;

class QSK_EXPORT QskInputContext : public QPlatformInputContext
{
    Q_OBJECT

    using Inherited = QPlatformInputContext;

public:
    QskInputContext();
    virtual ~QskInputContext();

    virtual bool isValid() const override;
    virtual bool hasCapability( Capability ) const override;

    virtual void update( Qt::InputMethodQueries ) override;
    virtual void invokeAction( QInputMethod::Action, int ) override;

    virtual QRectF keyboardRect() const override;
    virtual bool isAnimating() const override;

    virtual void showInputPanel() override;
    virtual void hideInputPanel() override;
    virtual bool isInputPanelVisible() const override;

    virtual void reset() override;
    virtual void commit() override;

    virtual void setFocusObject( QObject* ) override;

    virtual QLocale locale() const override;
    virtual Qt::LayoutDirection inputDirection() const override;

    void registerPredictor( const QLocale&, QskTextPredictor* );
    QskTextPredictor* registeredPredictor( const QLocale& );

    Q_INVOKABLE QQuickItem* inputItem();

    virtual bool filterEvent( const QEvent* ) override;

    static void setInputPanel( QskInputPanel* );
    static QskInputPanel* inputPanel();

protected:
    virtual bool eventFilter( QObject*, QEvent* ) override;

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};


#endif
