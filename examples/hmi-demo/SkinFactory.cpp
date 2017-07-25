#include "SkinFactory.h"
#include "DefaultSkin.h"
#include "OtherSkin.h"

#include <QskSetup.h>
#include <QskSkinTransition.h>

#include <squiek/QskSquiekSkin.h>
#include <material/QskMaterialSkin.h>

QStringList SkinFactory::skinNames() const
{
    return { "DefaultSkin", "OtherSkin", "Squiek", "Material" };
}

QskSkin* SkinFactory::createSkin( const QString& skinName )
{
    if ( skinName == "DefaultSkin" )
        return new DefaultSkin( skinName );

    if ( skinName == "OtherSkin" )
        return new OtherSkin( skinName );

    if ( skinName == "SquiekSkin" )
        return new QskSquiekSkin();

    if ( skinName == "MaterialSkin" )
        return new QskMaterialSkin();

    return nullptr;
}

void SkinFactory::toggleScheme()
{
    if ( qskSetup->skinName() == "DefaultSkin" )
        static_cast< DefaultSkin* >( qskSetup->skin() )->toggleScheme();
}

void SkinFactory::rotateSkin()
{
    const QStringList names = skinNames();

    int index = names.indexOf( qskSetup->skinName() );
    index = ( index + 1 ) % names.size();

    QskSkin* oldSkin = qskSetup->skin();
    if ( oldSkin->parent() == qskSetup )
        oldSkin->setParent( nullptr ); // otherwise setSkin deletes it

    QskSkin* newSkin = qskSetup->setSkin( names[ index ] );

    QskSkinTransition transition;

    transition.setSourceSkin( oldSkin );
    transition.setTargetSkin( newSkin );
    transition.setAnimation( 500 );

    transition.process();

    if ( oldSkin->parent() == nullptr )
        delete oldSkin;
}

#include "moc_SkinFactory.cpp"
