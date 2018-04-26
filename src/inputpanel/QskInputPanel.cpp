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

static inline void qskSendText( QQuickItem* inputItem,
    const QString& text, bool isFinal )
{
    if ( inputItem == nullptr )
        return;

    if ( isFinal )
    {
        QInputMethodEvent event;
        event.setCommitString( text );

        QCoreApplication::sendEvent( inputItem, &event );
    }
    else
    {
        QTextCharFormat format;
        format.setFontUnderline( true );

        const QInputMethodEvent::Attribute attribute(
            QInputMethodEvent::TextFormat, 0, text.length(), format );

        QInputMethodEvent event( text, { attribute } );

        QCoreApplication::sendEvent( inputItem, &event );
    }
}

static inline void qskSendKey( QQuickItem* inputItem, int key )
{
    if ( inputItem == nullptr )
        return;

    QKeyEvent keyPress( QEvent::KeyPress, key, Qt::NoModifier );
    QCoreApplication::sendEvent( inputItem, &keyPress );

    QKeyEvent keyRelease( QEvent::KeyRelease, key, Qt::NoModifier );
    QCoreApplication::sendEvent( inputItem, &keyRelease );
}

namespace
{
    class TextInput : public QskTextInput
    {
    public:
        TextInput( QQuickItem* parentItem = nullptr ):
            QskTextInput( parentItem )
        {
        }
    };
}

QSK_SUBCONTROL( QskInputPanel, Panel )

class QskInputPanel::PrivateData
{
public:
    PrivateData():
        hasInputProxy( true )
    {
    }

    QPointer< QskInputEngine > engine;
    QPointer< QQuickItem > inputItem;

    QskLinearBox* layout;
    QskTextLabel* prompt;
    TextInput* inputProxy;
    QskInputPredictionBar* predictionBar;
    QskVirtualKeyboard* keyboard;

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

    m_data->inputProxy = new TextInput();
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

