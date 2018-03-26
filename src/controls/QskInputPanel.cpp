/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputPanel.h"

#include "QskAspect.h"

#include <QskPushButton.h>
#include <QskTextLabel.h>
#include <QskDialog.h>
#include <QGuiApplication>
#include <QStyleHints>
#include <QskLinearBox.h>
#include <QskTextOptions.h>

#include <QTimer>

#include <cmath>
#include <unordered_map>

namespace
{
    struct KeyTable
    {
        using Row = QskInputPanel::KeyData[ QskInputPanel::KeyCount ];
        Row data[ QskInputPanel::RowCount ];

        int indexOf( const QskInputPanel::KeyData* value ) const
        {
            return int( intptr_t( value - data[0] ) );
        }
    };
}

struct QskInputPanelLayouts
{
    struct KeyCodes
    {
        using Row = Qt::Key[ QskInputPanel::KeyCount ];
        Row data[ QskInputPanel::RowCount ];
    };

    using Layout = KeyCodes[ QskInputPanel::ModeCount ];

    Layout bg; // Bulgarian
    Layout cs; // Czech
    Layout de; // German
    Layout da; // Danish
    Layout el; // Greek
    Layout en_GB; // English (GB)
    Layout en_US; // English (US)
    Layout es; // Spanish
    Layout fi; // Finnish
    Layout fr; // French
    Layout hu; // Hungarian
    Layout it; // Italian
    Layout ja; // Japanese
    Layout lv; // Latvian
    Layout lt; // Lithuanian
    Layout nl; // Dutch
    Layout pt; // Portuguese
    Layout ro; // Romanian
    Layout ru; // Russian
    Layout sl; // Slovene
    Layout sk; // Slovak
    Layout tr; // Turkish
    Layout zh; // Chinese

    Q_GADGET
};

#define LOWER(x) Qt::Key(x + 32) // Convert an uppercase key to lowercase
static constexpr const QskInputPanelLayouts qskInputPanelLayouts =
{
#include "QskInputPanelLayouts.cpp"
};
#undef LOWER

QSK_DECLARE_OPERATORS_FOR_FLAGS( Qt::Key ) // Must appear after the LOWER macro

static const Qt::Key KeyLocked =  static_cast< Qt::Key >( Qt::ControlModifier );
static const Qt::Key KeyStates =  static_cast< Qt::Key >( Qt::KeyboardModifierMask );

static qreal qskKeyStretch( Qt::Key key )
{
    switch( key )
    {
        case Qt::Key_Backspace:
        case Qt::Key_Shift:
        case Qt::Key_CapsLock:
            return 1.5;

        case Qt::Key_Space:
            return 3.5;

        case Qt::Key_Return:
        case Qt::Key_Mode_switch:

        // Possibly smaller
        default:
            break;
    }

    return 1.0;
}

static qreal qskRowStretch( const QskInputPanel::KeyRow& keyRow )
{
    qreal stretch = 0;

    for( const auto& key : keyRow )
    {
        if( !key )
        {
            continue;
        }

        stretch += qskKeyStretch( key );
    }

    if( stretch == 0.0 )
    {
        stretch = QskInputPanel::KeyCount;
    }

    return stretch;
}

static bool qskIsAutorepeat( Qt::Key key )
{
    return ( key != Qt::Key_Return && key != Qt::Key_Enter
            && key != Qt::Key_Shift && key!= Qt::Key_CapsLock
            && key != Qt::Key_Mode_switch );

}

namespace
{
    struct KeyCounter
    {
        int keyIndex;
        int count;
    };
}

QSK_SUBCONTROL( QskInputPanel, Panel )

QSK_SUBCONTROL( QskKeyButton, Panel )
QSK_SUBCONTROL( QskKeyButton, Text )
QSK_SUBCONTROL( QskKeyButton, TextCancelButton )

