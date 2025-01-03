/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSkin.h"

#include "QskAnimationHint.h"
#include "QskAspect.h"
#include "QskColorFilter.h"
#include "QskGraphic.h"
#include "QskGraphicProviderMap.h"
#include "QskStandardSymbol.h"
#include "QskPlatform.h"
#include "QskMargins.h"
#include "QskFontRole.h"

#include "QskSkinHintTable.h"
#include "QskSkinManager.h"
#include "QskSkinTransition.h"

#include <qguiapplication.h>
#include <qpa/qplatformdialoghelper.h>
#include <qpa/qplatformtheme.h>

#include <cmath>

#include "QskBox.h"
#include "QskBoxSkinlet.h"

#include "QskCheckBox.h"
#include "QskCheckBoxSkinlet.h"

#include "QskComboBox.h"
#include "QskComboBoxSkinlet.h"

#include "QskDrawer.h"
#include "QskDrawerSkinlet.h"

#include "QskFocusIndicator.h"
#include "QskFocusIndicatorSkinlet.h"

#include "QskGraphicLabel.h"
#include "QskGraphicLabelSkinlet.h"

#include "QskListView.h"
#include "QskListViewSkinlet.h"

#include "QskMenu.h"
#include "QskMenuSkinlet.h"

#include "QskPageIndicator.h"
#include "QskPageIndicatorSkinlet.h"

#include "QskPopup.h"
#include "QskPopupSkinlet.h"

#include "QskProgressBar.h"
#include "QskProgressBarSkinlet.h"

#include "QskProgressRing.h"
#include "QskProgressRingSkinlet.h"

#include "QskRadioBox.h"
#include "QskRadioBoxSkinlet.h"

#include "QskPushButton.h"
#include "QskPushButtonSkinlet.h"

#include "QskScrollView.h"
#include "QskScrollViewSkinlet.h"

#include "QskSegmentedBar.h"
#include "QskSegmentedBarSkinlet.h"

#include "QskSeparator.h"
#include "QskSeparatorSkinlet.h"

#include "QskSlider.h"
#include "QskSliderSkinlet.h"

#include "QskSpinBox.h"
#include "QskSpinBoxSkinlet.h"

#include "QskSubWindow.h"
#include "QskSubWindowSkinlet.h"

#include "QskSubWindowArea.h"
#include "QskSubWindowAreaSkinlet.h"

#include "QskSwitchButton.h"
#include "QskSwitchButtonSkinlet.h"

#include "QskTabButton.h"
#include "QskTabButtonSkinlet.h"

#include "QskTabView.h"
#include "QskTabViewSkinlet.h"

#include "QskTextLabel.h"
#include "QskTextLabelSkinlet.h"

#include "QskTextField.h"
#include "QskTextFieldSkinlet.h"

#include "QskStatusIndicator.h"
#include "QskStatusIndicatorSkinlet.h"

#include "QskInternalMacros.h"

#include <qhash.h>

static inline QskSkinlet* qskNewSkinlet( const QMetaObject* metaObject, QskSkin* skin )
{
    const QByteArray signature = metaObject->className() + QByteArrayLiteral( "(QskSkin*)" );

    QskSkinlet* skinlet = nullptr;

    const int index = metaObject->indexOfConstructor( signature.constData() );
    if ( index >= 0 )
    {
        void* param[] = { &skinlet, &skin };
        metaObject->static_metacall( QMetaObject::CreateInstance, index, param );
    }

    return skinlet;
}

// also used in QskSkinTransition.cpp TODO ...

QSK_HIDDEN_EXTERNAL_BEGIN

QFont qskResolvedFont( const QHash< QskFontRole, QFont >& fontTable,
    const QskFontRole& fontRole )
{
    auto it = fontTable.constFind( fontRole );
    if ( it != fontTable.constEnd() )
        return it.value();

    it = fontTable.constFind( QskFontRole() );
    if ( it != fontTable.constEnd() )
        return it.value();

    return QGuiApplication::font();
}

QSK_HIDDEN_EXTERNAL_END

namespace
{
    class SkinletData
    {
      public:
        SkinletData( const QMetaObject* metaObject )
            : metaObject( metaObject )
            , skinlet( nullptr )
        {
        }

        ~SkinletData()
        {
            delete skinlet;
        }

        const QMetaObject* metaObject;
        QskSkinlet* skinlet; // mutable ???
    };
}

class QskSkin::PrivateData
{
  public:
    QHash< const QMetaObject*, SkinletData > skinletMap;

    QskSkinHintTable hintTable;

    QHash< QskFontRole, QFont > fonts;
    QHash< int, QskColorFilter > graphicFilters;

    QskGraphicProviderMap graphicProviders;

    int colorScheme = -1; // uninitialized
};

