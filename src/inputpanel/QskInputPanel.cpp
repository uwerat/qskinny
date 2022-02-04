/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputPanel.h"
#include "QskInputContext.h"
#include "QskTextPredictor.h"

#include <qpointer.h>
#include <qtextformat.h>

namespace {
    struct Result
    {
        int key = 0;

        QString text;
        bool isFinal = true;
    };
}

static void qskRegisterInputPanel()
{
    qRegisterMetaType< Result >( "Result" );
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterInputPanel )

static inline QQuickItem* qskReceiverItem( const QskInputPanel* panel )
{
    if ( auto item = panel->inputProxy() )
        return item;

    return panel->inputItem();
}

static inline bool qskUsePrediction( Qt::InputMethodHints hints )
{
    constexpr Qt::InputMethodHints mask =
        Qt::ImhNoPredictiveText | Qt::ImhExclusiveInputMask | Qt::ImhHiddenText;

    return ( hints & mask ) == 0;
}

static inline void qskSendText(
    QQuickItem* receiver, const QString& text, bool isFinal )
{
    if ( receiver == nullptr )
        return;

    if ( isFinal )
    {
        QInputMethodEvent event;

        /*
            QQuickTextInput is buggy when receiving empty commit strings.
            We need to send a wrong replaceLength to work around it.
            See QTBUG: 68874
         */
        if ( text.isEmpty() )
            event.setCommitString( text, 0, 1 );
        else
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

static inline void qskSendReplaceText( QQuickItem* receiver, const QString& text )
{
    if ( receiver == nullptr )
        return;

    QInputMethodEvent::Attribute attribute(
        QInputMethodEvent::Selection, 0, 32767, QVariant() );

    QInputMethodEvent event( QString(), { attribute } );
    QCoreApplication::sendEvent( receiver, &event );

    qskSendText( receiver, text, true );
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

class KeyProcessor : public QObject
{
    Q_OBJECT

  public:
    QString preedit() const
    {
        return m_preedit;
    }

    void processKey(
        int key, Qt::InputMethodHints inputHints, QskInputPanel* panel,
        QskTextPredictor* predictor, int spaceLeft )
    {
        // reset:
        m_currentResult.isFinal = true;
        m_currentResult.text.clear();
        m_currentResult.key = 0;

        m_predictor = predictor;
        m_spaceLeft = spaceLeft;

        // First we have to handle the control keys

        switch ( key )
        {
            case Qt::Key_Backspace:
            case Qt::Key_Muhenkan:
            {
                if ( predictor && !m_preedit.isEmpty() )
                {
                    m_preedit.chop( 1 );

                    m_currentResult.text = m_preedit;
                    m_currentResult.isFinal = false;

                    Q_EMIT panel->predictionRequested( m_preedit );
                    // Let the input field update right away, otherwise
                    // we'll get weird effects with fast backspace presses:
                    Q_EMIT keyProcessingFinished( m_currentResult );
                    return;
                }
                else
                {
                    m_currentResult.key = Qt::Key_Backspace;
                    Q_EMIT keyProcessingFinished( m_currentResult );
                    return;
                }

                break;
            }
            case Qt::Key_Return:
            {
                if ( predictor )
                {
                    if ( !m_preedit.isEmpty() )
                    {
                        if ( spaceLeft )
                        {
                            m_currentResult.text = m_preedit.left( spaceLeft );
                            m_currentResult.isFinal = true;
                        }

                        reset();
                        Q_EMIT keyProcessingFinished( m_currentResult );
                        return;
                    }
                }

                if ( !( inputHints & Qt::ImhMultiLine ) )
                {
                    m_currentResult.key = Qt::Key_Return;
                    Q_EMIT keyProcessingFinished( m_currentResult );
                    return;
                }

                break;
            }
            case Qt::Key_Space:
            {
                if ( predictor )
                {
                    if ( !m_preedit.isEmpty() && spaceLeft )
                    {
                        m_preedit += keyString( key );
                        m_preedit = m_preedit.left( spaceLeft );

                        m_currentResult.text = m_preedit;
                        m_currentResult.isFinal = true;

                        reset();

                        Q_EMIT keyProcessingFinished( m_currentResult );
                        return;
                    }
                }

                break;
            }
            case Qt::Key_Left:
            case Qt::Key_Right:
            case Qt::Key_Escape:
            case Qt::Key_Cancel:
            {
                m_currentResult.key = key;
                Q_EMIT keyProcessingFinished( m_currentResult );
                return;
            }
        }

        const QString text = keyString( key );

        if ( predictor )
        {
            m_preedit += text;
            Q_EMIT panel->predictionRequested( m_preedit );
        }
        else
        {
            m_currentResult.text = text;
            m_currentResult.isFinal = true;
            Q_EMIT keyProcessingFinished( m_currentResult );
        }
    }

    void reset()
    {
        m_preedit.clear();
    }

    void continueProcessingKey( const QStringList& candidates )
    {
        if ( m_predictor )
        {
            if ( candidates.count() > 0 )
            {
                m_currentResult.text = m_preedit;
                m_currentResult.isFinal = false;
            }
            else
            {
                m_currentResult.text = m_preedit.left( m_spaceLeft );
                m_currentResult.isFinal = true;

                m_preedit.clear();
            }
        }

        Q_EMIT keyProcessingFinished( m_currentResult );
    }

  Q_SIGNALS:
    void keyProcessingFinished( const Result& result );

  private:
    inline QString keyString( int keyCode ) const
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

    QString m_preedit;
    int m_spaceLeft = -1;
    QskTextPredictor* m_predictor = nullptr;
    Result m_currentResult;
};


class QskInputPanel::PrivateData
{
  public:
    PrivateData( QskInputPanel* panel )
        : panel( panel )
    {
    }

    KeyProcessor keyProcessor;
    QPointer< QQuickItem > inputItem;

    QLocale predictorLocale;
    std::shared_ptr< QskTextPredictor > predictor;
    QStringList candidates;

    Qt::InputMethodHints inputHints;
    bool hasPredictorLocale = false;
    const QskInputPanel* panel;

    void handleKeyProcessingFinished( const Result& result )
    {
        switch ( result.key )
        {
            case 0:
            {
                qskSendText( qskReceiverItem( panel ),
                    result.text, result.isFinal );
                break;
            }
            case Qt::Key_Return:
            {
                if ( auto proxy = panel->inputProxy() )
                {
                    // using input method query instead
                    const auto value = proxy->property( "text" );
                    if ( value.canConvert< QString >() )
                    {
                        qskSendReplaceText( inputItem, value.toString() );
                    }
                }

                qskSendKey( inputItem, result.key );
                break;
            }
            case Qt::Key_Escape:
            case Qt::Key_Cancel:
            {
                qskSendKey( inputItem, result.key );
                break;
            }
            default:
            {
                qskSendKey( qskReceiverItem( panel ), result.key );
            }
        }
    }
};

QskInputPanel::QskInputPanel( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData( this ) )
{
    setAutoLayoutChildren( true );
    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Constrained );

    connect( this, &QskInputPanel::keySelected,
        this, &QskInputPanel::commitKey );

    connect( this, &QskInputPanel::predictiveTextSelected,
        this, &QskInputPanel::commitPredictiveText );

    connect( this, &QskControl::localeChanged,
        this, &QskInputPanel::updateLocale );

    connect( &m_data->keyProcessor, &KeyProcessor::keyProcessingFinished,
             this, [this]( const Result& result )
    {
        m_data->handleKeyProcessingFinished( result );
    } );

    updateLocale( locale() );
}

QskInputPanel::~QskInputPanel()
{
}

void QskInputPanel::attachInputItem( QQuickItem* item )
{
    if ( item == m_data->inputItem )
        return;

    if ( m_data->inputItem )
    {
        disconnect( m_data->inputItem, &QObject::destroyed,
            this, &QskInputPanel::inputItemDestroyed );
    }

    m_data->inputItem = item;

    if ( item )
    {
        if ( m_data->predictor )
            Q_EMIT predictionReset();

        m_data->keyProcessor.reset();
        m_data->inputHints = Qt::InputMethodHints();

        attachItem( item );

        Qt::InputMethodQueries queries = Qt::ImQueryAll;
        queries &= ~Qt::ImEnabled;

        updateInputPanel( queries );

        if ( inputProxy() )
        {
            /*
                Hiding the cursor in item. We use postEvent
                so that everything on the item is done,
                when receiving the event.
             */
            const QInputMethodEvent::Attribute attribute(
                QInputMethodEvent::Cursor, 0, 0, QVariant() );

            QCoreApplication::postEvent( item,
                new QInputMethodEvent( QString(), { attribute } ) );
        }

        connect( item, &QObject::destroyed,
            this, &QskInputPanel::inputItemDestroyed,
            Qt::UniqueConnection );
    }
    else
    {
        attachItem( nullptr );
    }
}

void QskInputPanel::updateInputPanel( Qt::InputMethodQueries queries )
{
    if ( m_data->inputItem == nullptr )
        return;

    QInputMethodQueryEvent event( queries );
    QCoreApplication::sendEvent( m_data->inputItem, &event );

    if ( queries & Qt::ImHints )
    {
        m_data->inputHints = static_cast< Qt::InputMethodHints >(
            event.value( Qt::ImHints ).toInt() );

        setPredictionEnabled(
            m_data->predictor && qskUsePrediction( m_data->inputHints ) );
    }

    if ( queries & Qt::ImPreferredLanguage )
    {
        setLocale( event.value( Qt::ImPreferredLanguage ).toLocale() );
    }

#if QT_VERSION >= QT_VERSION_CHECK( 5, 7, 0 )
    if ( queries & Qt::ImInputItemClipRectangle )
    {
        /*
            Could be used to move the panel,
            so that it does not hide the item.
         */
    }
#endif
}

void QskInputPanel::updateLocale( const QLocale& locale )
{
    if ( !m_data->hasPredictorLocale || locale != m_data->predictorLocale )
    {
        m_data->hasPredictorLocale = true;
        m_data->predictorLocale = locale;

        resetPredictor( locale );
        m_data->keyProcessor.reset();
    }
}

void QskInputPanel::resetPredictor( const QLocale& locale )
{
    auto predictor = QskInputContext::instance()->textPredictor( locale );

    if ( predictor == m_data->predictor )
        return;

    m_data->predictor = predictor;

    if ( predictor )
    {
        // text predictor lives in another thread, so these will all be QueuedConnections:
        connect( this, &QskInputPanel::predictionReset,
                 predictor.get(), &QskTextPredictor::reset );
        connect( this, &QskInputPanel::predictionRequested,
                 predictor.get(), &QskTextPredictor::request );

        connect( predictor.get(), &QskTextPredictor::predictionChanged,
            this, &QskInputPanel::updatePrediction );
    }

    setPredictionEnabled(
        predictor && qskUsePrediction( m_data->inputHints ) );
}

void QskInputPanel::commitPredictiveText( int index )
{
    QString text;

    if ( m_data->predictor )
    {
        text = m_data->candidates.at( index );
        Q_EMIT predictionReset();
    }

    m_data->keyProcessor.reset();

    setPrediction( {} );

    qskSendText( qskReceiverItem( this ), text, true );
}

void QskInputPanel::updatePrediction( const QString& text, const QStringList& candidates )
{
    if ( m_data->predictor )
    {
        if( m_data->keyProcessor.preedit() != text )
        {
            // This must be for another input panel
            return;
        }

        setPrediction( candidates );
        m_data->keyProcessor.continueProcessingKey( candidates );
    }
    else
    {
        qWarning() << "got prediction update, but no predictor. Something is wrong";
    }
}

QQuickItem* QskInputPanel::inputProxy() const
{
    return nullptr;
}

QQuickItem* QskInputPanel::inputItem() const
{
    return m_data->inputItem;
}

void QskInputPanel::setPrompt( const QString& )
{
}

void QskInputPanel::setPredictionEnabled( bool )
{
}

void QskInputPanel::setPrediction(const QStringList& candidates )
{
    m_data->candidates = candidates;
}

Qt::Alignment QskInputPanel::alignment() const
{
    /*
        When we have an input proxy, we don't care if
        the input item becomes hidden
     */

    return inputProxy() ? Qt::AlignVCenter : Qt::AlignBottom;
}

QStringList QskInputPanel::candidates() const
{
    return m_data->candidates;
}

void QskInputPanel::commitKey( int key )
{
    if ( m_data->inputItem == nullptr )
        return;

    int spaceLeft = -1;

    if ( !( m_data->inputHints & Qt::ImhMultiLine ) )
    {
        QInputMethodQueryEvent event1( Qt::ImMaximumTextLength );
        QCoreApplication::sendEvent( m_data->inputItem, &event1 );

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
        predictor = m_data->predictor.get(); // ### we could also make the predictor member of keyProcessor a shared ptr?

    m_data->keyProcessor.processKey(
        key, m_data->inputHints, this, predictor, spaceLeft );
}

#include "moc_QskInputPanel.cpp"
#include "QskInputPanel.moc"