QskKeyButton::QskKeyButton( int keyIndex, QskInputPanel* inputPanel, QQuickItem* parent ) :
    Inherited( parent ),
    m_keyIndex( keyIndex ),
    m_inputPanel( inputPanel )
{
    QskTextOptions options;
    options.setFontSizeMode( QskTextOptions::VerticalFit );
    setTextOptions( options );

    auto keyData = m_inputPanel->keyDataAt( m_keyIndex );
    const auto key = keyData.key & ~KeyStates;

    if ( qskIsAutorepeat( key ) )
    {
        setAutoRepeat( true );
        setAutoRepeatDelay( 500 );
        setAutoRepeatInterval( 1000 / QGuiApplication::styleHints()->keyboardAutoRepeatRate() );
    }

    updateText();

    connect( this, &QskKeyButton::pressed, this, [ this ]()
    {
        m_inputPanel->handleKey( m_keyIndex );
    } );

    connect( m_inputPanel, &QskInputPanel::modeChanged, this, &QskKeyButton::updateText );
}

QskAspect::Subcontrol QskKeyButton::effectiveSubcontrol( QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskPushButton::Panel )
    {
        return QskKeyButton::Panel;
    }

    if( subControl == QskPushButton::Text )
    {
        // ### we could also introduce a state to not always query the button
        return isCancelButton() ? QskKeyButton::TextCancelButton : QskKeyButton::Text;
    }

    return subControl;
}

int QskKeyButton::keyIndex() const
{
    return m_keyIndex;
}

void QskKeyButton::updateText()
{
    QString text = m_inputPanel->currentTextForKeyIndex( m_keyIndex );

    if( text.count() == 1 && text.at( 0 ) == Qt::Key_unknown )
    {
        setText( QStringLiteral( "" ) );
    }
    else
    {
        setText( text );
    }
}

bool QskKeyButton::isCancelButton() const
{
    auto keyData = m_inputPanel->keyDataAt( m_keyIndex );
    bool isCancel = ( keyData.key == 0x2716 );
    return isCancel;
}

class QskInputPanel::PrivateData
{
    public:
        PrivateData():
            currentLayout( nullptr ),
            mode( QskInputPanel::LowercaseMode ),
            selectedGroup( -1 ),
            candidateOffset( 0 ),
            buttonsBox( nullptr ),
            isUIInitialized( false )
        {
        }

    public:
        const QskInputPanelLayouts::Layout* currentLayout;
        QskInputPanel::Mode mode;

        qint16 selectedGroup;
        qint32 candidateOffset;

        QLocale locale;

        QVector< Qt::Key > groups;
        QVector< Qt::Key > candidates;

        std::unordered_map< int, KeyCounter > activeKeys;
        KeyTable keyTable[ ModeCount ];

        QskLinearBox* buttonsBox;
        QList< QskKeyButton* > keyButtons;
        bool isUIInitialized;
};

QskInputPanel::QskInputPanel( QQuickItem* parent ):
    QskControl( parent ),
    m_data( new PrivateData )
{
    qRegisterMetaType< Qt::Key >();

    setFlag( ItemHasContents );
    setAcceptedMouseButtons( Qt::LeftButton );

    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );

    setAutoFillBackground( true );

    auto margins = marginsHint( Panel | QskAspect::Margin );
    setMargins( margins );

    updateLocale( locale() );

    QObject::connect( this, &QskControl::localeChanged,
                      this, &QskInputPanel::updateLocale );

    setFlag( ItemIsFocusScope, true );

    // ### for some reason we never get focus when this is a tab fence:
//    setTabFence( true );

    setAutoLayoutChildren( true );

    auto& panelKeyData = keyData();

    m_data->buttonsBox = new QskLinearBox( Qt::Vertical, this );
    m_data->buttonsBox->setAutoAddChildren( true );

    for( const auto& keyRow : panelKeyData )
    {
        QskLinearBox* rowBox = new QskLinearBox( Qt::Horizontal, m_data->buttonsBox );
        rowBox->setAutoAddChildren( true );

        for( const auto& keyData : keyRow )
        {
            if( !keyData.key )
            {
                continue;
            }

            int keyIndex = m_data->keyTable[ m_data->mode ].indexOf( &keyData );
            QskKeyButton* button = new QskKeyButton( keyIndex, this, rowBox );

            m_data->keyButtons.append( button );
        }
    }
}

