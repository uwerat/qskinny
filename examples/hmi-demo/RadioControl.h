#ifndef RADIOCONTROL_H
#define RADIOCONTROL_H

#include <QskControl.h>

class RadioControl : public QskControl
{
public:
    RadioControl( QQuickItem *parent = nullptr );

private:
    QString m_currentBand;
};

#endif // RADIOCONTROL_H
