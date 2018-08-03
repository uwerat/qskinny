#ifndef SKIN_FACTORY_H
#define SKIN_FACTORY_H

#include <QskSkinFactory.h>

class SkinFactory : public QskSkinFactory
{
    Q_OBJECT

  public:
    enum GraphicRoles
    {
        // to be visisble on a button
        Button,

        // to be visisble on header/footer
        Indicator,

        // in contrast to the background pixmap
        Vehicle
    };

    QStringList skinNames() const override;
    QskSkin* createSkin( const QString& skinName ) override;

  public Q_SLOTS:
    void rotateSkin();
    void toggleScheme();
};

#endif
