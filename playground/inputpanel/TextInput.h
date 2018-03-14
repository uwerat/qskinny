#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include <private/qquicktextinput_p.h>

class TextInput : public QQuickTextInput
{
    public:
        TextInput( QQuickItem* parent );
        virtual ~TextInput() override;

protected:
        void inputMethodEvent(QInputMethodEvent *) Q_DECL_OVERRIDE;
};

#endif // TEXTINPUT_H
