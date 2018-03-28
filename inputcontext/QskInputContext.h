/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_CONTEXT_H
#define QSK_INPUT_CONTEXT_H

#include <qpa/qplatforminputcontext.h>
#include <QPointer>
#include <QQuickItem>

#include <memory>

class QskVirtualKeyboard;
class QskInputCompositionModel;

class QskInputContext : public QPlatformInputContext
{
    Q_OBJECT

    using Inherited = QPlatformInputContext;

public:
    QskInputContext();
    ~QskInputContext() override;

    bool isValid() const override;
    void update( Qt::InputMethodQueries ) override;
    void invokeAction( QInputMethod::Action, int ) override;
    QRectF keyboardRect() const override;
    bool isAnimating() const override;
    void showInputPanel() override;
    void hideInputPanel() override;
    bool isInputPanelVisible() const override;
    QLocale locale() const override;
    void setFocusObject( QObject* ) override;

private Q_SLOTS:
    void emitAnimatingChanged();
    void handleCandidatesChanged();
    void setInputPanel( QskVirtualKeyboard* );

private:
    QPointer< QObject > m_focusObject;
    QPointer< QQuickItem > m_inputItem;
    QPointer< QskVirtualKeyboard > m_inputPanel;
    std::unique_ptr< QskInputCompositionModel > m_inputCompositionModel;
};

#endif // QSK_INPUT_CONTEXT_H
