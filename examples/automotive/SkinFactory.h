#ifndef SKIN_FACTORY_H
#define SKIN_FACTORY_H

#include <QskSkinFactory.h>

class SkinFactory : public QskSkinFactory
{
    Q_OBJECT

  public:
    enum GraphicRoles
    {
        // to be visible on a button
        Button,

        // to be visible on header/footer
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
