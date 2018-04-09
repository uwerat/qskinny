/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskVirtualKeyboard.h"
#include "QskPushButton.h"
#include "QskTextOptions.h"

#include <QGuiApplication>
#include <QStyleHints>

QSK_QT_PRIVATE_BEGIN
#include <private/qinputmethod_p.h>
QSK_QT_PRIVATE_END

#include <qpa/qplatformintegration.h>
#include <qpa/qplatforminputcontext.h>

namespace
{
    enum
    {
        RowCount = 5,
        KeyCount = 12
    };

    using KeyRow = Qt::Key[KeyCount];

    class Button final : public QskPushButton
    {
    public:
        Button( int row, int column, QQuickItem* parent ):
            QskPushButton( parent ),
            m_row( row ),
            m_column( column )
        {
            QskTextOptions options;
            options.setFontSizeMode( QskTextOptions::VerticalFit );
            setTextOptions( options );

            setFocusPolicy( Qt::TabFocus );
        }

        virtual QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override
        {
            if( subControl == QskPushButton::Panel )
                return QskVirtualKeyboard::ButtonPanel;

            if( subControl == QskPushButton::Text )
                return QskVirtualKeyboard::ButtonText;

            return subControl;
        }

        int row() const { return m_row; }
        int column() const { return m_column; }

    private:
        const int m_row;
        const int m_column;
    };
}

struct QskVirtualKeyboardLayouts
{
    struct KeyCodes
    {
        using Row = Qt::Key[ KeyCount ];
        Row data[ RowCount ];
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
};

#define LOWER(x) Qt::Key(x + 32) // Convert an uppercase key to lowercase
static constexpr const QskVirtualKeyboardLayouts qskKeyboardLayouts =
{
#include "QskVirtualKeyboardLayouts.cpp"
};
#undef LOWER

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

static qreal qskRowStretch( const KeyRow& keyRow )
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
        stretch = KeyCount;
    }

    return stretch;
}

static QString qskTextForKey( Qt::Key key )
{
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
            return QChar( key );
    }
}

static bool qskIsAutorepeat( int key )
{
    return ( key != Qt::Key_Return && key != Qt::Key_Enter
        && key != Qt::Key_Shift && key != Qt::Key_CapsLock
        && key != Qt::Key_Mode_switch );
}

static inline QQuickItem* qskInputItem()
{
    QPlatformInputContext* inputContext;
#if 1
    inputContext = QGuiApplicationPrivate::platformIntegration()->inputContext();
#else
    // for some reason the gcc sanitizer does not like this one
    inputContext = QInputMethodPrivate::get( inputMethod )->platformInputContext();
#endif

    QQuickItem* item = nullptr;

    QMetaObject::invokeMethod( inputContext, "inputItem",
        Qt::DirectConnection, Q_RETURN_ARG( QQuickItem*, item ) );

    return item;
}

QSK_SUBCONTROL( QskVirtualKeyboard, Panel )
QSK_SUBCONTROL( QskVirtualKeyboard, ButtonPanel )
QSK_SUBCONTROL( QskVirtualKeyboard, ButtonText )

class QskVirtualKeyboard::PrivateData
{
public:
    PrivateData():
        currentLayout( nullptr ),
        mode( QskVirtualKeyboard::LowercaseMode )
    {
    }

public:
    const QskVirtualKeyboardLayouts::Layout* currentLayout;
    QskVirtualKeyboard::Mode mode;

    QVector< Button* > keyButtons;
};

QskVirtualKeyboard::QskVirtualKeyboard( QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData )
{
    setFlag( ItemHasContents );
    setFlag( ItemIsFocusScope, true );
#if 0
    // TODO ...
    setTabFence( true );
#endif

    setPolishOnResize( true );
    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );

    m_data->keyButtons.reserve( RowCount * KeyCount );

    const auto autoRepeatInterval =
        1000 / QGuiApplication::styleHints()->keyboardAutoRepeatRate();

    for ( int row = 0; row < RowCount; row++ )
    {
        for ( int col = 0; col < KeyCount; col++ )
        {
            auto button = new Button( row, col, this );
            button->installEventFilter( this );

            button->setAutoRepeat( false );
            button->setAutoRepeatDelay( 500 );
            button->setAutoRepeatInterval( autoRepeatInterval );

            connect( button, &QskPushButton::pressed,
                this, &QskVirtualKeyboard::buttonPressed );

            m_data->keyButtons += button;
        }
    }

    connect( this, &QskControl::localeChanged,
        this, &QskVirtualKeyboard::updateLocale );

    updateLocale( locale() );
}

