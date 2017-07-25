#ifndef _SKIN_FACTORY_H_
#define _SKIN_FACTORY_H_

#include <QskSkinFactory.h>

class SkinFactory : public QskSkinFactory
{
    Q_OBJECT

public:
    virtual QStringList skinNames() const override;
    virtual QskSkin* createSkin( const QString& skinName ) override;

public Q_SLOTS:
    void rotateSkin();
    void toggleScheme();
};

#endif