    m_data->predictionBar->setVisible( engine && engine->predictor() );
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

void QskInputPanel::updateInputProxy( const QQuickItem* inputItem )
{
    if ( inputItem == nullptr )
        return;

    QInputMethodQueryEvent event( Qt::ImQueryAll );
    QCoreApplication::sendEvent( const_cast< QQuickItem* >( inputItem ), &event );

    const auto proxy = m_data->inputProxy;

    if ( event.queries() & Qt::ImHints )
    {
        const auto hints = static_cast< Qt::InputMethodHints >(
            event.value( Qt::ImHints ).toInt() );

        const auto echoMode = ( hints & Qt::ImhHiddenText )
            ? QskTextInput::PasswordEchoOnEdit : QskTextInput::Normal;

        proxy->setEchoMode( echoMode );
    }

    if ( event.queries() & Qt::ImSurroundingText )
    {
        const auto text = event.value( Qt::ImSurroundingText ).toString();
        proxy->setText( text );
    }

    if ( event.queries() & Qt::ImCursorPosition )
    {
        const auto pos = event.value( Qt::ImCursorPosition ).toInt();
        proxy->setCursorPosition( pos );
    }

#if 0
    if ( event.queries() & Qt::ImCurrentSelection )
    {
        const auto text = event.value( Qt::ImCursorPosition ).toString();
        if ( !text.isEmpty() )
        {
        }
    }
#endif

    if ( event.queries() & Qt::ImMaximumTextLength )
    {
        const auto length = event.value( Qt::ImMaximumTextLength ).toInt();
        proxy->setMaxLength( length );
    }
}

void QskInputPanel::commitPredictiveText( int index )
{
    m_data->predictionBar->setPrediction( QVector< QString >() );

    if ( m_data->engine )
    {
        const QString text = m_data->engine->predictiveText( index );
        m_data->engine->reset();

        qskSendText( m_data->inputItem, text, true );
    }
}

void QskInputPanel::commitKey( int key )
{
    if ( m_data->engine == nullptr || m_data->inputItem == nullptr )
        return;

    auto engine = m_data->engine;
    auto inputItem = m_data->inputItem;

    QInputMethodQueryEvent event( Qt::ImHints );
    QCoreApplication::sendEvent( inputItem, &event );

    const auto inputHints = static_cast< Qt::InputMethodHints >(
        event.value( Qt::ImHints ).toInt() );

    int spaceLeft = -1;

    if ( !( inputHints & Qt::ImhMultiLine ) )
    {
        QInputMethodQueryEvent event(
            Qt::ImSurroundingText | Qt::ImMaximumTextLength );

        QCoreApplication::sendEvent( inputItem, &event );

        const int max = event.value( Qt::ImMaximumTextLength ).toInt();

        if ( max > 0 )
        {
            const auto text = event.value( Qt::ImSurroundingText ).toString();
            spaceLeft = max - text.length();
        }
    }

    processKey( key, inputHints, spaceLeft );
}

void QskInputPanel::processKey( int key,
    Qt::InputMethodHints inputHints, int spaceLeft )
{
    const auto result = m_data->engine->processKey( key, inputHints, spaceLeft );

    auto inputItem = m_data->inputItem;

    if ( result.key )
    {
        // sending a control key
        qskSendKey( inputItem, result.key );
    }
    else if ( !result.text.isEmpty() )
    {
        // changing the current text
        qskSendText( inputItem, result.text, result.isFinal );
    }
}

void QskInputPanel::processInputMethodQueries( Qt::InputMethodQueries queries )
{
    if ( m_data->inputItem == nullptr )
        return;

    /*
        adjust the input panel to information provided from the input item
     */

    QInputMethodQueryEvent queryEvent( queries );
    QCoreApplication::sendEvent( m_data->inputItem, &queryEvent );

    if ( queryEvent.queries() & Qt::ImHints )
    {
        /*
            ImhHiddenText = 0x1,          // might need to disable certain checks
            ImhSensitiveData = 0x2,       // shouldn't change anything
            ImhNoAutoUppercase = 0x4,     // if we support auto uppercase, disable it
            ImhPreferNumbers = 0x8,       // default to number keyboard
            ImhPreferUppercase = 0x10,    // start with shift on
            ImhPreferLowercase = 0x20,    // start with shift off
            ImhNoPredictiveText = 0x40,   // not use predictive text

            ImhDate = 0x80,               // ignored for now (no date keyboard)
            ImhTime = 0x100,              // ignored for know (no time keyboard)

            ImhPreferLatin = 0x200,       // can be used to launch chinese kb in english mode

            ImhMultiLine = 0x400,         // not useful?

            ImhDigitsOnly                 // default to number keyboard, disable other keys
            ImhFormattedNumbersOnly       // hard to say
            ImhUppercaseOnly              // caps-lock, disable shift
            ImhLowercaseOnly              // disable shift
            ImhDialableCharactersOnly     // dial pad (calculator?)
            ImhEmailCharactersOnly        // disable certain symbols (email-only kb?)
            ImhUrlCharactersOnly          // disable certain symbols (url-only kb?)
            ImhLatinOnly                  // disable chinese input
         */

#if 0
        const auto hints = static_cast< Qt::InputMethodHints >(
            queryEvent.value( Qt::ImHints ).toInt() );

#endif
    }

#if 0
    if ( queryEvent.queries() & Qt::ImPreferredLanguage )
    {
    }
#endif

    /*
        Qt::ImMicroFocus
        Qt::ImCursorRectangle
        Qt::ImFont
        Qt::ImCursorPosition
        Qt::ImSurroundingText // important for chinese input
        Qt::ImCurrentSelection // important for prediction
        Qt::ImMaximumTextLength // should be monitored
        Qt::ImAnchorPosition

        Qt::ImAbsolutePosition
        Qt::ImTextBeforeCursor // important for chinese
        Qt::ImTextAfterCursor  // important for chinese
        Qt::ImPlatformData     // hard to say...
        Qt::ImEnterKeyType
        Qt::ImAnchorRectangle
        Qt::ImInputItemClipRectangle // could be used for the geometry of the panel
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
