/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskVirtualKeyboard.h"
#include "QskPushButton.h"
#include "QskTextOptions.h"

#include <qguiapplication.h>
#include <qset.h>
#include <qstylehints.h>

namespace
{
    enum
    {
        RowCount = 5,
        ColumnCount = 12
    };

    using KeyRow = int[ ColumnCount ];

    class Button final : public QskPushButton
    {
      public:
        Button( int row, int column, QskVirtualKeyboard* parent )
            : QskPushButton( parent )
            , m_row( row )
            , m_column( column )
        {
            QskTextOptions options;
            options.setFontSizeMode( QskTextOptions::VerticalFit );
            setTextOptions( options );

            setFocusPolicy( Qt::TabFocus );
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override
        {
            auto keyBoard = static_cast< const QskVirtualKeyboard* >( parent() );

            if ( subControl == QskPushButton::Panel )
                return keyBoard->effectiveSubcontrol( QskVirtualKeyboard::ButtonPanel );

            if ( subControl == QskPushButton::Text )
                return keyBoard->effectiveSubcontrol( QskVirtualKeyboard::ButtonText );

            return QskPushButton::effectiveSubcontrol( subControl );
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
        using Row = int[ ColumnCount ];
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

#define LOWER( x ) int( x + 32 ) // Convert an uppercase key to lowercase
static constexpr const QskVirtualKeyboardLayouts qskKeyboardLayouts =
{
#include "QskVirtualKeyboardLayouts.cpp"
};
#undef LOWER

static qreal qskKeyStretch( int key )
{
    switch ( key )
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

    for ( const auto& key : keyRow )
    {
        if ( !key )
        {
            continue;
        }

        stretch += qskKeyStretch( key );
    }

    if ( stretch == 0.0 )
    {
        stretch = ColumnCount;
    }

    return stretch;
}

static QString qskTextForKey( int key )
{
    // Special cases
    switch ( key )
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
    return (
        ( key != Qt::Key_Return ) &&
        ( key != Qt::Key_Enter ) &&
        ( key != Qt::Key_Shift ) &&
        ( key != Qt::Key_CapsLock ) &&
        ( key != Qt::Key_Mode_switch ) );
}

static QSet< int > qskKeyCodes( const QskVirtualKeyboardLayouts::Layout& layout )
{
    QSet< int > codes;
    codes.reserve( RowCount * ColumnCount );

    for ( int mode = 0; mode <= QskVirtualKeyboard::ModeCount; mode++ )
    {
        const auto& keyCodes = layout[ mode ];

        for ( int row = 0; row < RowCount; row++ )
        {
            const auto& keys = keyCodes.data[ row ];

            for ( int col = 0; col < ColumnCount; col++ )
                codes += keys[ col ];
        }
    }

    return codes;
}

QSK_SUBCONTROL( QskVirtualKeyboard, Panel )
QSK_SUBCONTROL( QskVirtualKeyboard, ButtonPanel )
QSK_SUBCONTROL( QskVirtualKeyboard, ButtonText )

class QskVirtualKeyboard::PrivateData
{
  public:
    const QskVirtualKeyboardLayouts::Layout* currentLayout = nullptr;
    QskVirtualKeyboard::Mode mode = QskVirtualKeyboard::LowercaseMode;

    QVector< Button* > keyButtons;
    QSet< int > keyCodes;
};

QskVirtualKeyboard::QskVirtualKeyboard( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
    setPolishOnResize( true );
    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Constrained );

    m_data->keyButtons.reserve( RowCount * ColumnCount );

    const auto autoRepeatInterval =
        1000 / QGuiApplication::styleHints()->keyboardAutoRepeatRate();

    for ( int row = 0; row < RowCount; row++ )
    {
        for ( int col = 0; col < ColumnCount; col++ )
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
    if ( subControl == QskBox::Panel )
        return QskVirtualKeyboard::Panel;

#if 1
    // TODO ...
    if ( subControl == QskVirtualKeyboard::ButtonPanel )
        return QskPushButton::Panel;

    if ( subControl == QskVirtualKeyboard::ButtonText )
        return QskPushButton::Text;
#endif

    return subControl;
}

QskVirtualKeyboard::Mode QskVirtualKeyboard::mode() const
{
    return m_data->mode;
}

QSizeF QskVirtualKeyboard::layoutSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    constexpr qreal ratio = qreal( RowCount ) / ColumnCount;

    qreal w = constraint.width();
    qreal h = constraint.height();

    if ( h >= 0 )
    {
        const auto padding = innerPadding( Panel, QSizeF( h, h ) );
        const auto dw = padding.left() + padding.right();
        const auto dh = padding.top() + padding.bottom();

        w = ( h - dh ) / ratio + dw;
    }
    else
    {
        if ( w < 0 )
            w = 600;

        const auto padding = innerPadding( Panel, QSizeF( w, w ) );
        const auto dw = padding.left() + padding.right();
        const auto dh = padding.top() + padding.bottom();

        h = ( w - dw ) * ratio + dh;
    }

    return QSizeF( w, h );
}

void QskVirtualKeyboard::updateLayout()
{
    const auto r = layoutRect();
    if ( r.isEmpty() )
        return;

    const auto spacing = spacingHint( Panel );
    const auto totalVSpacing = ( RowCount - 1 ) * spacing;

    const auto keyHeight = ( r.height() - totalVSpacing ) / RowCount;

    const auto& keyCodes = ( *m_data->currentLayout )[ m_data->mode ];

    qreal yPos = r.top();

    for ( int row = 0; row < RowCount; row++ )
    {
        const auto& keys = keyCodes.data[ row ];

#if 1
        // there should be a better way
        auto totalHSpacing = -spacing;
        if ( spacing )
        {
            for ( int col = 0; col < ColumnCount; col++ )
            {
                if ( keys[ col ] != 0 )
                    totalHSpacing += spacing;
            }
        }
#endif
        const auto baseKeyWidth = ( r.width() - totalHSpacing ) / qskRowStretch( keys );
        qreal xPos = r.left();

        for ( int col = 0; col < ColumnCount; col++ )
        {
            const int key = keys[ col ];
            auto button = m_data->keyButtons[ row * ColumnCount + col ];

            button->setVisible( key != 0 );

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

bool QskVirtualKeyboard::hasKey( int keyCode ) const
{
    return m_data->keyCodes.contains( keyCode );
}

void QskVirtualKeyboard::buttonPressed()
{
    const auto button = static_cast< const Button* >( sender() );
    if ( button == nullptr )
        return;

    const auto& keyCodes = ( *m_data->currentLayout )[ m_data->mode ];
    const int key = keyCodes.data[ button->row() ][ button->column() ];

    // Mode-switching keys
    switch ( key )
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
            Q_EMIT keySelected( key );
        }
    }
}

void QskVirtualKeyboard::updateLocale( const QLocale& locale )
{
    const QskVirtualKeyboardLayouts::Layout* newLayout = nullptr;

    switch ( locale.language() )
    {
        case QLocale::Bulgarian:
            newLayout = &qskKeyboardLayouts.bg;
            break;

        case QLocale::Czech:
            newLayout = &qskKeyboardLayouts.cs;
            break;

        case QLocale::German:
            newLayout = &qskKeyboardLayouts.de;
            break;

        case QLocale::Danish:
            newLayout = &qskKeyboardLayouts.da;
            break;

        case QLocale::Greek:
            newLayout = &qskKeyboardLayouts.el;
            break;

        case QLocale::English:
        {
            switch ( locale.country() )
            {
                case QLocale::Canada:
                case QLocale::UnitedStates:
                case QLocale::UnitedStatesMinorOutlyingIslands:
                case QLocale::UnitedStatesVirginIslands:
                    newLayout = &qskKeyboardLayouts.en_US;
                    break;

                default:
                    newLayout = &qskKeyboardLayouts.en_GB;
                    break;
            }

            break;
        }

        case QLocale::Spanish:
            newLayout = &qskKeyboardLayouts.es;
            break;

        case QLocale::Finnish:
            newLayout = &qskKeyboardLayouts.fi;
            break;

        case QLocale::French:
            newLayout = &qskKeyboardLayouts.fr;
            break;

        case QLocale::Hungarian:
            newLayout = &qskKeyboardLayouts.hu;
            break;

        case QLocale::Italian:
            newLayout = &qskKeyboardLayouts.it;
            break;

        case QLocale::Japanese:
            newLayout = &qskKeyboardLayouts.ja;
            break;

        case QLocale::Latvian:
            newLayout = &qskKeyboardLayouts.lv;
            break;

        case QLocale::Lithuanian:
            newLayout = &qskKeyboardLayouts.lt;
            break;

        case QLocale::Dutch:
            newLayout = &qskKeyboardLayouts.nl;
            break;

        case QLocale::Portuguese:
            newLayout = &qskKeyboardLayouts.pt;
            break;

        case QLocale::Romanian:
            newLayout = &qskKeyboardLayouts.ro;
            break;

        case QLocale::Russian:
            newLayout = &qskKeyboardLayouts.ru;
            break;

        case QLocale::Slovenian:
            newLayout = &qskKeyboardLayouts.sl;
            break;

        case QLocale::Slovak:
            newLayout = &qskKeyboardLayouts.sk;
            break;

        case QLocale::Turkish:
            newLayout = &qskKeyboardLayouts.tr;
            break;

        case QLocale::Chinese:
            newLayout = &qskKeyboardLayouts.zh;
            break;
#if 1
        case QLocale::C:
            newLayout = &qskKeyboardLayouts.en_US;
            break;
#endif
        default:
            qWarning() << "QskVirtualKeyboard: unsupported locale:" << locale;
            newLayout = &qskKeyboardLayouts.en_US;
    }

    if ( newLayout != m_data->currentLayout )
    {
        m_data->currentLayout = newLayout;
        m_data->keyCodes = qskKeyCodes( *newLayout );

        setMode( LowercaseMode );
        polish();
    }
}

void QskVirtualKeyboard::setMode( QskVirtualKeyboard::Mode mode )
{
    m_data->mode = mode;
    polish();

    Q_EMIT modeChanged( m_data->mode );
}

#include "moc_QskVirtualKeyboard.cpp"
