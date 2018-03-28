/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskVirtualKeyboard.h"

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
        using Row = QskVirtualKeyboard::KeyData[ QskVirtualKeyboard::KeyCount ];
        Row data[ QskVirtualKeyboard::RowCount ];

        int indexOf( const QskVirtualKeyboard::KeyData* value ) const
        {
            return int( intptr_t( value - data[0] ) );
        }
    };
}

struct QskInputPanelLayouts
{
    struct KeyCodes
    {
        using Row = Qt::Key[ QskVirtualKeyboard::KeyCount ];
        Row data[ QskVirtualKeyboard::RowCount ];
    };

    using Layout = KeyCodes[ QskVirtualKeyboard::ModeCount ];

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

static const int KeyLocked =  static_cast< int >( Qt::ControlModifier );
static const int KeyStates =  static_cast< int >( Qt::KeyboardModifierMask );

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

static qreal qskRowStretch( const QskVirtualKeyboard::KeyRow& keyRow )
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
        stretch = QskVirtualKeyboard::KeyCount;
    }

    return stretch;
}

static bool qskIsAutorepeat( int key )
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

QSK_SUBCONTROL( QskVirtualKeyboard, Panel )

QSK_SUBCONTROL( QskVirtualKeyboardButton, Panel )
QSK_SUBCONTROL( QskVirtualKeyboardButton, Text )
QSK_SUBCONTROL( QskVirtualKeyboardButton, TextCancelButton )

QskVirtualKeyboardButton::QskVirtualKeyboardButton( int keyIndex, QskVirtualKeyboard* inputPanel, QQuickItem* parent ) :
    Inherited( parent ),
    m_keyIndex( keyIndex ),
    m_inputPanel( inputPanel )
{
    QskTextOptions options;
    options.setFontSizeMode( QskTextOptions::VerticalFit );
    setTextOptions( options );

    setFocusPolicy( Qt::TabFocus );

    auto keyData = m_inputPanel->keyDataAt( m_keyIndex );
    const auto key = keyData.key & ~KeyStates;

    if ( qskIsAutorepeat( key ) )
    {
        setAutoRepeat( true );
        setAutoRepeatDelay( 500 );
        setAutoRepeatInterval( 1000 / QGuiApplication::styleHints()->keyboardAutoRepeatRate() );
    }

    updateText();

    connect( this, &QskVirtualKeyboardButton::pressed, this, [ this ]()
    {
        m_inputPanel->handleKey( m_keyIndex );
    } );

    connect( m_inputPanel, &QskVirtualKeyboard::modeChanged, this, &QskVirtualKeyboardButton::updateText );
}

QskAspect::Subcontrol QskVirtualKeyboardButton::effectiveSubcontrol( QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskPushButton::Panel )
    {
        return QskVirtualKeyboardButton::Panel;
    }

    if( subControl == QskPushButton::Text )
    {
        // ### we could also introduce a state to not always query the button
        return isCancelButton() ? QskVirtualKeyboardButton::TextCancelButton : QskVirtualKeyboardButton::Text;
    }

    return subControl;
}

int QskVirtualKeyboardButton::keyIndex() const
{
    return m_keyIndex;
}

void QskVirtualKeyboardButton::updateText()
{
    QString text = m_inputPanel->currentTextForKeyIndex( m_keyIndex );

    if( text.count() == 1 && text.at( 0 ) == QChar( 0 ) )
    {
        setVisible( false );
    }
    else
    {
        setVisible( true );
        setText( text );
    }
}

bool QskVirtualKeyboardButton::isCancelButton() const
{
    auto keyData = m_inputPanel->keyDataAt( m_keyIndex );
    bool isCancel = ( keyData.key == 0x2716 );
    return isCancel;
}

class QskVirtualKeyboard::PrivateData
{
    public:
        PrivateData():
            currentLayout( nullptr ),
            mode( QskVirtualKeyboard::LowercaseMode ),
            selectedGroup( -1 ),
            candidateOffset( 0 ),
            buttonsBox( nullptr ),
            isUIInitialized( false )
        {
        }

    public:
        const QskInputPanelLayouts::Layout* currentLayout;
        QskVirtualKeyboard::Mode mode;

        qint16 selectedGroup;
        qint32 candidateOffset;

        QLocale locale;

        QVector< Qt::Key > groups;
        QVector< Qt::Key > candidates;

        std::unordered_map< int, KeyCounter > activeKeys;
        KeyTable keyTable[ ModeCount ];

        QskLinearBox* buttonsBox;
        QList< QskVirtualKeyboardButton* > keyButtons;
        bool isUIInitialized;
};

QskVirtualKeyboard::QskVirtualKeyboard( QQuickItem* parent ):
    Inherited( parent ),
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
                      this, &QskVirtualKeyboard::updateLocale );

    setFlag( ItemIsFocusScope, true );

#if 0
    setTabFence( true );
