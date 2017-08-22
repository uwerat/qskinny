/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkin.h"

#include "QskAspect.h"
#include "QskGraphic.h"
#include "QskColorFilter.h"
#include "QskGraphicProviderMap.h"
#include "QskSetup.h"
#include "QskAnimationHint.h"
#include "QskStandardSymbol.h"

#include "QskFocusIndicator.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qguiapplication_p.h>
QSK_QT_PRIVATE_END

#include <qpa/qplatformtheme.h>
#include <qpa/qplatformdialoghelper.h>

#include <QtAlgorithms>

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <cmath>

#include "QskBox.h"
#include "QskBoxSkinlet.h"

#include "QskFocusIndicator.h"
#include "QskFocusIndicatorSkinlet.h"

#include "QskGraphicLabel.h"
#include "QskGraphicLabelSkinlet.h"

#include "QskInputPanel.h"
#include "QskInputPanelSkinlet.h"

#include "QskListView.h"
#include "QskListViewSkinlet.h"

#include "QskPopup.h"
#include "QskPopupSkinlet.h"

#include "QskPushButton.h"
#include "QskPushButtonSkinlet.h"

#include "QskScrollView.h"
#include "QskScrollViewSkinlet.h"

#include "QskSlider.h"
#include "QskSliderSkinlet.h"

#include "QskTabButton.h"
#include "QskTabButtonSkinlet.h"

#include "QskTabView.h"
#include "QskTabViewSkinlet.h"

#include "QskTextLabel.h"
#include "QskTextLabelSkinlet.h"

#include "QskSeparator.h"
#include "QskSeparatorSkinlet.h"

#include "QskSubWindow.h"
#include "QskSubWindowSkinlet.h"

#include "QskSubWindowArea.h"
#include "QskSubWindowAreaSkinlet.h"

#include "QskPageIndicator.h"
#include "QskPageIndicatorSkinlet.h"

#include "QskStatusIndicator.h"
#include "QskStatusIndicatorSkinlet.h"

namespace
{
    class SkinletData
    {
    public:
        SkinletData( const QMetaObject* metaObject ):
            metaObject( metaObject ),
            skinlet( nullptr )
        {
        }

        ~SkinletData()
        {
            delete skinlet;
        }

        const QMetaObject* metaObject;
        QskSkinlet* skinlet;
    };
}

class QskSkin::PrivateData
{
public:
    PrivateData( QskSkin* skin ):
        skin( skin )
    {
    }

    QskSkin* skin;
    std::unordered_map< const QMetaObject*, SkinletData > skinletMap;

    std::unordered_map< QskAspect::Aspect, QVariant > skinHints;
    std::unordered_map< int, std::set< QskAspect::Aspect > > animatorAspects;

    std::unordered_map< int, QFont > fonts;
    std::unordered_map< int, QskColorFilter > graphicFilters;

    QskGraphicProviderMap graphicProviders;
};

QskSkin::QskSkin( QObject* parent ):
    QObject( parent ),
    m_data( new PrivateData( this ) )
{
    declareSkinlet< QskControl, QskSkinlet >();

    declareSkinlet< QskBox, QskBoxSkinlet >();
    declareSkinlet< QskFocusIndicator, QskFocusIndicatorSkinlet >();
    declareSkinlet< QskGraphicLabel, QskGraphicLabelSkinlet >();
    declareSkinlet< QskInputPanel, QskInputPanelSkinlet >();
    declareSkinlet< QskListView, QskListViewSkinlet >();
    declareSkinlet< QskPageIndicator, QskPageIndicatorSkinlet >();
    declareSkinlet< QskPopup, QskPopupSkinlet >();
    declareSkinlet< QskPushButton, QskPushButtonSkinlet >();
    declareSkinlet< QskScrollView, QskScrollViewSkinlet >();
    declareSkinlet< QskSeparator, QskSeparatorSkinlet >();
    declareSkinlet< QskSlider, QskSliderSkinlet >();
    declareSkinlet< QskStatusIndicator, QskStatusIndicatorSkinlet >();
    declareSkinlet< QskSubWindow, QskSubWindowSkinlet >();
    declareSkinlet< QskSubWindowArea, QskSubWindowAreaSkinlet >();
    declareSkinlet< QskTabButton, QskTabButtonSkinlet >();
    declareSkinlet< QskTabView, QskTabViewSkinlet >();
    declareSkinlet< QskTextLabel, QskTextLabelSkinlet >();

    const QFont font = QGuiApplication::font();
    setupFonts( font.family(), font.weight(), font.italic() );
}

