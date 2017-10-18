#include "SkinFactory.h"
#include "DefaultSkin.h"
#include "OtherSkin.h"
#include "SoundControl.h"

#include <QskSetup.h>
#include <QskSkinTransition.h>
#include <QskColorFilter.h>
#include <QskTextLabel.h>
#include <QskPushButton.h>
#include <QskSeparator.h>
#include <QskBoxBorderMetrics.h>

#include <squiek/QskSquiekSkin.h>
#include <material/QskMaterialSkin.h>

QStringList SkinFactory::skinNames() const
{
    return { "DefaultSkin", "OtherSkin", "SquiekSkin", "MaterialSkin" };
}

QskSkin* SkinFactory::createSkin( const QString& skinName )
{
    if ( skinName == "DefaultSkin" )
        return new DefaultSkin( skinName );

    if ( skinName == "OtherSkin" )
        return new OtherSkin( skinName );

    if ( skinName == "SquiekSkin" )
    {
        auto skin = new QskSquiekSkin();

        const QColor themeColor( "FireBrick" );

        QColor overlayColor( "SlateGray");
        overlayColor.setAlpha( 200 );
        skin->setGradient( SoundControl::Overlay, overlayColor );

        QskColorFilter filter;
        filter.addColorSubstitution( Qt::white, themeColor.rgb() );
        skin->setGraphicFilter( SkinFactory::Indicator, filter );

        filter.addColorSubstitution( Qt::white, QColor( "CornflowerBlue" ).rgb() );
        skin->setGraphicFilter( SkinFactory::Vehicle, filter );

        skin->setSkinHint( SoundControl::MarkerControl | QskAspect::GraphicRole, SkinFactory::Indicator );
        skin->setSkinHint( SoundControl::Vehicle | QskAspect::GraphicRole, SkinFactory::Vehicle );

        skin->setGradient( SoundControl::Marker, themeColor );
        skin->setBoxBorderMetrics( SoundControl::Marker, 1 );
        skin->setBoxShape( SoundControl::Marker, 6 );

        skin->setColor( QskTextLabel::Text, "PeachPuff" );
        skin->setGradient( QskPushButton::Text, themeColor );

        return skin;
    }

    if ( skinName == "MaterialSkin" )
    {
        auto skin = new QskMaterialSkin();

        const QColor themeColor( "Tan" );

        QColor overlayColor = themeColor;
        overlayColor.setAlpha( 200 );
        skin->setGradient( SoundControl::Overlay, overlayColor );

        QskColorFilter filter;
        filter.addColorSubstitution( Qt::white, QColor( "SaddleBrown" ).rgb() );
        skin->setGraphicFilter( SkinFactory::Indicator, filter );

        skin->setSkinHint( SoundControl::MarkerControl | QskAspect::GraphicRole, SkinFactory::Indicator );

        skin->setBoxBorderMetrics( SoundControl::Marker, 1 );
        skin->setBoxShape( SoundControl::Marker, 8 );

        skin->setGradient( SoundControl::Marker, QColor( "SaddleBrown" ) );
        skin->setGradient( SoundControl::CrossHair, QColor( "Sienna" ) );
        skin->setColor( QskTextLabel::Text, "SaddleBrown" );
        skin->setGradient( QskSeparator::Panel, QColor( "Sienna" ) );

        return skin;
    }

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
    transition.setAnimation( 600 );

    transition.process();

    if ( oldSkin->parent() == nullptr )
        delete oldSkin;
}

#include "moc_SkinFactory.cpp"
