/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputPanel.h"
#include "QskInputContext.h"
#include "QskTextPredictor.h"

#include <qpointer.h>
#include <qtextformat.h>

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

namespace
{
    class KeyProcessor
    {
      public:
        struct Result
        {
            int key = 0;

            QString text;
            bool isFinal = true;
        };

        Result processKey(
            int key, Qt::InputMethodHints inputHints,
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

                    if ( !( inputHints & Qt::ImhMultiLine ) )
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
                        if ( !m_preedit.isEmpty() && spaceLeft )
                        {
                            m_preedit += keyString( key );
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

            const QString text = keyString( key );

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
    };
}

class QskInputPanel::PrivateData
{
  public:
    KeyProcessor keyProcessor;
    QPointer< QQuickItem > inputItem;

    QLocale predictorLocale;
    QPointer< QskTextPredictor > predictor;

    Qt::InputMethodHints inputHints;
    bool hasPredictorLocale = false;
};

QskInputPanel::QskInputPanel( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setAutoLayoutChildren( true );
    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Constrained );

    connect( this, &QskInputPanel::keySelected,
        this, &QskInputPanel::commitKey );

    connect( this, &QskInputPanel::predictiveTextSelected,
        this, &QskInputPanel::commitPredictiveText );

    connect( this, &QskControl::localeChanged,
        this, &QskInputPanel::updateLocale );

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
            m_data->predictor->reset();

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

    m_data->predictor = predictor;

    if ( predictor )
    {
        if ( predictor->parent() == nullptr )
            predictor->setParent( this );

        connect( predictor, &QskTextPredictor::predictionChanged,
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
        text = m_data->predictor->candidate( index );
        m_data->predictor->reset();
    }

    m_data->keyProcessor.reset();

    setPrediction( QStringList() );

    qskSendText( qskReceiverItem( this ), text, true );
}

void QskInputPanel::updatePrediction()
{
    if ( m_data->predictor )
        setPrediction( m_data->predictor->candidates() );
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

void QskInputPanel::setPrediction( const QStringList& )
{
}

Qt::Alignment QskInputPanel::alignment() const
{
    /*
        When we have an input proxy, we don't care if
        the input item becomes hidden
     */

    return inputProxy() ? Qt::AlignVCenter : Qt::AlignBottom;
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
        predictor = m_data->predictor;

    const auto result = m_data->keyProcessor.processKey(
        key, m_data->inputHints, predictor, spaceLeft );

    switch ( result.key )
    {
        case 0:
        {
            if ( !result.text.isEmpty() )
            {
                qskSendText( qskReceiverItem( this ),
                    result.text, result.isFinal );
            }
            break;
        }
        case Qt::Key_Return:
        {
            if ( auto proxy = inputProxy() )
            {
                // using input method query instead
                const auto value = proxy->property( "text" );
                if ( value.canConvert< QString >() )
                {
                    qskSendReplaceText( m_data->inputItem, value.toString() );
                }
            }

            qskSendKey( m_data->inputItem, result.key );
            break;
        }
        case Qt::Key_Escape:
        {
            qskSendKey( m_data->inputItem, result.key );
            break;
        }
        default:
        {
            qskSendKey( qskReceiverItem( this ), result.key );
        }
    }
}

#include "moc_QskInputPanel.cpp"
