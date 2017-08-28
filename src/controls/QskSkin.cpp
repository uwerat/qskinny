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
#include "QskSkinHintTable.h"

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
        skin( skin ),
        hintTable( true )
    {
    }

    QskSkin* skin;
    std::unordered_map< const QMetaObject*, SkinletData > skinletMap;

    QskSkinHintTable hintTable;

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

    setMargins( QskAspect::Control | QskAspect::Margin, 0 );
    setMargins( QskAspect::Control | QskAspect::Padding, 0 );
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
    return m_data->hintTable.color( aspect );
}

void QskSkin::setMetric( QskAspect::Aspect aspect, qreal metric )
{
    m_data->hintTable.setMetric( aspect, metric );
}

qreal QskSkin::metric( QskAspect::Aspect aspect ) const
{
    return m_data->hintTable.metric( aspect );
}

void QskSkin::setMargins( QskAspect::Aspect aspect, const QskMargins& margins )
{
    m_data->hintTable.setMargins( aspect, margins );
}

QskMargins QskSkin::margins( QskAspect::Aspect aspect ) const
{
    return m_data->hintTable.margins( aspect );
}

void QskSkin::setFontRole( QskAspect::Aspect aspect, int fontRole )
{
    m_data->hintTable.setFontRole( aspect, fontRole );
}

void QskSkin::setGraphicRole( QskAspect::Aspect aspect, int graphicRole )
{
    m_data->hintTable.setGraphicRole( aspect, graphicRole );
}

void QskSkin::setAnimation(
    QskAspect::Aspect aspect, QskAnimationHint animation )
{
    m_data->hintTable.setAnimation( aspect, animation );
}

QskAnimationHint QskSkin::animation( QskAspect::Aspect aspect ) const
{
    return m_data->hintTable.animation( aspect );
}

void QskSkin::setSkinHint( QskAspect::Aspect aspect, const QVariant& skinHint )
{
    m_data->hintTable.setSkinHint( aspect, skinHint );
}

const QVariant& QskSkin::skinHint( QskAspect::Aspect aspect ) const
{
    return m_data->hintTable.skinHint( aspect );
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

const QskSkinHintTable& QskSkin::hintTable() const
{
    return m_data->hintTable;
}

QskSkinHintTable& QskSkin::skinHintTable()
{
    return m_data->hintTable;
}

const std::unordered_map< int, QFont >& QskSkin::fonts() const
{
    return m_data->fonts;
}

const std::unordered_map< int, QskColorFilter >& QskSkin::graphicFilters() const
{
    return m_data->graphicFilters;
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