QskVirtualKeyboard::~QskVirtualKeyboard()
{
}

QskAspect::Subcontrol QskVirtualKeyboard::effectiveSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskBox::Panel )
        return QskVirtualKeyboard::Panel;

    return subControl;
}


QskVirtualKeyboard::Mode QskVirtualKeyboard::mode() const
{
    return m_data->mode;
}

void QskVirtualKeyboard::setPreeditCandidates( const QVector< QString >& )
{
#if 0
    m_suggestionBar->setCandidates( candidates );
#endif
}

void QskVirtualKeyboard::updateLayout()
{
    const auto r = layoutRect();
    if( r.isEmpty() )
        return;

    const auto spacing = metric( Panel | QskAspect::Spacing );
    const auto totalVSpacing = ( RowCount - 1 ) * spacing;

    const auto keyHeight = ( r.height() - totalVSpacing ) / RowCount; 

    const auto& keyCodes = ( *m_data->currentLayout )[ m_data->mode ];

    qreal yPos = r.top();

    for( int row = 0; row < RowCount; row++ )
    {
        const auto& keys = keyCodes.data[ row ];

#if 1
        // there should be a better way
        auto totalHSpacing = -spacing;
        if ( spacing )
        {
            for ( int col = 0; col < KeyCount; col++ )
            {
                if ( keys[ col ] != Qt::Key( 0 ) )
                    totalHSpacing += spacing;
            }
        }
#endif
        const auto baseKeyWidth = ( r.width() - totalHSpacing ) / qskRowStretch( keys );
        qreal xPos = r.left();

        for ( int col = 0; col < KeyCount; col++ )
        {
            const Qt::Key key = keys[ col ];
            auto button = m_data->keyButtons[ row * KeyCount + col ];

            button->setVisible( key != Qt::Key( 0 ) );

            if ( button->isVisible() )
            {
                const qreal keyWidth = baseKeyWidth * qskKeyStretch( key );

                const QRectF rect( xPos, yPos, keyWidth, keyHeight );

                button->setGeometry( rect );
                button->setAutoRepeat( qskIsAutorepeat( key ) );
                button->setText( qskTextForKey( key ) );

                xPos += keyWidth + spacing;
            }
        }

        yPos += keyHeight + spacing;
    }
}

void QskVirtualKeyboard::buttonPressed()
{
    const auto button = static_cast< const Button* >( sender() );
    if ( button == nullptr )
        return;

    const auto& keyCodes = ( *m_data->currentLayout )[ m_data->mode ];
    const Qt::Key key = keyCodes.data[ button->row() ][ button->column() ];

    // Mode-switching keys
    switch( key )
    {
        case Qt::Key_CapsLock:
        case Qt::Key_Kana_Lock:
        {
            setMode( UppercaseMode ); // Lock caps
            break;
        }

        case Qt::Key_Shift:
        case Qt::Key_Kana_Shift:
        {
            setMode( LowercaseMode ); // Unlock caps
            break;
        }

        case Qt::Key_Mode_switch: // Cycle through modes, but skip caps
        {
            setMode( static_cast< QskVirtualKeyboard::Mode >(
                m_data->mode ? ( ( m_data->mode + 1 ) % QskVirtualKeyboard::ModeCount )
                : SpecialCharacterMode ) );

            break;
        }
        default:
        {
            QGuiApplication::inputMethod()->invokeAction(
                static_cast< QInputMethod::Action >( Compose ), key );
        }
    }
}

void QskVirtualKeyboard::setCandidateBarVisible( bool )
{
}

