#ifndef DAYTIMESKIN_H
#define DAYTIMESKIN_H

#include <QskSkin.h>

class DaytimeSkin : public QskSkin
{
public:
    DaytimeSkin( QObject* parent = nullptr );

    enum SkinFontRole {
        TitleFont = QskSkin::HugeFont + 1,
    };

private:
    void initHints();

};

#endif // DAYTIMESKIN_H
