/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputPanel.h"
#include "QskInputEngine.h"
#include "QskVirtualKeyboard.h"
#include "QskInputPredictionBar.h"
#include "QskTextInput.h"
#include "QskTextLabel.h"
#include "QskLinearBox.h"

#include <QString>
#include <QLocale>
#include <QGuiApplication>
#include <QPointer>
#include <QInputMethodQueryEvent>
#include <QTextCharFormat>

static inline void qskSendReplaceText( QQuickItem* receiver, const QString& text )
{
    if ( receiver == nullptr )
        return;

    QInputMethodEvent::Attribute attribute(
        QInputMethodEvent::Selection, 0, 32767, QVariant() );

    QInputMethodEvent event1( QString(), { attribute } );
    QCoreApplication::sendEvent( receiver, &event1 );

    QInputMethodEvent event2;
    event2.setCommitString( text );

    QCoreApplication::sendEvent( receiver, &event2 );
}

static inline void qskSendText( QQuickItem* receiver,
    const QString& text, bool isFinal )
{
    if ( receiver == nullptr )
        return;

    if ( isFinal )
    {
        QInputMethodEvent event;
        event.setCommitString( text );

        QCoreApplication::sendEvent( receiver, &event );
    }
    else
    {
        QTextCharFormat format;
        format.setFontUnderline( true );

        const QInputMethodEvent::Attribute attribute(
            QInputMethodEvent::TextFormat, 0, text.length(), format );

        QInputMethodEvent event( text, { attribute } );

        QCoreApplication::sendEvent( receiver, &event );
    }
}

static inline void qskSendKey( QQuickItem* receiver, int key )
{
    if ( receiver == nullptr )
        return;

    QKeyEvent keyPress( QEvent::KeyPress, key, Qt::NoModifier );
    QCoreApplication::sendEvent( receiver, &keyPress );

    QKeyEvent keyRelease( QEvent::KeyRelease, key, Qt::NoModifier );
    QCoreApplication::sendEvent( receiver, &keyRelease );
}

static inline void qskSyncInputProxy(
    QQuickItem* inputItem, QskTextInput* inputProxy )
{
    int passwordMaskDelay = -1;
    QString passwordCharacter;

    if ( auto textInput = qobject_cast< QskTextInput* >( inputItem ) )
    {
        passwordMaskDelay = textInput->passwordMaskDelay();
        passwordCharacter = textInput->passwordCharacter();

        if ( inputProxy->echoMode() == QskTextInput::NoEcho )
        {
            /*
                Qt::ImhHiddenText does not provide information
                to decide between NoEcho/Password
             */
            auto mode = textInput->echoMode();
            if ( mode == QskTextInput::Password )
                inputProxy->setEchoMode( mode );
        }
    }

    if ( passwordMaskDelay >= 0 )
        inputProxy->setPasswordMaskDelay( passwordMaskDelay );
    else
        inputProxy->resetPasswordMaskDelay();

    if ( !passwordCharacter.isEmpty() )
        inputProxy->setPasswordCharacter( passwordCharacter );
    else
        inputProxy->resetPasswordCharacter();
}

namespace
{
    class TextInputProxy final : public QskTextInput
    {
    public:
        TextInputProxy( QQuickItem* parentItem = nullptr ):
            QskTextInput( parentItem )
        {
            setObjectName( "InputPanelInputProxy" );
#if 1
            setFocusPolicy( Qt::NoFocus );
#endif
        }

    protected:
        virtual void focusInEvent( QFocusEvent* ) override final
        {
        }

        virtual void focusOutEvent( QFocusEvent* ) override final
        {
        }
    };
}

QSK_SUBCONTROL( QskInputPanel, Panel )

class QskInputPanel::PrivateData
{
public:
    PrivateData():
        inputHints( 0 ),
        maxChars( -1 ),
        hasPrediction( true ),
        hasInputProxy( true )
    {
    }

    QQuickItem* receiverItem()
    {
        return hasInputProxy ? inputProxy : inputItem;
    }

    QPointer< QskInputEngine > engine;
    QPointer< QQuickItem > inputItem;

    QskLinearBox* layout;
    QskTextLabel* prompt;
    TextInputProxy* inputProxy;
    QskInputPredictionBar* predictionBar;
    QskVirtualKeyboard* keyboard;

    Qt::InputMethodHints inputHints;
    int maxChars;

    bool hasPrediction : 1;
    bool hasInputProxy : 1;
};