QskInputPanel::~QskInputPanel()
{
}


QskAspect::Subcontrol QskInputPanel::effectiveSubcontrol( QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskAspect::Control )
    {
        return QskInputPanel::Panel;
    }

    return subControl;
}


QskInputPanel::Mode QskInputPanel::mode() const
{
    return m_data->mode;
}

const QskInputPanel::KeyDataSet& QskInputPanel::keyData( Mode mode ) const
{
    mode = mode == CurrentMode ? m_data->mode : mode;
    Q_ASSERT( mode >= 0 && mode < ModeCount );
    return m_data->keyTable[ mode ].data;
}

QString QskInputPanel::textForKey( Qt::Key key ) const
{
    key &= ~KeyStates;

    // Special cases
    switch( key )
    {
        case Qt::Key_Backspace:
        case Qt::Key_Muhenkan:
            return QChar( 0x232B );

        case Qt::Key_CapsLock:
        case Qt::Key_Kana_Lock:
            return QChar( 0x21E7 );

        case Qt::Key_Shift:
        case Qt::Key_Kana_Shift:
            return QChar( 0x2B06 );

        case Qt::Key_Mode_switch:
            return QChar( 0x2026 );

        case Qt::Key_Return:
        case Qt::Key_Kanji:
            return QChar( 0x23CE );

        case Qt::Key_Left:
            return QChar( 0x2190 );

        case Qt::Key_Right:
            return QChar( 0x2192 );

        case Qt::Key_ApplicationLeft:
            return QChar( 0x2B05 );

        case Qt::Key_ApplicationRight:
            return QChar( 0x27A1 );

        default:
            break;
    }

    // TODO: possibly route through locale for custom strings

    // Default to direct key mapping
    return QChar( key );
}

QString QskInputPanel::displayLanguageName() const
{
    const auto locale = this->locale();

    switch( locale.language() )
    {
        case QLocale::Bulgarian:
            return QStringLiteral( "български език" );

        case QLocale::Czech:
            return QStringLiteral( "Čeština" );

        case QLocale::German:
            return QStringLiteral( "Deutsch" );

        case QLocale::Danish:
            return QStringLiteral( "Dansk" );

        case QLocale::Greek:
            return QStringLiteral( "Eλληνικά" );

        case QLocale::English:
            {
                switch( locale.country() )
                {
                    case QLocale::Canada:
                    case QLocale::UnitedStates:
                    case QLocale::UnitedStatesMinorOutlyingIslands:
                    case QLocale::UnitedStatesVirginIslands:
                        return QStringLiteral( "English (US)" );

                    default:
                        return QStringLiteral( "English (UK)" );
                }

                break;
            }

        case QLocale::Spanish:
            return QStringLiteral( "Español" );

        case QLocale::Finnish:
            return QStringLiteral( "Suomi" );

        case QLocale::French:
            return QStringLiteral( "Français" );

        case QLocale::Hungarian:
            return QStringLiteral( "Magyar" );

        case QLocale::Italian:
            return QStringLiteral( "Italiano" );

        case QLocale::Japanese:
            return QStringLiteral( "日本語" );

        case QLocale::Latvian:
            return QStringLiteral( "Latviešu" );

        case QLocale::Lithuanian:
            return QStringLiteral( "Lietuvių" );

        case QLocale::Dutch:
            return QStringLiteral( "Nederlands" );

        case QLocale::Portuguese:
            return QStringLiteral( "Português" );

        case QLocale::Romanian:
            return QStringLiteral( "Română" );

        case QLocale::Russia:
            return QStringLiteral( "Русский" );

        case QLocale::Slovenian:
            return QStringLiteral( "Slovenščina" );

        case QLocale::Slovak:
            return QStringLiteral( "Slovenčina" );

        case QLocale::Turkish:
            return QStringLiteral( "Türkçe" );

        case QLocale::Chinese:
            return QStringLiteral( "中文" );

        default:
            break;
    }

    return QLocale::languageToString( locale.language() );
}