QskSkin::QskSkin( QObject* parent )
    : QObject( parent )
    , m_data( new PrivateData() )
{
    declareSkinlet< QskControl, QskSkinlet >();

    declareSkinlet< QskBox, QskBoxSkinlet >();
    declareSkinlet< QskCheckBox, QskCheckBoxSkinlet >();
    declareSkinlet< QskComboBox, QskComboBoxSkinlet >();
    declareSkinlet< QskDrawer, QskDrawerSkinlet >();
    declareSkinlet< QskFocusIndicator, QskFocusIndicatorSkinlet >();
    declareSkinlet< QskGraphicLabel, QskGraphicLabelSkinlet >();
    declareSkinlet< QskListView, QskListViewSkinlet >();
    declareSkinlet< QskPageIndicator, QskPageIndicatorSkinlet >();
    declareSkinlet< QskPopup, QskPopupSkinlet >();
    declareSkinlet< QskMenu, QskMenuSkinlet >();
    declareSkinlet< QskPushButton, QskPushButtonSkinlet >();
    declareSkinlet< QskScrollView, QskScrollViewSkinlet >();
    declareSkinlet< QskSegmentedBar, QskSegmentedBarSkinlet >();
    declareSkinlet< QskSeparator, QskSeparatorSkinlet >();
    declareSkinlet< QskSlider, QskSliderSkinlet >();
    declareSkinlet< QskSpinBox, QskSpinBoxSkinlet >();
    declareSkinlet< QskStatusIndicator, QskStatusIndicatorSkinlet >();
    declareSkinlet< QskSubWindow, QskSubWindowSkinlet >();
    declareSkinlet< QskSubWindowArea, QskSubWindowAreaSkinlet >();
    declareSkinlet< QskSwitchButton, QskSwitchButtonSkinlet >();
    declareSkinlet< QskTabButton, QskTabButtonSkinlet >();
    declareSkinlet< QskTabView, QskTabViewSkinlet >();
    declareSkinlet< QskTextLabel, QskTextLabelSkinlet >();
    declareSkinlet< QskTextField, QskTextFieldSkinlet >();
    declareSkinlet< QskProgressBar, QskProgressBarSkinlet >();
    declareSkinlet< QskProgressRing, QskProgressRingSkinlet >();
    declareSkinlet< QskRadioBox, QskRadioBoxSkinlet >();

    const QFont font = QGuiApplication::font();
    setupFontTable( font.family(), font.italic() );

    const auto noMargins = QVariant::fromValue( QskMargins( 0 ) );

    {
        // some defaults
        const auto aspect = QskAspect::NoSubcontrol | QskAspect::Metric;

        setSkinHint( aspect | QskAspect::Margin, noMargins );
        setSkinHint( aspect | QskAspect::Padding, noMargins );
        setSkinHint( aspect | QskAspect::Spacing, 0 );
    }

    setSkinHint( QskControl::Background | QskAspect::Metric | QskAspect::Padding, noMargins );

    setSkinHint( QskControl::Background | QskAspect::Color,
        QVariant::fromValue( QskGradient() ) );
}

QskSkin::~QskSkin()
{
}

QskSkin::ColorScheme QskSkin::colorScheme() const
{
    if ( m_data->colorScheme < 0 )
        return QskSkin::UnknownScheme;

    return static_cast< QskSkin::ColorScheme >( m_data->colorScheme );
}

void QskSkin::setColorScheme( ColorScheme colorScheme )
{
    if ( colorScheme == m_data->colorScheme )
        return;

    m_data->colorScheme = colorScheme;

    const auto transitionHint = qskSkinManager->transitionHint();
    if ( transitionHint.isValid() )
    {
        QskSkinTransition transition;
        transition.setMask( QskSkinTransition::Color );
        transition.setSourceSkin( this );

        clearHints();
        initHints();

        transition.setTargetSkin( this );
        transition.run( transitionHint );
    }
    else
    {
        clearHints();
        initHints();
    }

    Q_EMIT colorSchemeChanged( colorScheme );
}

void QskSkin::setSkinHint( QskAspect aspect, const QVariant& skinHint )
{
    m_data->hintTable.setHint( aspect, skinHint );
}

const QVariant& QskSkin::skinHint( QskAspect aspect ) const
{
    return m_data->hintTable.hint( aspect );
}

void QskSkin::declareSkinlet( const QMetaObject* metaObject,
    const QMetaObject* skinletMetaObject )
{
    Q_ASSERT( skinletMetaObject->constructorCount() );

    const auto it = m_data->skinletMap.find( metaObject );

    if ( it != m_data->skinletMap.end() )
    {
        auto& entry = it.value();
        if ( entry.metaObject != skinletMetaObject )
        {
            entry.metaObject = skinletMetaObject;

            delete entry.skinlet;
            entry.skinlet = nullptr;
        }
    }
    else
    {
        m_data->skinletMap.insert( metaObject, skinletMetaObject );
    }
}

static inline void qskSetFont( QskSkin* skin,
    QskFontRole::Category category, QFont font, qreal pointSize )
{
    font.setPointSize( pointSize );
    skin->setFont( { category, QskFontRole::Normal }, font );
}