QskSkin::~QskSkin()
{
}

void QskSkin::setColor( QskAspect::Aspect aspect, QRgb rgb )
{
    setSkinHint( aspect | QskAspect::Color, QColor::fromRgba( rgb ) );
}

void QskSkin::setColor( QskAspect::Aspect aspect, Qt::GlobalColor color )
{
    setSkinHint( aspect | QskAspect::Color, QColor( color ) );
}

void QskSkin::setColor( QskAspect::Aspect aspect, const QColor& color )
{
    setSkinHint( aspect | QskAspect::Color, color );
}

QColor QskSkin::color( QskAspect::Aspect aspect ) const
{
    return skinHint( aspect | QskAspect::Color ).value<QColor>();
}

void QskSkin::setMetric( QskAspect::Aspect aspect, qreal metric )
{
    setSkinHint( aspect | QskAspect::Metric, metric );
}

qreal QskSkin::metric( QskAspect::Aspect aspect ) const
{
    return skinHint( aspect | QskAspect::Metric ).toReal();
}

void QskSkin::setAnimation(
    QskAspect::Aspect aspect, QskAnimationHint animation )
{
    aspect.setAnimator( true );
    setSkinHint( aspect, QVariant::fromValue( animation ) );
}

QskAnimationHint QskSkin::animation( QskAspect::Aspect aspect ) const
{
    aspect.setAnimator( true );
    return skinHint( aspect ).value< QskAnimationHint >();
}

void QskSkin::setSkinHint( QskAspect::Aspect aspect, const QVariant& skinHint )
{
    // For edges/corners, add all the implied assignments
    if ( aspect.isBoxPrimitive() )
    {
        if ( aspect.boxPrimitive() == QskAspect::Radius )
        {
            setSkinHint( aspect | QskAspect::RadiusX, skinHint );
            setSkinHint( aspect | QskAspect::RadiusY, skinHint );
            return;
        }

        const auto edges = aspect.edge();

        const auto bitcount = qPopulationCount( static_cast<quint8>( edges ) );
        if ( !bitcount || bitcount > 1 )
        {
            using namespace QskAspect;

            auto aspectEdges = aspect;
            aspectEdges.clearEdge();

            if ( !bitcount || edges & TopEdge )
                setSkinHint( aspectEdges | TopEdge, skinHint );

            if ( !bitcount || ( edges & LeftEdge ) )
                setSkinHint( aspectEdges | LeftEdge, skinHint );

            if ( !bitcount || ( edges & RightEdge ) )
                setSkinHint( aspectEdges | RightEdge, skinHint );

            if ( !bitcount || ( edges & BottomEdge ) )
                setSkinHint( aspectEdges | BottomEdge, skinHint );
        }

        if ( bitcount > 1 ) // Allows 0 to imply AllEdges
            return;
    }

    auto it = m_data->skinHints.find( aspect );
    if ( it == m_data->skinHints.end() )
    {
        m_data->skinHints.emplace( aspect, skinHint );
    }
    else if ( it->second != skinHint )
    {
        it->second = skinHint;
    }

    if ( aspect.isAnimator() )
        m_data->animatorAspects[ aspect.subControl() ].insert( aspect );
}

void QskSkin::removeSkinHint( QskAspect::Aspect aspect )
{
    m_data->skinHints.erase( aspect );
}

const QVariant& QskSkin::skinHint( QskAspect::Aspect aspect ) const
{
    auto it = m_data->skinHints.find( aspect );
    if ( it != m_data->skinHints.cend() )
        return it->second;

    static QVariant invalidHint;
    return invalidHint;
}

void QskSkin::declareSkinlet( const QMetaObject* metaObject,
    const QMetaObject* skinletMetaObject )
{
    const auto it = m_data->skinletMap.find( metaObject );

    if ( it != m_data->skinletMap.cend() )
    {
        auto& entry = it->second;
        if ( entry.metaObject != skinletMetaObject )
        {
            entry.metaObject = skinletMetaObject;
            if ( entry.skinlet != nullptr )
            {
                delete entry.skinlet;
                entry.skinlet = nullptr;
            }
        }
    }
    else
    {
        m_data->skinletMap.emplace( metaObject, skinletMetaObject );
    }
}

