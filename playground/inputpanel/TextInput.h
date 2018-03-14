#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include <private/qquicktextinput_p.h>

class TextInput : public QQuickTextInput
{
    public:
        TextInput( QQuickItem* parent );
        virtual ~TextInput();
};

#endif // TEXTINPUT_H
