/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskVirtualKeyboard.h"
#include "QskPushButton.h"

#include <qguiapplication.h>
#include <qset.h>
#include <qstylehints.h>

namespace
{
    class Button : public QskPushButton
    {
      public:
        Button( QskVirtualKeyboard* parent )
            : QskPushButton( parent )
        {
            setFocusPolicy( Qt::TabFocus );

            setSubcontrolProxy( QskPushButton::Panel, QskVirtualKeyboard::ButtonPanel );
            setSubcontrolProxy( QskPushButton::Text, QskVirtualKeyboard::ButtonText );
        }

        int key() const
        {
            return m_key;
        }

        void setKey( int key )
        {
            m_key = key;
        }

      private:
        int m_key = 0;
    };

    static bool qskIsAutorepeat( int key )
    {
        return (
            ( key != Qt::Key_Return ) &&
            ( key != Qt::Key_Enter ) &&
            ( key != Qt::Key_Shift ) &&
            ( key != Qt::Key_CapsLock ) &&
            ( key != Qt::Key_Mode_switch ) );
    }
}


QSK_SUBCONTROL( QskVirtualKeyboard, Panel )
QSK_SUBCONTROL( QskVirtualKeyboard, ButtonPanel )
QSK_SUBCONTROL( QskVirtualKeyboard, ButtonText )

class QskVirtualKeyboard::PrivateData
{
  public:
    int rowCount = 5;
    int columnCount = 12;

    QskVirtualKeyboardLayouts layouts;
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
    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Fixed );

#define LOWER( x ) int( x + 32 ) // Convert an uppercase key to lowercase
    m_data->layouts =
    {
#include "QskVirtualKeyboardLayouts.cpp"
    };
#undef LOWER

    ensureButtons();

    connect( this, &QskControl::localeChanged,
        this, &QskVirtualKeyboard::updateLocale );

    updateLocale( locale() );

    setSubcontrolProxy( QskBox::Panel, Panel );
}

QskVirtualKeyboard::~QskVirtualKeyboard()
{
}

void QskVirtualKeyboard::setMode( QskVirtualKeyboard::Mode mode )
{
    m_data->mode = mode;
    polish();

    Q_EMIT modeChanged( m_data->mode );
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

    const qreal ratio = qreal( rowCount() ) / columnCount();

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

qreal QskVirtualKeyboard::keyStretch( int key ) const
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

bool QskVirtualKeyboard::isKeyVisible( int key ) const
{
    return key != 0;
}

QString QskVirtualKeyboard::textForKey( int key ) const
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

QskVirtualKeyboard::KeyType QskVirtualKeyboard::typeForKey( int key ) const
{
    switch( key )
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            return EnterType;

        case Qt::Key_Backspace:
            return BackspaceType;

        case Qt::Key_Shift:
        case Qt::Key_CapsLock:
            return CapsSwitchType;

        case Qt::Key_Mode_switch:
            return ModeSwitchType;

        case Qt::Key_Comma:
        case Qt::Key_Period:
            return SpecialCharacterType;

        default:
            return NormalType;
    }
}

