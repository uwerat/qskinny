#ifndef DEFAULTSKIN_H
#define DEFAULTSKIN_H

#include <QskSkin.h>

class Palette;

class DefaultSkin : public QskSkin
{
public:
    DefaultSkin( const QString& name, QObject* parent = nullptr );
    virtual ~DefaultSkin();

    void toggleScheme();
    void resetColors();

    enum Scheme
    {
        Daylight,
        Nighttime
    };

private:
    class Transition;

    void initHints();

    QString m_name;
    Palette* m_palette;
    Scheme m_scheme;
};

#endif // DEFAULTSKIN_H
