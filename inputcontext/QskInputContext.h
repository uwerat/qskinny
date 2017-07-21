#ifndef QSK_INPUT_CONTEXT_H
#define QSK_INPUT_CONTEXT_H

#include <qpa/qplatforminputcontext.h>
#include <QPointer>

#include <memory>

class QskInputPanel;
class QskInputCompositionModel;

class QskInputContext : public QPlatformInputContext
{
    Q_OBJECT

    using Inherited = QPlatformInputContext;

public:
    QskInputContext();
    ~QskInputContext();

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

protected:
    bool eventFilter( QObject*, QEvent* ) override;

private Q_SLOTS:
    void emitAnimatingChanged();
    void handleCandidatesChanged();
    void setInputPanel( QskInputPanel* );

private:
    QPointer< QObject > m_focusObject;
    QPointer< QskInputPanel > m_inputPanel;
    std::unique_ptr< QskInputCompositionModel > m_inputCompositionModel;
};

#endif // QSK_INPUT_CONTEXT_H
