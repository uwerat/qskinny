/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputEngine.h"
#include "QskInputContext.h"
#include "QskTextPredictor.h"
#include "QskControl.h"

#include <QPointer>
#include <QTextCharFormat>

static inline QQuickItem* qskReceiverItem( const QskInputEngine* engine )
{
    auto item = engine->inputProxy();
    return item ? item : engine->inputItem();
}

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

static inline bool qskUsePrediction( Qt::InputMethodHints hints )
{
    constexpr Qt::InputMethodHints mask =
        Qt::ImhNoPredictiveText | Qt::ImhExclusiveInputMask | Qt::ImhHiddenText;

    return ( hints & mask ) == 0;
}

static inline QString qskKeyString( int keyCode )
{
    // Special case entry codes here, else default to the symbol
    switch ( keyCode )
    {
        case Qt::Key_Shift:
        case Qt::Key_CapsLock:
        case Qt::Key_Mode_switch:
        case Qt::Key_Backspace:
        case Qt::Key_Muhenkan:
            return QString();

        case Qt::Key_Return:
        case Qt::Key_Kanji:
            return QChar( QChar::CarriageReturn );

        case Qt::Key_Space:
            return QChar( QChar::Space );

        default:
            break;
    }

    return QChar( keyCode );
}

namespace
{
    class KeyProcessor
    {
    public:
        class Result
        {
        public:
            int key = 0;

            QString text;
            bool isFinal = true;
        };

        Result processKey( int key, Qt::InputMethodHints inputHints,
            QskTextPredictor* predictor, int spaceLeft )
        {
            Result result;

            // First we have to handle the control keys

            switch ( key )
            {
                case Qt::Key_Backspace:
                case Qt::Key_Muhenkan:
                {
                    if ( predictor )
                    {
                        if ( !m_preedit.isEmpty() )
                        {
                            m_preedit.chop( 1 );

                            result.text = m_preedit;
                            result.isFinal = false;

                            predictor->request( m_preedit );

                            return result;
                        }
                    }

                    result.key = Qt::Key_Backspace;
                    return result;
                }
                case Qt::Key_Return:
                {
                    if ( predictor )
                    {
                        if ( !m_preedit.isEmpty() )
                        {
                            if ( spaceLeft )
                            {
                                result.text = m_preedit.left( spaceLeft );
                                result.isFinal = true;
                            }

                            reset();

                            return result;
                        }
                    }

                    if( !( inputHints & Qt::ImhMultiLine ) )
                    {
                        result.key = Qt::Key_Return;
                        return result;
                    }

                    break;
                }
                case Qt::Key_Space:
                {
                    if ( predictor )
                    {
                        if ( !m_preedit.isEmpty() && spaceLeft)
                        {
                            m_preedit += qskKeyString( key );
                            m_preedit = m_preedit.left( spaceLeft );

                            result.text = m_preedit;
                            result.isFinal = true;

                            reset();

                            return result;
                        }
                    }

                    break;
                }
                case Qt::Key_Left:
                case Qt::Key_Right:
                case Qt::Key_Escape:
                {
                    result.key = key;
                    return result;
                }
            }

            const QString text = qskKeyString( key );

            if ( predictor )
            {
                m_preedit += text;

                predictor->request( m_preedit );

                if ( predictor->candidateCount() > 0 )
                {
                    result.text = m_preedit;
                    result.isFinal = false;
                }
                else
                {
                    result.text = m_preedit.left( spaceLeft );
                    result.isFinal = true;

                    m_preedit.clear();
                }
            }
            else
            {
                result.text = text;
                result.isFinal = true;
            }

            return result;
        }

        void reset()
        {
            m_preedit.clear();
        }

    private:
        QString m_preedit;
    };
}

class QskInputEngine::PrivateData
{
public:
    KeyProcessor keyProcessor;
    QPointer< QskControl > panel;
    QPointer< QQuickItem > inputItem;

    QLocale predictorLocale;
    QPointer< QskTextPredictor > predictor;

    Qt::InputMethodHints inputHints = 0;
    bool hasPredictorLocale = false;
};

QskInputEngine::QskInputEngine( QObject* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
}

QskInputEngine::~QskInputEngine()
{
}

void QskInputEngine::attachInputItem( QQuickItem* item )
{
    if ( item == m_data->inputItem )
        return;

    m_data->inputItem = item;

    if ( item )
    {
        auto context = QskInputContext::instance();
        const auto locale = context->locale();

        if ( m_data->predictor )
            m_data->predictor->reset();

        m_data->keyProcessor.reset();
        m_data->inputHints = 0;

        attachToPanel( item );

        Qt::InputMethodQueries queries = Qt::ImQueryAll;
        queries &= ~Qt::ImEnabled;

        updateInputPanel( queries );
    }
    else
    {
        attachToPanel( nullptr );
    }
}

void QskInputEngine::updateInputPanel(
    Qt::InputMethodQueries queries )
{
    auto item = inputItem();
    if ( item == nullptr )
        return;

    QInputMethodQueryEvent event( queries );
    QCoreApplication::sendEvent( item, &event );

    if ( queries & Qt::ImHints )
    {
        m_data->inputHints = static_cast< Qt::InputMethodHints >(
            event.value( Qt::ImHints ).toInt() );

        updatePanel();
    }

    if ( queries & Qt::ImPreferredLanguage )
    {
        if ( m_data->panel )
        {
            m_data->panel->setLocale(
                event.value( Qt::ImPreferredLanguage ).toLocale() );
        }
    }
}