void QskInputPanel::setPreeditGroups( const QVector< Qt::Key >& groups )
{
    auto& topRow = m_data->keyTable[ LowercaseMode ].data[ 0 ];

    for( const auto& group : groups )
    {
        auto& keyData = topRow[ &group - groups.data() ];
        keyData.key = group;
    }

    m_data->groups = groups;
    selectGroup( -1 );

    if( m_data->mode == LowercaseMode )
    {
        update();
    }
}

void QskInputPanel::setPreeditCandidates( const QVector< Qt::Key >& candidates )
{
    if( m_data->candidates == candidates )
    {
        return;
    }

    m_data->candidates = candidates;
    setCandidateOffset( 0 );
}

void QskInputPanel::setCandidateOffset( int candidateOffset )
{
    m_data->candidateOffset = candidateOffset;

    auto& topRow = m_data->keyTable[ LowercaseMode ].data[ 0 ];

    const auto groupCount = m_data->groups.length();
    const auto candidateCount = m_data->candidates.length();
    const auto count = std::min( candidateCount, KeyCount - groupCount );
    const bool continueLeft = m_data->candidateOffset > 0;
    const bool continueRight = ( candidateCount - m_data->candidateOffset ) > count;

    for( int i = 0; i < count; ++i )
    {
        auto& keyData = topRow[ i + groupCount ];

        if( continueLeft && i == 0 )
        {
            keyData.key = Qt::Key_ApplicationLeft;
        }
        else if( continueRight && ( i == KeyCount - groupCount - 1 ) )
        {
            keyData.key = Qt::Key_ApplicationRight;
        }
        else
        {
            keyData.isSuggestionKey = true; // ### reset when switching layouts etc.!
            keyData.key = m_data->candidates.at( i + m_data->candidateOffset );
        }
    }

    for( int i = count; i < KeyCount - groupCount; ++i )
    {
        auto& keyData = topRow[ i + groupCount ];
        keyData.key = Qt::Key_unknown;
    }

    if( m_data->mode == LowercaseMode )
    {
        updateUI();
    }
}

QRectF QskInputPanel::keyboardRect() const
{
    auto keyboardRect = rect(); // ### margins? would eliminate this thing below
    //    if ( QskDialog::instance()->policy() != QskDialog::TopLevelWindow )
    //        keyboardRect.adjust( 0, keyboardRect.height() * 0.5, 0, 0 );
    return keyboardRect;
}

void QskInputPanel::registerCompositionModelForLocale( const QLocale& locale,
                                                       QskInputCompositionModel* model )
{
    Q_EMIT inputMethodRegistered( locale, model );
}

void QskInputPanel::geometryChanged(
    const QRectF& newGeometry, const QRectF& oldGeometry )
{
    Inherited::geometryChanged( newGeometry, oldGeometry );

    Q_EMIT keyboardRectChanged();
}

void QskInputPanel::updateLayout()
{
    if( geometry().isNull() )
        return; // no need to calculate anything, will be called again

    QMarginsF margins = marginsHint( Panel | QskAspect::Margin );
    QRectF rect = keyboardRect().marginsRemoved( margins );
    qreal verticalSpacing = m_data->buttonsBox->spacing();

    for( QQuickItem* rowItem : m_data->buttonsBox->childItems() )
    {
        QskLinearBox* rowBox = qobject_cast< QskLinearBox* >( rowItem );
        qreal horizontalSpacing = rowBox->spacing();

        for( QQuickItem* keyItem : rowBox->childItems() )
        {
            QskKeyButton* button = qobject_cast< QskKeyButton* >( keyItem );
            QRectF keyRect = keyDataAt( button->keyIndex() ).rect;
            qreal width = keyRect.width() * rect.width() - horizontalSpacing;
            qreal height = keyRect.height() * rect.height() - verticalSpacing;

            button->setFixedSize( width, height );
        }
    }
}

void QskInputPanel::updateUI()
{
    for( QskKeyButton* button : qskAsConst( m_data->keyButtons ) )
    {
        button->updateText();
    }
}