QskInputPanel::QskInputPanel( QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
    setAutoLayoutChildren( true );
    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Constrained );

    m_data->prompt = new QskTextLabel();
    m_data->prompt->setVisible( false );

    m_data->inputProxy = new TextInputProxy();
    m_data->inputProxy->setVisible( m_data->hasInputProxy );

    m_data->predictionBar = new QskInputPredictionBar();
    m_data->predictionBar->setVisible( false );

    m_data->keyboard = new QskVirtualKeyboard();

    auto layout = new QskLinearBox( Qt::Vertical, this );

    layout->addItem( m_data->prompt, Qt::AlignLeft | Qt::AlignHCenter );
    layout->addItem( m_data->inputProxy, Qt::AlignLeft | Qt::AlignHCenter );
    layout->addStretch( 10 );
    layout->addItem( m_data->predictionBar );
    layout->addItem( m_data->keyboard );

    m_data->layout = layout;

    connect( m_data->predictionBar, &QskInputPredictionBar::predictiveTextSelected,
        this, &QskInputPanel::commitPredictiveText );

    connect( m_data->keyboard, &QskVirtualKeyboard::keySelected,
        this, &QskInputPanel::commitKey );
}

QskInputPanel::~QskInputPanel()
{
}

void QskInputPanel::setEngine( QskInputEngine* engine )
{
    if ( engine == m_data->engine )
        return;

    if ( m_data->engine )
        m_data->engine->disconnect( this );

    m_data->engine = engine;

    if ( engine )
    {
        connect( engine, &QskInputEngine::predictionChanged,
            this, &QskInputPanel::updatePredictionBar );
    }

    m_data->predictionBar->setVisible(
        m_data->hasPrediction && engine && engine->predictor() );
}

void QskInputPanel::attachInputItem( QQuickItem* item )
{
    if ( item == m_data->inputItem )
        return;

    m_data->inputItem = item;

    if ( m_data->engine )
        m_data->engine->reset();

    if ( item )
    {
        Qt::InputMethodQueries queries = Qt::ImQueryAll;
        queries &= ~Qt::ImEnabled;

        processInputMethodQueries( queries );

        if ( m_data->hasInputProxy )
        {
            m_data->inputProxy->setEditing( true );

            // hiding the cursor in the real input item
            const QInputMethodEvent::Attribute attribute(
                QInputMethodEvent::Cursor, 0, 0, QVariant() );

            QInputMethodEvent event( QString(), { attribute } );
            QCoreApplication::sendEvent( item, &event );

            // not all information is available from the input method query
            qskSyncInputProxy( item, m_data->inputProxy );
        }
    }
}

QQuickItem* QskInputPanel::attachedInputItem() const
{
    return m_data->inputItem;
}

void QskInputPanel::updatePredictionBar()
{
    m_data->predictionBar->setPrediction(
        m_data->engine->prediction() );
}

QskInputEngine* QskInputPanel::engine()
{
    return m_data->engine;
}

QskAspect::Subcontrol QskInputPanel::effectiveSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskBox::Panel )
        return QskInputPanel::Panel;

    return subControl;
}

QString QskInputPanel::inputPrompt() const
{
    return m_data->prompt->text();
}

void QskInputPanel::setInputPrompt( const QString& text )
{
    auto prompt = m_data->prompt;

    if ( text != prompt->text() )
    {
        prompt->setText( text );

        if ( m_data->hasInputProxy )
            prompt->setVisible( !text.isEmpty() );

        Q_EMIT inputPromptChanged( text );
    }
}

bool QskInputPanel::hasInputProxy() const
{
    return m_data->hasInputProxy;
}

void QskInputPanel::setInputProxy( bool on )
{
    if ( m_data->hasInputProxy == on )
        return;

    m_data->hasInputProxy = on;
    m_data->inputProxy->setVisible( on );

    auto prompt = m_data->prompt;

    if ( on )
        prompt->setVisible( !prompt->text().isEmpty() );
    else
        prompt->setVisible( false );
}

void QskInputPanel::commitPredictiveText( int index )
{
    m_data->predictionBar->setPrediction( QVector< QString >() );

    if ( m_data->engine )
    {
        const QString text = m_data->engine->predictiveText( index );

        m_data->engine->reset();
        m_data->predictionBar->setPrediction( QVector< QString >() );

        qskSendText( m_data->receiverItem(), text, true );
    }
}