void QskSkin::setupFontTable( const QString& family, bool italic )
{
    QFont font( family );
    font.setItalic( italic );
    font.setWeight( QFont::Normal );

    qskSetFont( this, QskFontRole::Caption, font, 8 );
    qskSetFont( this, QskFontRole::Subtitle, font, 10 );
    qskSetFont( this, QskFontRole::Body, font, 12 );
    qskSetFont( this, QskFontRole::Title, font, 20 );
    qskSetFont( this, QskFontRole::Headline, font, 30 );
    qskSetFont( this, QskFontRole::Display, font, 48 );

    completeFontTable();
}

void QskSkin::completeFontTable()
{
    // varying the weight of QskFontRole::Normal
        
    for ( int i = QskFontRole::Caption; i <= QskFontRole::Display; i++ )
    {
        auto& table = m_data->fonts;

        const auto category = static_cast< QskFontRole::Category >( i );

        const auto it = table.constFind( { category, QskFontRole::Normal } );
        if ( it == table.constEnd() )
            continue;

        const auto normalFont = it.value();

        for ( int j = QskFontRole::VeryLow; j <= QskFontRole::VeryHigh; j++ )
        {
            const auto emphasis = static_cast< QskFontRole::Emphasis >( j );

            if ( emphasis == QskFontRole::Normal
                || table.contains( { category, emphasis } ) )
            {
                continue;
            }

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
            const auto step = 10; // weight: [0-99]
#else
            const auto step = 100; // weight: [1-1000]
#endif

            int weight = normalFont.weight() + ( j - 2 ) * step;
            weight = qBound( static_cast<int>( QFont::Thin ),
                weight, static_cast<int>( QFont::Black ) );

            auto font = normalFont;
            font.setWeight( static_cast< QFont::Weight >( weight ) );

            m_data->fonts[ { category, emphasis } ] = font;
        }
    }
}

void QskSkin::setFont( const QskFontRole& fontRole, const QFont& font )
{
    m_data->fonts[ fontRole ] = font;
}

void QskSkin::resetFont( const QskFontRole& fontRole )
{
    m_data->fonts.remove( fontRole );
}

QFont QskSkin::font( const QskFontRole& fontRole ) const
{
    return qskResolvedFont( m_data->fonts, fontRole );
}

void QskSkin::setGraphicFilter( int graphicRole, const QskColorFilter& colorFilter )
{
    m_data->graphicFilters[ graphicRole ] = colorFilter;
}

void QskSkin::resetGraphicFilter( int graphicRole )
{
    m_data->graphicFilters.remove( graphicRole );
}

QskColorFilter QskSkin::graphicFilter( int graphicRole ) const
{
    return m_data->graphicFilters.value( graphicRole );
}

const QskSkinHintTable& QskSkin::hintTable() const
{
    return m_data->hintTable;
}

QskSkinHintTable& QskSkin::hintTable()
{
    return m_data->hintTable;
}

const QHash< QskFontRole, QFont >& QskSkin::fontTable() const
{
    return m_data->fonts;
}

const QHash< int, QskColorFilter >& QskSkin::graphicFilters() const
{
    return m_data->graphicFilters;
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

void QskSkin::clearHints()
{
    m_data->hintTable.clear();
    m_data->fonts.clear();
    m_data->graphicFilters.clear();
    m_data->graphicProviders.clear();
}

QString QskSkin::dialogButtonText( int action ) const
{
    const auto theme = qskPlatformTheme();

    auto text = theme->standardButtonText( action );
    text = QPlatformTheme::removeMnemonics( text );

    return text;
}

const int* QskSkin::dialogButtonLayout( Qt::Orientation orientation ) const
{
    // auto policy = QPlatformDialogHelper::UnknownLayout;
    auto policy = QPlatformDialogHelper::WinLayout;

    if ( const auto theme = qskPlatformTheme() )
    {
        const QVariant v = theme->themeHint( QPlatformTheme::DialogButtonBoxLayout );
        policy = static_cast< QPlatformDialogHelper::ButtonLayout >( v.toInt() );
    }

    return QPlatformDialogHelper::buttonLayout( orientation, policy );
}

const QMetaObject* QskSkin::skinletMetaObject( const QMetaObject* metaObject ) const
{
    while ( metaObject )
    {
        auto it = m_data->skinletMap.constFind( metaObject );
        if ( it != m_data->skinletMap.constEnd() )
            return it.value().metaObject;

        metaObject = metaObject->superClass();
    }

    return nullptr;
}

QskSkinlet* QskSkin::skinlet( const QMetaObject* metaObject )
{
    while ( metaObject )
    {
        auto it = m_data->skinletMap.find( metaObject );
        if ( it != m_data->skinletMap.end() )
        {
            auto& entry = it.value();

            if ( entry.skinlet == nullptr )
                entry.skinlet = qskNewSkinlet( entry.metaObject, this );

            return entry.skinlet;
        }

        metaObject = metaObject->superClass();
    }

    static QskSkinlet defaultSkinlet;
    return &defaultSkinlet;
}

#include "moc_QskSkin.cpp"