QskInputPanel::KeyData& QskInputPanel::keyDataAt( int keyIndex ) const
{
    const auto row = keyIndex / KeyCount;
    const auto col = keyIndex % KeyCount;
    return m_data->keyTable[ m_data->mode ].data[ row ][ col ];
}

void QskInputPanel::handleKey( int keyIndex )
{
    KeyData keyData = keyDataAt( keyIndex );
    const auto key = keyData.key & ~KeyStates;

    // Preedit keys
    const auto row = keyIndex / KeyCount;
    const auto column = keyIndex % KeyCount;

    if( m_data->mode == LowercaseMode && !m_data->groups.isEmpty() && row == 0 )
    {
        if( key == Qt::Key_ApplicationLeft
                || key == Qt::Key_ApplicationRight )
        {
            setCandidateOffset( m_data->candidateOffset
                                + ( key == Qt::Key_ApplicationLeft ? -1 : 1 ) );
            return;
        }

        const auto groupCount = m_data->groups.length();

        if( column < groupCount )
        {
            selectGroup( column );
        }
        else if( column < KeyCount )
        {
            selectCandidate( column - groupCount + m_data->candidateOffset );
        }
        else
        {
            Q_UNREACHABLE();    // Handle the final key...
        }

        return;
    }

    // Mode-switching keys
    switch( key )
    {
        case Qt::Key_CapsLock:
        case Qt::Key_Kana_Lock:
            setMode( UppercaseMode ); // Lock caps
            return;

        case Qt::Key_Shift:
        case Qt::Key_Kana_Shift:
            setMode( LowercaseMode ); // Unlock caps
            return;

        case Qt::Key_Mode_switch: // Cycle through modes, but skip caps
            setMode( static_cast< QskInputPanel::Mode >(
                         m_data->mode ? ( ( m_data->mode + 1 ) % QskInputPanel::ModeCount )
                         : SpecialCharacterMode ) );
            return;

        // This is (one of) the cancel symbol, not Qt::Key_Cancel:
        case Qt::Key( 10006 ):
            Q_EMIT cancelPressed();
            return;

        default:
            break;
    }

    if( keyData.isSuggestionKey )
    {
        selectCandidate( keyIndex );
    }
    else
    {
        compose( key );
    }
}

QString QskInputPanel::currentTextForKeyIndex( int keyIndex ) const
{
    auto keyData = keyDataAt( keyIndex );
    QString text = textForKey( keyData.key );
    return text;
}

void QskInputPanel::compose( Qt::Key key )
{
    QGuiApplication::inputMethod()->invokeAction(
        static_cast< QInputMethod::Action >( Compose ), key );
}

void QskInputPanel::selectGroup( int index )
{
    auto& topRow = m_data->keyTable[ m_data->mode ].data[ 0 ];

    if( m_data->selectedGroup >= 0 )
    {
        topRow[ m_data->selectedGroup ].key &= ~KeyLocked;
    }

    if( m_data->selectedGroup == index )
    {
        index = -1;    // clear selection
    }

    m_data->selectedGroup = index;
    QGuiApplication::inputMethod()->invokeAction(
        static_cast< QInputMethod::Action >( SelectGroup ), m_data->selectedGroup + 1 );

    if( m_data->selectedGroup < 0 )
    {
        return;
    }

    topRow[ m_data->selectedGroup ].key |= KeyLocked;
}

void QskInputPanel::selectCandidate( int index )
{
    QGuiApplication::inputMethod()->invokeAction(
        static_cast< QInputMethod::Action >( SelectCandidate ), index );
}