void QskInputPanel::commitKey( int key )
{
    if ( m_data->engine == nullptr || m_data->inputItem == nullptr )
        return;

    int spaceLeft = -1;

    if ( !( m_data->inputHints & Qt::ImhMultiLine ) )
    {
        auto receiver = m_data->receiverItem();

        if ( m_data->maxChars >= 0 )
        {
            QInputMethodQueryEvent event( Qt::ImSurroundingText );
            QCoreApplication::sendEvent( receiver, &event );

            const auto text = event.value( Qt::ImSurroundingText ).toString();
            spaceLeft = m_data->maxChars - text.length();
        }
    }

    processKey( key, m_data->inputHints, spaceLeft );
}

void QskInputPanel::processKey( int key,
    Qt::InputMethodHints inputHints, int spaceLeft )
{
    const auto result = m_data->engine->processKey( key, inputHints, spaceLeft );

    auto inputItem = m_data->inputItem;
    auto inputProxy = m_data->inputProxy;

    if ( result.key )
    {
        switch( result.key )
        {
            case Qt::Key_Return:
            {
                if ( m_data->hasInputProxy )
                    qskSendReplaceText( inputItem, inputProxy->text() );

                qskSendKey( inputItem, result.key );
                break;
            }
            case Qt::Key_Escape:
            {
                qskSendKey( inputItem, result.key );
                break;
            }
            default:
            {
                qskSendKey( m_data->receiverItem(), result.key );
            }
        }
    }
    else if ( !result.text.isEmpty() )
    {
        // changing the current text
        qskSendText( m_data->receiverItem(), result.text, result.isFinal );
    }
}

void QskInputPanel::processInputMethodQueries( Qt::InputMethodQueries queries )
{
    if ( m_data->inputItem == nullptr )
        return;

    QInputMethodQueryEvent event( queries );
    QCoreApplication::sendEvent( m_data->inputItem, &event );

    if ( event.queries() & Qt::ImHints )
    {
        bool hasPrediction = true;
        QskTextInput::EchoMode echoMode = QskTextInput::Normal;

        const auto hints = static_cast< Qt::InputMethodHints >(
            event.value( Qt::ImHints ).toInt() );

        if ( hints & Qt::ImhHiddenText )
        {
            echoMode = QskTextInput::NoEcho;
        }

        if ( hints & Qt::ImhSensitiveData )
        {
        }

        if ( hints & Qt::ImhNoAutoUppercase )
        {
        }

        if ( hints & Qt::ImhPreferNumbers )
        {
            // we should start with having the number keys being visible
        }

        if ( hints & Qt::ImhPreferUppercase )
        {
            // we should start with having the upper keys being visible
        }

        if ( hints & Qt::ImhPreferLowercase )
        {
            // we should start with having the upper keys being visible
        }

        if ( hints & Qt::ImhNoPredictiveText )
        {
            hasPrediction = false;
        }

        if ( hints & Qt::ImhDate )
        {
            // we should have a date/time input
        }

        if ( hints & Qt::ImhTime )
        {
            // we should have a date/time input
        }

        if ( hints & Qt::ImhPreferLatin )
        {
            // conflicts with our concept of using the locale
        }

        if ( hints & Qt::ImhMultiLine )
        {
            // we need an implementation of QskTextEdit for this
        }

        if ( hints & Qt::ImhDigitsOnly )
        {
            // using a numpad instead of our virtual keyboard
            hasPrediction = false;
        }

        if ( hints & Qt::ImhFormattedNumbersOnly )
        {
            // a numpad with decimal point and minus sign
            hasPrediction = false;
        }

        if ( hints & Qt::ImhUppercaseOnly )
        {
            // locking all other keys
        }

        if ( hints & Qt::ImhLowercaseOnly )
        {
            // locking all other keys
        }

        if ( hints & Qt::ImhDialableCharactersOnly )
        {
            // characters suitable for phone dialing
            hasPrediction = false;
        }

        if ( hints & Qt::ImhEmailCharactersOnly )
        {
            // characters suitable for email addresses
            hasPrediction = false;
        }

        if ( hints & Qt::ImhUrlCharactersOnly )
        {
            // characters suitable for URLs
            hasPrediction = false;
        }

        if ( hints & Qt::ImhLatinOnly )
        {
            // locking all other keys
        }

        m_data->hasPrediction = hasPrediction;

        m_data->predictionBar->setVisible(
            hasPrediction && m_data->engine && m_data->engine->predictor() );

        m_data->inputProxy->setEchoMode( echoMode );

        m_data->inputHints = hints;
    }

    if ( event.queries() & Qt::ImPreferredLanguage )
    {
        // already handled by the input context
    }

    if ( event.queries() & Qt::ImMaximumTextLength )
    {
        // needs to be handled before Qt::ImCursorPosition !

        m_data->maxChars = event.value( Qt::ImMaximumTextLength ).toInt();
#if 1
        if ( m_data->maxChars >= 32767 )
            m_data->maxChars = -1;
#endif

        if ( m_data->hasInputProxy )
            m_data->inputProxy->setMaxLength( m_data->maxChars );
    }


    if ( event.queries() & Qt::ImSurroundingText )
    {
        if ( m_data->hasInputProxy )
        {
            const auto text = event.value( Qt::ImSurroundingText ).toString();
            m_data->inputProxy->setText( text );
        }
    }

    if ( event.queries() & Qt::ImCursorPosition )
    {
        if ( m_data->hasInputProxy )
        {
            const auto pos = event.value( Qt::ImCursorPosition ).toInt();
            m_data->inputProxy->setCursorPosition( pos );
        }
    }

    if ( event.queries() & Qt::ImCurrentSelection )
    {
#if 0
        const auto text = event.value( Qt::ImCurrentSelection ).toString();
        if ( !text.isEmpty() )
        {
        }
#endif
    }
    /*
        Qt::ImMicroFocus
        Qt::ImCursorRectangle
        Qt::ImFont
        Qt::ImAnchorPosition

        Qt::ImAbsolutePosition
        Qt::ImTextBeforeCursor
        Qt::ImTextAfterCursor
        Qt::ImPlatformData     // hard to say...
        Qt::ImEnterKeyType
        Qt::ImAnchorRectangle
        Qt::ImInputItemClipRectangle
     */
}

