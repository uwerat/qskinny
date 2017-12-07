#ifndef BUTTON_BAR
#define BUTTON_BAR

#include <QskLinearBox.h>

class ButtonBar : public QskLinearBox
{
public:
    QSK_SUBCONTROLS( Indicator )

    ButtonBar( QQuickItem* = nullptr );
    void addIndicator( const char* name );

protected:
    virtual QSizeF contentsSizeHint() const override;
};

#endif
