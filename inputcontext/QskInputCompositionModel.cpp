/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputCompositionModel.h"

#include <QGuiApplication>
#include <QInputMethodEvent>
#include <QTextCharFormat>
#include <QWindow>

static inline QString qskKeyString( int code )
{
    // Special case entry codes here, else default to the symbol
    switch ( code )
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

    return QChar( code );
}

static inline void qskSendKeyEvents( QObject* receiver, int key )
{
    QKeyEvent keyPress( QEvent::KeyPress, key, Qt::NoModifier );
    QCoreApplication::sendEvent( receiver, &keyPress );

    QKeyEvent keyRelease( QEvent::KeyRelease, key, Qt::NoModifier );
    QCoreApplication::sendEvent( receiver, &keyRelease );
}

class QskInputCompositionModel::PrivateData
{
public:
    PrivateData() :
        inputItem( nullptr ),
        groupIndex( 0 )
    {
    }
    // QInputMethod
    QString preedit;
    QTextCharFormat preeditFormat;
    QList< QInputMethodEvent::Attribute > preeditAttributes;

    QObject* inputItem;
    int groupIndex;
};

QskInputCompositionModel::QskInputCompositionModel( QObject* parent ):
    QObject( parent ),
    m_data( new PrivateData )
{
    m_data->groupIndex = 0;

    m_data->preeditFormat.setFontUnderline( true );
    m_data->preeditAttributes.append( QInputMethodEvent::Attribute(
            QInputMethodEvent::TextFormat, 0, 0, m_data->preeditFormat ) );
}

QskInputCompositionModel::~QskInputCompositionModel()
{
}

bool QskInputCompositionModel::supportsSuggestions() const
{
    return false;
}

void QskInputCompositionModel::composeKey( Qt::Key key )
{
    /*
     * This operation might be expensive (e.g. for Hunspell) and
     * should be done asynchronously to be able to run e.g. suggestions
     * in a separate thread to not block the UI.
     * TODO
     */

    auto inputMethod = QGuiApplication::inputMethod();
    if ( !inputMethod )
        return;

    if ( !m_data->inputItem )
        return;

    QInputMethodQueryEvent queryEvent(
        Qt::ImSurroundingText | Qt::ImMaximumTextLength | Qt::ImHints );
    QCoreApplication::sendEvent( m_data->inputItem, &queryEvent );
    const auto hints = static_cast< Qt::InputMethodHints >(
            queryEvent.value( Qt::ImHints ).toInt() );
    const int maxLength = queryEvent.value( Qt::ImMaximumTextLength ).toInt();
    const int currentLength = queryEvent.value( Qt::ImSurroundingText ).toString().length();

    int spaceLeft = -1;
    if ( !( hints& Qt::ImhMultiLine ) && maxLength > 0 )
        spaceLeft = maxLength - currentLength;

    switch ( key )
    {
        case Qt::Key_Backspace:
        case Qt::Key_Muhenkan:
        {
            backspace();
            return;
        }
        case Qt::Key_Space:
        {
            if( !spaceLeft )
            {
                return;
            }

            if( !m_data->preedit.isEmpty() )
            {
                commit( m_data->preedit.left( spaceLeft ) );
            }

            commit( qskKeyString( key ) );
            return;
        }

        case Qt::Key_Return:
        {
            if ( !spaceLeft )
                return;

            // Commit what is in the buffer
            if( !m_data->preedit.isEmpty() )
            {
                commit( m_data->preedit.left( spaceLeft ) );
            }
            else if( hints & Qt::ImhMultiLine )
            {
                commit( qskKeyString( key ) );
            }
            else
            {
                if( auto focusWindow = QGuiApplication::focusWindow() )
                    qskSendKeyEvents( focusWindow, Qt::Key_Return );
            }

            return;
        }

        case Qt::Key_Left:
        case Qt::Key_Right:
        {
            moveCursor( key );
            return;
        }
        default:
        {
            if ( !spaceLeft )
                return;
        }
    }

    if ( hints & Qt::ImhHiddenText )
    {
        commit( qskKeyString( key ) );
        return;
    }

    const auto firstCandidate = candidateCount() > 0 ? candidate( 0 ) : QString();
    const auto oldPreedit = m_data->preedit;
    m_data->preedit += qskKeyString( key );
    auto displayPreedit = polishPreedit( m_data->preedit );

    // If there is no intermediate, decide between committing the first candidate and skipping
    if ( !hasIntermediate() )
    {
        // Skip preedit phase if there are no candidates/intermediates
        if ( firstCandidate.isEmpty() )
        {
            commit( oldPreedit.left( spaceLeft ) );
            spaceLeft -= oldPreedit.leftRef( spaceLeft ).length();
        }
        else
        {
            commit( firstCandidate );
            --spaceLeft;
        }

        if ( !spaceLeft )
            return;

        m_data->preedit = qskKeyString( key );
        displayPreedit = polishPreedit( m_data->preedit );

        if ( !hasIntermediate() )
        {
            commit( m_data->preedit );
            return;
        }
    }

    m_data->preeditAttributes.first().length = displayPreedit.length();

    QInputMethodEvent event( displayPreedit, m_data->preeditAttributes );
    sendCompositionEvent( &event );
}

