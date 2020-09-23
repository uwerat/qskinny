#ifndef NIGHTTIMESKIN_H
#define NIGHTTIMESKIN_H

#include <QskSkin.h>

class NighttimeSkin : public QskSkin
{
    public:
        NighttimeSkin( QObject* parent = nullptr );

        enum SkinFontRole
        {
            TitleFont = QskSkin::HugeFont + 1,
        };

    private:
        void initHints();
};

#endif // NIGHTTIMESKIN_H