void QskInputPanel::updateLocale( const QLocale& locale )
{
    switch( locale.language() )
    {
        case QLocale::Bulgarian:
            m_data->currentLayout = &qskInputPanelLayouts.bg;
            break;

        case QLocale::Czech:
            m_data->currentLayout = &qskInputPanelLayouts.cs;
            break;

        case QLocale::German:
            m_data->currentLayout = &qskInputPanelLayouts.de;
            break;

        case QLocale::Danish:
            m_data->currentLayout = &qskInputPanelLayouts.da;
            break;

        case QLocale::Greek:
            m_data->currentLayout = &qskInputPanelLayouts.el;
            break;

        case QLocale::English:
            {
                switch( locale.country() )
                {
                    case QLocale::Canada:
                    case QLocale::UnitedStates:
                    case QLocale::UnitedStatesMinorOutlyingIslands:
                    case QLocale::UnitedStatesVirginIslands:
                        m_data->currentLayout = &qskInputPanelLayouts.en_US;
                        break;

                    default:
                        m_data->currentLayout = &qskInputPanelLayouts.en_GB;
                        break;
                }

                break;
            }

        case QLocale::Spanish:
            m_data->currentLayout = &qskInputPanelLayouts.es;
            break;

        case QLocale::Finnish:
            m_data->currentLayout = &qskInputPanelLayouts.fi;
            break;

        case QLocale::French:
            m_data->currentLayout = &qskInputPanelLayouts.fr;
            break;

        case QLocale::Hungarian:
            m_data->currentLayout = &qskInputPanelLayouts.hu;
            break;

        case QLocale::Italian:
            m_data->currentLayout = &qskInputPanelLayouts.it;
            break;

        case QLocale::Japanese:
            m_data->currentLayout = &qskInputPanelLayouts.ja;
            break;

        case QLocale::Latvian:
            m_data->currentLayout = &qskInputPanelLayouts.lv;
            break;

        case QLocale::Lithuanian:
            m_data->currentLayout = &qskInputPanelLayouts.lt;
            break;

        case QLocale::Dutch:
            m_data->currentLayout = &qskInputPanelLayouts.nl;
            break;

        case QLocale::Portuguese:
            m_data->currentLayout = &qskInputPanelLayouts.pt;
            break;

        case QLocale::Romanian:
            m_data->currentLayout = &qskInputPanelLayouts.ro;
            break;

        case QLocale::Russia:
            m_data->currentLayout = &qskInputPanelLayouts.ru;
            break;

        case QLocale::Slovenian:
            m_data->currentLayout = &qskInputPanelLayouts.sl;
            break;

        case QLocale::Slovak:
            m_data->currentLayout = &qskInputPanelLayouts.sk;
            break;

        case QLocale::Turkish:
            m_data->currentLayout = &qskInputPanelLayouts.tr;
            break;

        case QLocale::Chinese:
            m_data->currentLayout = &qskInputPanelLayouts.zh;
            break;

        default:
            qWarning() << "QskInputPanel: unsupported locale:" << locale;
            m_data->currentLayout = &qskInputPanelLayouts.en_US;
    }

    Q_EMIT displayLanguageNameChanged();

    updateKeyData();
    setMode( LowercaseMode );
}

void QskInputPanel::updateKeyData()
{
    // Key data is in normalized coordinates
    const auto keyHeight = 1.0f / RowCount;

    for( const auto& keyLayout : *m_data->currentLayout )
    {
        auto& keyDataLayout = m_data->keyTable[ &keyLayout - *m_data->currentLayout ];
        qreal yPos = 0;

        for( int i = 0; i < RowCount; i++ )
        {
            auto& row = keyLayout.data[i];
            auto& keyDataRow = keyDataLayout.data[ i ];

            const auto baseKeyWidth = 1.0 / qskRowStretch( row );

            qreal xPos = 0;
            qreal keyWidth = baseKeyWidth;

            for( const auto& key : row )
            {
                auto& keyData = keyDataRow[ &key - row ];
                keyData.key = key;

                keyWidth = baseKeyWidth * qskKeyStretch( key );
                keyData.rect = { xPos, yPos, keyWidth, keyHeight };
                xPos += keyWidth;
            }

            yPos += keyHeight;
        }
    }
}

void QskInputPanel::setMode( QskInputPanel::Mode mode )
{
    m_data->mode = mode;
    Q_EMIT modeChanged( m_data->mode );
}

#include "QskInputPanel.moc"
#include "moc_QskInputPanel.cpp"