#endif

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
            QskVirtualKeyboardButton* button = new QskVirtualKeyboardButton( keyIndex, this, rowBox );
            rowBox->setRetainSizeWhenHidden( button, true );

            m_data->keyButtons.append( button );
        }
    }

    connect( this, &QskVirtualKeyboard::modeChanged, this, [ this ]() {
        updateLayout();
    });
}

QskVirtualKeyboard::~QskVirtualKeyboard()
{
}


QskAspect::Subcontrol QskVirtualKeyboard::effectiveSubcontrol( QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskBox::Panel )
    {
        return QskVirtualKeyboard::Panel;
    }

    return subControl;
}


QskVirtualKeyboard::Mode QskVirtualKeyboard::mode() const
{
    return m_data->mode;
}

const QskVirtualKeyboard::KeyDataSet& QskVirtualKeyboard::keyData( Mode mode ) const
{
    mode = mode == CurrentMode ? m_data->mode : mode;
    Q_ASSERT( mode >= 0 && mode < ModeCount );
    return m_data->keyTable[ mode ].data;
}

QString QskVirtualKeyboard::textForKey( int key ) const
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

QString QskVirtualKeyboard::displayLanguageName() const
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

void QskVirtualKeyboard::setPreeditGroups( const QVector< Qt::Key >& groups )
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

void QskVirtualKeyboard::setPreeditCandidates( const QVector< Qt::Key >& candidates )
{
    if( m_data->candidates == candidates )
    {
        return;
    }

    m_data->candidates = candidates;
    setCandidateOffset( 0 );
}

void QskVirtualKeyboard::setCandidateOffset( int candidateOffset )
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

void QskVirtualKeyboard::registerCompositionModelForLocale( const QLocale& locale,
                                                       QskInputCompositionModel* model )
{
    Q_EMIT inputMethodRegistered( locale, model );
}

void QskVirtualKeyboard::geometryChanged(
    const QRectF& newGeometry, const QRectF& oldGeometry )
{
    Inherited::geometryChanged( newGeometry, oldGeometry );

    Q_EMIT keyboardRectChanged();
}

void QskVirtualKeyboard::updateLayout()
{
    if( geometry().isNull() )
        return; // no need to calculate anything, will be called again

    QRectF rect = layoutRect();
    qreal verticalSpacing = m_data->buttonsBox->spacing();

    const auto& children = m_data->buttonsBox->childItems();
    for( auto rowItem : children )
    {
        auto rowBox = qobject_cast< QskLinearBox* >( rowItem );
        const qreal horizontalSpacing = rowBox->spacing();

        const auto& rowChildren = rowBox->childItems();
        for( auto keyItem : rowChildren )
        {
            auto button = qobject_cast< QskVirtualKeyboardButton* >( keyItem );
            QRectF keyRect = keyDataAt( button->keyIndex() ).rect;
            qreal width = keyRect.width() * rect.width() - horizontalSpacing;
            qreal height = keyRect.height() * rect.height() - verticalSpacing;

            button->setFixedSize( width, height );
        }
    }
}

void QskVirtualKeyboard::updateUI()
{
    for( QskVirtualKeyboardButton* button : qskAsConst( m_data->keyButtons ) )
    {
        button->updateText();
    }
}

QskVirtualKeyboard::KeyData& QskVirtualKeyboard::keyDataAt( int keyIndex ) const
{
    const auto row = keyIndex / KeyCount;
    const auto col = keyIndex % KeyCount;
    return m_data->keyTable[ m_data->mode ].data[ row ][ col ];
}

void QskVirtualKeyboard::handleKey( int keyIndex )
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
            setMode( static_cast< QskVirtualKeyboard::Mode >(
                         m_data->mode ? ( ( m_data->mode + 1 ) % QskVirtualKeyboard::ModeCount )
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

QString QskVirtualKeyboard::currentTextForKeyIndex( int keyIndex ) const
{
    auto keyData = keyDataAt( keyIndex );
    QString text = textForKey( keyData.key );
    return text;
}

void QskVirtualKeyboard::compose( int key )
{
    QGuiApplication::inputMethod()->invokeAction(
        static_cast< QInputMethod::Action >( Compose ), key );
}

void QskVirtualKeyboard::selectGroup( int index )
{
    auto& topRow = m_data->keyTable[ m_data->mode ].data[ 0 ];

    if( m_data->selectedGroup >= 0 )
    {
        auto group = static_cast< int >( m_data->selectedGroup );
        topRow[ group ].key &= ~KeyLocked;
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

void QskVirtualKeyboard::selectCandidate( int index )
{
    QGuiApplication::inputMethod()->invokeAction(
        static_cast< QInputMethod::Action >( SelectCandidate ), index );
}

void QskVirtualKeyboard::updateLocale( const QLocale& locale )
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

void QskVirtualKeyboard::updateKeyData()
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

void QskVirtualKeyboard::setMode( QskVirtualKeyboard::Mode mode )
{
    m_data->mode = mode;
    Q_EMIT modeChanged( m_data->mode );
}

#include "moc_QskVirtualKeyboard.cpp"