void QskVirtualKeyboard::updateLayout() // ### fill keyCodes here
{
    const auto r = layoutRect();
    if ( r.isEmpty() )
        return;

    const auto spacing = spacingHint( Panel );
    const auto totalVSpacing = ( rowCount() - 1 ) * spacing;

    const auto keyHeight = ( r.height() - totalVSpacing ) / rowCount();

    qreal yPos = r.top();

    const auto& page = ( *m_data->currentLayout )[ mode() ];

    for ( int i = 0; i < page.size(); i++ )
    {
        const QVector< int > row = page[ i ];
#if 1
        // there should be a better way
        auto totalHSpacing = -spacing;
        if ( spacing )
        {
            for ( int j = 0; j < row.size(); j++ )
            {
                if ( row[ j ] != 0 )
                    totalHSpacing += spacing;
            }
        }
#endif
        const auto baseKeyWidth = ( r.width() - totalHSpacing ) / rowStretch( row );
        qreal xPos = r.left();

        for ( int j = 0; j < columnCount(); j++ )
        {
            auto button = m_data->keyButtons[ i * columnCount() + j ];

            if( j < row.size() )
            {
                const int key = row[ j ];
                button->setVisible( isKeyVisible( key ) );

                if ( button->isVisible() )
                {
                    const qreal keyWidth = baseKeyWidth * keyStretch( key );

                    const QRectF rect( xPos, yPos, keyWidth, keyHeight );

                    button->setGeometry( rect );
                    button->setAutoRepeat( qskIsAutorepeat( key ) );
                    button->setKey( key );
                    button->setText( textForKey( key ) );

                    const auto type = typeForKey( key );
                    const auto emphasis = emphasisForType( type );
                    button->setEmphasis( emphasis );

                    xPos += keyWidth + spacing;
                }
            }
            else
            {
                button->setVisible( false );
            }
        }

        yPos += keyHeight + spacing;
    }
}

bool QskVirtualKeyboard::hasKey( int keyCode ) const
{
    return m_data->keyCodes.contains( keyCode );
}

int QskVirtualKeyboard::rowCount() const
{
    return m_data->rowCount;
}

void QskVirtualKeyboard::setRowCount( int rowCount )
{
    m_data->rowCount = rowCount;
    ensureButtons();
}

int QskVirtualKeyboard::columnCount() const
{
    return m_data->columnCount;
}

void QskVirtualKeyboard::setColumnCount( int columnCount )
{
    m_data->columnCount = columnCount;
    ensureButtons();
}

QskVirtualKeyboardLayouts QskVirtualKeyboard::layouts() const
{
    return m_data->layouts;
}

void QskVirtualKeyboard::setLayouts( const QskVirtualKeyboardLayouts& layouts )
{
    m_data->layouts = layouts;
}

void QskVirtualKeyboard::ensureButtons()
{
    const int newButtonSize = rowCount() * columnCount();
    const int oldButtonSize = m_data->keyButtons.size();

    if( newButtonSize == oldButtonSize )
        return;

    const auto autoRepeatInterval =
        1000 / QGuiApplication::styleHints()->keyboardAutoRepeatRate();

    m_data->keyButtons.reserve( rowCount() * columnCount() );

    for( int i = 0; i < rowCount(); i++ )
    {
        for( int j = 0; j < columnCount(); j++ )
        {
            const int index = i * columnCount() + j;

            if( index >= m_data->keyButtons.size() )
            {
                auto button = new Button( this );
                button->installEventFilter( this );

                button->setAutoRepeat( false );
                button->setAutoRepeatDelay( 500 );
                button->setAutoRepeatInterval( autoRepeatInterval );

                connect( button, &QskPushButton::pressed,
                    this, &QskVirtualKeyboard::buttonPressed );

                m_data->keyButtons += button;
            }
        }
    }

    while( m_data->keyButtons.size() > newButtonSize )
    {
        auto* button = m_data->keyButtons.takeLast();
        button->deleteLater();
    }
}