void QskVirtualKeyboard::updateLocale( const QLocale& locale )
{
    switch( locale.language() )
    {
        case QLocale::Bulgarian:
            m_data->currentLayout = &qskKeyboardLayouts.bg;
            break;

        case QLocale::Czech:
            m_data->currentLayout = &qskKeyboardLayouts.cs;
            break;

        case QLocale::German:
            m_data->currentLayout = &qskKeyboardLayouts.de;
            break;

        case QLocale::Danish:
            m_data->currentLayout = &qskKeyboardLayouts.da;
            break;

        case QLocale::Greek:
            m_data->currentLayout = &qskKeyboardLayouts.el;
            break;

        case QLocale::English:
        {
            switch( locale.country() )
            {
                case QLocale::Canada:
                case QLocale::UnitedStates:
                case QLocale::UnitedStatesMinorOutlyingIslands:
                case QLocale::UnitedStatesVirginIslands:
                    m_data->currentLayout = &qskKeyboardLayouts.en_US;
                    break;

                default:
                    m_data->currentLayout = &qskKeyboardLayouts.en_GB;
                    break;
            }

            break;
        }

        case QLocale::Spanish:
            m_data->currentLayout = &qskKeyboardLayouts.es;
            break;

        case QLocale::Finnish:
            m_data->currentLayout = &qskKeyboardLayouts.fi;
            break;

        case QLocale::French:
            m_data->currentLayout = &qskKeyboardLayouts.fr;
            break;

        case QLocale::Hungarian:
            m_data->currentLayout = &qskKeyboardLayouts.hu;
            break;

        case QLocale::Italian:
            m_data->currentLayout = &qskKeyboardLayouts.it;
            break;

        case QLocale::Japanese:
            m_data->currentLayout = &qskKeyboardLayouts.ja;
            break;

        case QLocale::Latvian:
            m_data->currentLayout = &qskKeyboardLayouts.lv;
            break;

        case QLocale::Lithuanian:
            m_data->currentLayout = &qskKeyboardLayouts.lt;
            break;

        case QLocale::Dutch:
            m_data->currentLayout = &qskKeyboardLayouts.nl;
            break;

        case QLocale::Portuguese:
            m_data->currentLayout = &qskKeyboardLayouts.pt;
            break;

        case QLocale::Romanian:
            m_data->currentLayout = &qskKeyboardLayouts.ro;
            break;

        case QLocale::Russia:
            m_data->currentLayout = &qskKeyboardLayouts.ru;
            break;

        case QLocale::Slovenian:
            m_data->currentLayout = &qskKeyboardLayouts.sl;
            break;

        case QLocale::Slovak:
            m_data->currentLayout = &qskKeyboardLayouts.sk;
            break;

        case QLocale::Turkish:
            m_data->currentLayout = &qskKeyboardLayouts.tr;
            break;

        case QLocale::Chinese:
            m_data->currentLayout = &qskKeyboardLayouts.zh;
            break;
#if 1
        case QLocale::C:
            m_data->currentLayout = &qskKeyboardLayouts.en_US;
            break;
#endif
        default:
            qWarning() << "QskInputPanel: unsupported locale:" << locale;
            m_data->currentLayout = &qskKeyboardLayouts.en_US;

    }

    setMode( LowercaseMode );
    polish();
}

void QskVirtualKeyboard::setMode( QskVirtualKeyboard::Mode mode )
{
    m_data->mode = mode;
    polish();

    Q_EMIT modeChanged( m_data->mode );
}

bool QskVirtualKeyboard::eventFilter( QObject* object, QEvent* event )
{
    if ( event->type() == QEvent::InputMethodQuery )
    {
        /*
            Qt/Quick expects that the item associated with the input context
            always has the focus. But this does not work, when a virtual
            keyboard is used, where you can navigate and select inside.
            So we have to fix the receiver.

            Maybe QEvent::EnterEditFocus is good for something ??
         */

        if ( auto item = qskInputItem() )
            QGuiApplication::sendEvent( item, event );

        return true;
    }

    return Inherited::eventFilter( object, event );
}

bool QskVirtualKeyboard::event( QEvent* event )
{
    /*
        Handling Qt::Key_Return/Qt::KeyEscape here
        and forward everything else to the input item TODO ...
     */
    return Inherited::event( event );
}

#include "moc_QskVirtualKeyboard.cpp"
