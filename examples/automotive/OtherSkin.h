#ifndef OTHERSKIN_H
#define OTHERSKIN_H

#include <QskSkin.h>

class Palette;

class OtherSkin : public QskSkin
{
public:
    OtherSkin( const QString& name, QObject* parent = nullptr );
    ~OtherSkin() override;

private:
    void initHints();
    void initGraphicFilters();

    QString m_name;
    Palette* m_palette;
};

#endif // OTHERSKIN_H
