#ifndef DAYTIMESKIN_H
#define DAYTIMESKIN_H

#include <QskSkin.h>

class DaytimeSkin : public QskSkin
{
public:
    DaytimeSkin( QObject* parent = nullptr );

private:
    void initHints();

};

#endif // DAYTIMESKIN_H
