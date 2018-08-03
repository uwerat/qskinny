#ifndef BUTTON_BAR_H
#define BUTTON_BAR_H

#include <QskLinearBox.h>

class ButtonBar : public QskLinearBox
{
  public:
    QSK_SUBCONTROLS( Indicator )

    ButtonBar( QQuickItem* = nullptr );
    void addIndicator( const char* name );

  protected:
    QSizeF contentsSizeHint() const override;
};

#endif