void QskInputPanel::keyPressEvent( QKeyEvent* event )
{
    // animate the corresponding key button TODO

    switch( event->key() )
    {
        case Qt::Key_Return:
        case Qt::Key_Escape:
        {
            commitKey( event->key() );
            break;
        }

        case Qt::Key_Shift:
        case Qt::Key_Control:
        case Qt::Key_Meta:
        case Qt::Key_Alt:
        case Qt::Key_AltGr:
        case Qt::Key_CapsLock:
        case Qt::Key_NumLock:
        case Qt::Key_ScrollLock:
        {
            break;
        }

        default:
        {
            const auto text = event->text();
            if ( !text.isEmpty() )
                commitKey( text[0].unicode() );
            else
                commitKey( event->key() );
        }
    }
}

void QskInputPanel::keyReleaseEvent( QKeyEvent* event )
{
    return Inherited::keyReleaseEvent( event );
}

qreal QskInputPanel::heightForWidth( qreal width ) const
{
    /*
        This code looks like as it could be generalized
        and moved to QskLinearBox. TODO ...
     */

    const auto margins = this->margins();

    width -= margins.left() + margins.right();

    const auto padding = innerPadding(
        Panel, QSizeF( width, width ) );

    width -= padding.left() + padding.right();

    qreal height = m_data->keyboard->heightForWidth( width );

    const QskControl* controls[] =
        { m_data->prompt, m_data->inputProxy, m_data->predictionBar };

    for ( auto control : controls )
    {
        if ( control->isVisible() )
        {
            height += m_data->layout->spacing();
            height += control->sizeHint().height();
        }
    }

    height += padding.top() + padding.bottom();
    height += margins.top() + margins.bottom();

    return height;
}

qreal QskInputPanel::widthForHeight( qreal height ) const
{
    const auto margins = this->margins();

    height -= margins.top() + margins.bottom();

    const auto padding = innerPadding(
        Panel, QSizeF( height, height ) );

    height -= padding.top() + padding.bottom();

    const QskControl* controls[] =
        { m_data->prompt, m_data->inputProxy, m_data->predictionBar };

    for ( auto control : controls )
    {
        if ( control->isVisible() )
        {
            height -= m_data->layout->spacing();
            height -= control->sizeHint().height();
        }
    }

    qreal width = m_data->keyboard->widthForHeight( height );

    width += padding.left() + padding.right();
    width += margins.left() + margins.right();

    return width;
}

#include "moc_QskInputPanel.cpp"