void QskVirtualKeyboard::buttonPressed()
{
    const auto button = static_cast< const Button* >( sender() );
    if ( button == nullptr )
        return;

    const int key = button->key();

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

void QskVirtualKeyboard::updateKeyCodes()
{
    m_data->keyCodes = {};
    m_data->keyCodes.reserve( rowCount() * columnCount() );

    const auto layout = *m_data->currentLayout;

    for ( int mode = 0; mode < layout.size(); mode++ )
    {
        const auto& page = layout[ mode ];

        for ( int i = 0; i < page.size(); i++ )
        {
            const auto& row = page[ i ];

            for ( int j = 0; j < row.size(); j++ )
            {
                m_data->keyCodes += row[ j ];
            }
        }
    }
}

qreal QskVirtualKeyboard::rowStretch( const QVector< int >& row )
{
    qreal stretch = 0;

    for ( const int& key : row )
    {
        if ( !key )
        {
            continue;
        }

        stretch += keyStretch( key );
    }

    if ( stretch == 0.0 )
    {
        stretch = columnCount();
    }

    return stretch;
}

void QskVirtualKeyboard::updateLocale( const QLocale& locale )
{
    const QskVirtualKeyboardLayouts::Layout* newLayout = nullptr;

    switch ( locale.language() )
    {
        case QLocale::Bulgarian:
            newLayout = &m_data->layouts.bg;
            break;

        case QLocale::Czech:
            newLayout = &m_data->layouts.cs;
            break;

        case QLocale::German:
            newLayout = &m_data->layouts.de;
            break;

        case QLocale::Danish:
            newLayout = &m_data->layouts.da;
            break;

        case QLocale::Greek:
            newLayout = &m_data->layouts.el;
            break;

        case QLocale::English:
        {
            switch ( locale.country() )
            {
                case QLocale::Canada:
                case QLocale::UnitedStates:
                case QLocale::UnitedStatesMinorOutlyingIslands:
                case QLocale::UnitedStatesVirginIslands:
                    newLayout = &m_data->layouts.en_US;
                    break;

                default:
                    newLayout = &m_data->layouts.en_GB;
                    break;
            }

            break;
        }

        case QLocale::Spanish:
            newLayout = &m_data->layouts.es;
            break;

        case QLocale::Finnish:
            newLayout = &m_data->layouts.fi;
            break;

        case QLocale::French:
            newLayout = &m_data->layouts.fr;
            break;

        case QLocale::Hungarian:
            newLayout = &m_data->layouts.hu;
            break;

        case QLocale::Italian:
            newLayout = &m_data->layouts.it;
            break;

        case QLocale::Japanese:
            newLayout = &m_data->layouts.ja;
            break;

        case QLocale::Latvian:
            newLayout = &m_data->layouts.lv;
            break;

        case QLocale::Lithuanian:
            newLayout = &m_data->layouts.lt;
            break;

        case QLocale::Dutch:
            newLayout = &m_data->layouts.nl;
            break;

        case QLocale::Portuguese:
            newLayout = &m_data->layouts.pt;
            break;

        case QLocale::Romanian:
            newLayout = &m_data->layouts.ro;
            break;

        case QLocale::Russian:
            newLayout = &m_data->layouts.ru;
            break;

        case QLocale::Slovenian:
            newLayout = &m_data->layouts.sl;
            break;

        case QLocale::Slovak:
            newLayout = &m_data->layouts.sk;
            break;

        case QLocale::Turkish:
            newLayout = &m_data->layouts.tr;
            break;

        case QLocale::Chinese:
            newLayout = &m_data->layouts.zh;
            break;
#if 1
        case QLocale::C:
            newLayout = &m_data->layouts.en_US;
            break;
#endif
        default:
            qWarning() << "QskVirtualKeyboard: unsupported locale:" << locale;
            newLayout = &m_data->layouts.en_US;
    }

    if ( newLayout != m_data->currentLayout )
    {
        m_data->currentLayout = newLayout;
        updateKeyCodes();

        setMode( LowercaseMode );
        polish();
        Q_EMIT keyboardLayoutChanged();
    }
}

QskPushButton::Emphasis QskVirtualKeyboard::emphasisForType( KeyType type )
{
    switch( type )
    {
        case EnterType:
            return QskPushButton::VeryHighEmphasis;

        case BackspaceType:
        case CapsSwitchType:
            return QskPushButton::HighEmphasis;

        case ModeSwitchType:
            return QskPushButton::LowEmphasis;

        case SpecialCharacterType:
            return QskPushButton::VeryLowEmphasis;

        default:
            return QskPushButton::NoEmphasis;
    }
}

#include "moc_QskVirtualKeyboard.cpp"