QskControl* QskInputEngine::panel( bool doCreate )
{
    if ( m_data->panel == nullptr && doCreate )
    {
        auto panel = createPanel();

        connect( panel, &QQuickItem::visibleChanged,
            this, &QskInputEngine::activeChanged );

        connect( panel, &QskControl::localeChanged,
            this, &QskInputEngine::updateLocale );

        m_data->panel = panel;
        updateLocale( m_data->panel->locale() );
    }

    return m_data->panel;
}

Qt::Alignment QskInputEngine::panelAlignment() const
{
    /*
        When we have an input proxy, we don't care if
        the input item becomes hidden
     */

    return inputProxy() ? Qt::AlignVCenter : Qt::AlignBottom;
}

QLocale QskInputEngine::locale() const
{
    if ( m_data->panel )
        return m_data->panel->locale();

    return QLocale();
}

void QskInputEngine::updateLocale( const QLocale& locale )
{
    if ( !m_data->hasPredictorLocale || locale != m_data->predictorLocale )
    {
        m_data->hasPredictorLocale = true;
        m_data->predictorLocale = locale;
        resetPredictor( locale );

        m_data->keyProcessor.reset();
        updatePanel();
    }

    Q_EMIT localeChanged();
}

void QskInputEngine::updatePanel()
{
    setPredictionEnabled(
        m_data->predictor && qskUsePrediction( m_data->inputHints ) );
}

QQuickItem* QskInputEngine::inputItem() const
{
    return m_data->inputItem;
}

QQuickItem* QskInputEngine::inputProxy() const
{
    return nullptr;
}

void QskInputEngine::resetPredictor( const QLocale& locale )
{
    auto predictor = QskInputContext::instance()->textPredictor( locale );

    if ( predictor == m_data->predictor )
        return;

    if ( m_data->predictor )
    {
        if ( m_data->predictor->parent() == this )
        {
            delete m_data->predictor;
        }
        else
        {
            m_data->predictor->disconnect( this );
            m_data->predictor = nullptr;
        }
    }

    if ( predictor )
    {
        if ( predictor->parent() == nullptr )
            predictor->setParent( this );

        connect( predictor, &QskTextPredictor::predictionChanged,
            this, &QskInputEngine::updatePrediction );
    }

    m_data->predictor = predictor;
}

void QskInputEngine::applyInput( bool success )
{
    auto item = inputItem();
    if ( item == nullptr )
        return;

    if ( success )
    {
        if ( auto proxy = inputProxy() )
        {
            const auto value = proxy->property( "text" );
            if ( value.canConvert< QString >() )
                qskSendReplaceText( item, value.toString() );
        }
    }

    qskSendKey( item, success ? Qt::Key_Return : Qt::Key_Escape );
}

void QskInputEngine::applyText( const QString& text, bool isFinal )
{
    qskSendText( qskReceiverItem( this ), text, isFinal );
}

void QskInputEngine::applyKey( int key )
{
    // control keys like left/right
    qskSendKey( qskReceiverItem( this ), key );
}

void QskInputEngine::commitPredictiveText( int index )
{
    QString text;

    if ( m_data->predictor )
    {
        text = m_data->predictor->candidate( index );
        m_data->predictor->reset();
    }

    m_data->keyProcessor.reset();

    showPrediction( QStringList() );
    applyText( text, true );
}

void QskInputEngine::updatePrediction()
{
    if ( m_data->predictor )
        showPrediction( m_data->predictor->candidates() );
}

void QskInputEngine::setPredictionEnabled( bool on )
{
    Q_UNUSED( on )
}

void QskInputEngine::showPrediction( const QStringList& )
{
}

void QskInputEngine::commitKey( int key )
{
    int spaceLeft = -1;

    if ( !( m_data->inputHints & Qt::ImhMultiLine ) )
    {
        QInputMethodQueryEvent event1( Qt::ImMaximumTextLength );
        QCoreApplication::sendEvent( inputItem(), &event1 );

        const int maxChars = event1.value( Qt::ImMaximumTextLength ).toInt();
        if ( maxChars >= 0 )
        {
            QInputMethodQueryEvent event2( Qt::ImSurroundingText );
            QCoreApplication::sendEvent( qskReceiverItem( this ), &event2 );

            const auto text = event2.value( Qt::ImSurroundingText ).toString();
            spaceLeft = maxChars - text.length();
        }
    }

    QskTextPredictor* predictor = nullptr;
    if ( qskUsePrediction( m_data->inputHints ) )
        predictor = m_data->predictor;

    const auto result = m_data->keyProcessor.processKey(
        key, m_data->inputHints, predictor, spaceLeft );

    if ( result.key )
    {
        switch( result.key )
        {
            case Qt::Key_Return:
            {
                applyInput( true );
                break;
            }
            case Qt::Key_Escape:
            {
                applyInput( false );
                break;
            }
            default:
            {
                applyKey( result.key );
            }
        }
    }
    else if ( !result.text.isEmpty() )
    {
        applyText( result.text, result.isFinal );
    }
}

#include "moc_QskInputEngine.cpp"