void QskInputCompositionModel::clearPreedit()
{
    m_data->preedit.clear();
    polishPreedit( m_data->preedit );
}

int QskInputCompositionModel::candidateCount() const
{
    return 0;
}

QString QskInputCompositionModel::candidate( int ) const
{
    return QString();
}

// This method is called before displaying a new preedit string. It can be used
// to return a modified preedit string which is not stored as the underlying
// preedit text. This allows for marking up the preedit text without changing the
// data model. If the actual text needs to be modified, it is safe to call
// setPreeditText() here.
QString QskInputCompositionModel::polishPreedit( const QString& preedit )
{
    return preedit;
}

void QskInputCompositionModel::commit( const QString& text )
{
    QInputMethodEvent event( QString(), { } );
    event.setCommitString( text );
    sendCompositionEvent( &event );

    m_data->preedit.clear();
    polishPreedit( m_data->preedit );
}

void QskInputCompositionModel::commitCandidate( int index )
{
    commit( candidate( index ) );
}

void QskInputCompositionModel::backspace()
{
    if ( m_data->inputItem == nullptr )
        return;

    if ( !m_data->preedit.isEmpty() )
    {
        m_data->preedit.chop( 1 );

        const QString displayText = polishPreedit( m_data->preedit );
        m_data->preeditAttributes.first().length = displayText.length();

        QInputMethodEvent event( displayText, m_data->preeditAttributes );
        sendCompositionEvent( &event );
    }
    else
    {
        // Backspace one character only if preedit was inactive
        qskSendKeyEvents( m_data->inputItem, Qt::Key_Backspace );
    }
}

void QskInputCompositionModel::moveCursor( Qt::Key key )
{
    if ( key != Qt::Key_Left && key != Qt::Key_Right )
        return;

    if ( !m_data->inputItem )
        return;

    // Moving cursor is disabled when preedit is active.
    if ( !m_data->preedit.isEmpty() )
        return;

    QKeyEvent moveCursorPress( QEvent::KeyPress, key, Qt::NoModifier );
    QKeyEvent moveCursorRelease( QEvent::KeyRelease, key,  Qt::NoModifier );
#if 1
    QFocusEvent focusIn( QEvent::FocusIn ); // hack to display the cursor
#endif
    QCoreApplication::sendEvent( m_data->inputItem, &focusIn );
    QCoreApplication::sendEvent( m_data->inputItem, &moveCursorPress );
    QCoreApplication::sendEvent( m_data->inputItem, &moveCursorRelease );
}

void QskInputCompositionModel::sendCompositionEvent( QInputMethodEvent* e )
{
    if ( m_data->inputItem )
        QCoreApplication::sendEvent( m_data->inputItem, e );
}

bool QskInputCompositionModel::hasIntermediate() const
{
    return false;
}

bool QskInputCompositionModel::isComposable( const QStringRef& preedit ) const
{
    Q_UNUSED( preedit );
    return false;
}

int QskInputCompositionModel::groupIndex() const
{
    return m_data->groupIndex;
}

void QskInputCompositionModel::setGroupIndex( int groupIndex )
{
    if ( groupIndex == m_data->groupIndex )
        return;

    m_data->groupIndex = groupIndex;
    const QString displayText = polishPreedit( m_data->preedit );
    m_data->preeditAttributes.first().length = displayText.length();

    QInputMethodEvent event( displayText, m_data->preeditAttributes );
    sendCompositionEvent( &event );
}

QVector< Qt::Key > QskInputCompositionModel::groups() const
{
    return QVector< Qt::Key >();
}

void QskInputCompositionModel::setInputItem( QObject* inputItem )
{
    m_data->inputItem = inputItem;
}

bool QskInputCompositionModel::nextGroupIndex( int& index, bool forward ) const
{
    Q_UNUSED( index );
    Q_UNUSED( forward );
    return false;
}

#include "moc_QskInputCompositionModel.cpp"