void QskSkin::setupFonts( const QString& family, int weight, bool italic )
{
    QFont font( family, -1, weight, italic );

    const uint base = TinyFont;
    for ( int i = TinyFont; i <= HugeFont; i++ )
    {
        // TODO: make the scaling components configurable
        font.setPixelSize( int( std::pow( uint( i ) - base + 2, 2.5 ) ) );
        m_data->fonts[ i ] = font;
    }

    const QFont appFont( QGuiApplication::font() );
    if ( appFont.pixelSize() > 0 )
        font.setPixelSize( appFont.pixelSize() );
    else
        font.setPointSize( appFont.pointSize() );

    m_data->fonts[ QskSkin::DefaultFont ] = font;
}

void QskSkin::setFont( int fontRole, const QFont& font )
{
    m_data->fonts[ fontRole ] = font;
}

void QskSkin::resetFont( int fontRole )
{
    m_data->fonts.erase( fontRole );
}

QFont QskSkin::font( int fontRole ) const
{
    auto it = m_data->fonts.find( fontRole );
    if ( it != m_data->fonts.cend() )
        return it->second;

    it = m_data->fonts.find( QskSkin::DefaultFont );
    if ( it != m_data->fonts.cend() )
        return it->second;

    return QGuiApplication::font();
}

void QskSkin::setGraphicFilter( int graphicRole, const QskColorFilter& colorFilter )
{
    m_data->graphicFilters[ graphicRole ] = colorFilter;
}

void QskSkin::resetGraphicFilter( int graphicRole )
{
    m_data->graphicFilters.erase( graphicRole );
}

QskColorFilter QskSkin::graphicFilter( int graphicRole ) const
{
    auto it = m_data->graphicFilters.find( graphicRole );
    if ( it != m_data->graphicFilters.cend() )
        return it->second;

    return QskColorFilter();
}

const std::unordered_map< QskAspect::Aspect, QVariant >& QskSkin::skinHints() const
{
    return m_data->skinHints;
}

const std::unordered_map< int, QFont >& QskSkin::fonts() const
{
    return m_data->fonts;
}

const std::unordered_map< int, QskColorFilter >& QskSkin::graphicFilters() const
{
    return m_data->graphicFilters;
}

const std::set< QskAspect::Aspect >& QskSkin::animatorAspects(
    QskAspect::Subcontrol subControl ) const
{
    auto it = m_data->animatorAspects.find( subControl );
    if ( it != m_data->animatorAspects.cend() )
        return it->second;

    static std::set< QskAspect::Aspect > dummyAspects;
    return dummyAspects;
}

QskGraphic QskSkin::symbol( int symbolType ) const
{
    // should this one be somehow related to the platform icons ???
    return QskStandardSymbol::graphic(
        static_cast< QskStandardSymbol::Type >( symbolType ) );
}

void QskSkin::addGraphicProvider(
    const QString& providerId, QskGraphicProvider* provider )
{
    m_data->graphicProviders.insert( providerId, provider );
}

QskGraphicProvider* QskSkin::graphicProvider( const QString& providerId ) const
{
    return m_data->graphicProviders.provider( providerId );
}

bool QskSkin::hasGraphicProvider() const
{
    return m_data->graphicProviders.size() > 0;
}

const int *QskSkin::dialogButtonLayout( Qt::Orientation orientation ) const
{
    //auto policy = QPlatformDialogHelper::UnknownLayout;
    auto policy = QPlatformDialogHelper::WinLayout;

    if ( const QPlatformTheme* theme = QGuiApplicationPrivate::platformTheme() )
    {
        const QVariant v = theme->themeHint( QPlatformTheme::DialogButtonBoxLayout );
        policy = static_cast<QPlatformDialogHelper::ButtonLayout>( v.toInt() );
    }

    return QPlatformDialogHelper::buttonLayout( orientation, policy );
}

QskSkinlet* QskSkin::skinlet( const QskSkinnable* skinnable )
{
    for ( auto metaObject = skinnable->metaObject();
        metaObject != nullptr; metaObject = metaObject->superClass() )
    {
        auto it = m_data->skinletMap.find( metaObject );
        if ( it != m_data->skinletMap.cend() )
        {
            auto& entry = it->second;

            if ( entry.skinlet == nullptr )
            {
                entry.skinlet = reinterpret_cast< QskSkinlet* >(
                    entry.metaObject->newInstance( Q_ARG( QskSkin*, this ) ) );
            }

            return entry.skinlet;
        }
    }

    static QskSkinlet defaultSkinlet;
    return &defaultSkinlet;
}

void QskSkin::resetColors( const QColor& )
{
}

#include "moc_QskSkin.cpp"
