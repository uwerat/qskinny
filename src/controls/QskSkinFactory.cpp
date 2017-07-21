#include "QskSkinFactory.h"

#include "squiek/QskSquiekSkin.h"
#include "material/QskMaterialSkin.h"

#include <QHash>
#include <QPointer>

namespace
{
    static const QString factoryId = "QSkinny";
    static const QString squiekSkinName = "Squiek";
    static const QString materialSkinName = "Material";

    class SkinFactory final : public QskSkinFactory
    {
    public:
        SkinFactory( QObject* parent ):
            QskSkinFactory( parent )
        {
        }

        virtual QStringList skinNames() const override final
        {
            return { squiekSkinName, materialSkinName };
        }

        virtual QskSkin* createSkin( const QString& name ) override final
        {
            QskSkin* skin = nullptr;

            if ( name == squiekSkinName )
                skin = new QskSquiekSkin();

            if ( name == materialSkinName )
                skin = new QskMaterialSkin();

            if ( skin )
                skin->setObjectName( name );

            return skin;
        }
    };
}

static QHash< QString, QPointer< QskSkinFactory > >& qskFactoryTable()
{
    static QHash< QString, QPointer< QskSkinFactory > > table;
    if ( table.isEmpty() )
    {
        static SkinFactory dummySkinFactory ( nullptr );
        table.insert( factoryId.toLower(), &dummySkinFactory );
    }

    return table;
}

QskSkinFactory::QskSkinFactory( QObject* parent ):
    QObject( parent )
{
}

QskSkinFactory::~QskSkinFactory() = default;


void Qsk::registerSkinFactory( const QString& id, QskSkinFactory* factory )
{
    qskFactoryTable().insert( id.toLower(), factory );
}

QskSkinFactory* Qsk::skinFactory( const QString& id )
{
    return qskFactoryTable().value( id.toLower() ).data();
}

QStringList Qsk::skinNames()
{
    QStringList names;

    for ( auto factory : qskFactoryTable() )
    {
        if ( factory )
            names += factory->skinNames();
    }

    return names;
}

QskSkin* Qsk::createSkin( const QString& skinName )
{
    if ( !skinName.isEmpty() )
    {
        for ( auto factory : qskFactoryTable() )
        {
            QskSkin* skin = factory->createSkin( skinName );
            if ( skin )
                return skin;

        }
    }

    QskSkinFactory* factory = skinFactory( factoryId );
    return factory->createSkin( squiekSkinName );
}

#include "moc_QskSkinFactory.cpp"
