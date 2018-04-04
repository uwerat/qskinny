/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputCompositionModel.h"
#include "QskInputContext.h"

#include <QGuiApplication>
#include <QInputMethodEvent>
#include <QTextCharFormat>

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

class QskInputCompositionModel::PrivateData
{
public:
    QString preedit;
    QTextCharFormat preeditFormat;
    QList< QInputMethodEvent::Attribute > preeditAttributes;

    int groupIndex = 0;
};

QskInputCompositionModel::QskInputCompositionModel( QskInputContext* context ):
    QObject( context ),
    m_data( new PrivateData )
{
    m_data->preeditFormat.setFontUnderline( true );
    m_data->preeditAttributes.append( QInputMethodEvent::Attribute(
            QInputMethodEvent::TextFormat, 0, 0, m_data->preeditFormat ) );
}

QskInputCompositionModel::~QskInputCompositionModel()
{
}

QskInputContext* QskInputCompositionModel::context() const
{
    return qobject_cast< QskInputContext* >( parent() );
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

    const auto queryEvent = context()->queryInputMethod(
        Qt::ImSurroundingText | Qt::ImMaximumTextLength | Qt::ImHints );

    const auto hints = static_cast< Qt::InputMethodHints >(
            queryEvent.value( Qt::ImHints ).toInt() );
    const int maxLength = queryEvent.value( Qt::ImMaximumTextLength ).toInt();
    const int currentLength = queryEvent.value( Qt::ImSurroundingText ).toString().length();

    int spaceLeft = -1;
    if ( !( hints & Qt::ImhMultiLine ) && maxLength > 0 )
        spaceLeft = maxLength - currentLength;

    switch ( key )
    {
        case Qt::Key_Backspace:
        case Qt::Key_Muhenkan:
        {
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
                sendKeyEvents( Qt::Key_Backspace );
            }
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
                sendKeyEvents( Qt::Key_Return );
            }

            return;
        }

        case Qt::Key_Left:
        case Qt::Key_Right:
        {
            if ( m_data->preedit.isEmpty() )
                sendKeyEvents( key );

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

void QskInputCompositionModel::sendCompositionEvent( QInputMethodEvent* event )
{
    context()->sendEventToInputItem( event );
}

void QskInputCompositionModel::sendKeyEvents( int key )
{
    auto context = this->context();

    QKeyEvent keyPress( QEvent::KeyPress, key, Qt::NoModifier );
    context->sendEventToInputItem( &keyPress );

    QKeyEvent keyRelease( QEvent::KeyRelease, key, Qt::NoModifier );
    context->sendEventToInputItem( &keyRelease );
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

bool QskInputCompositionModel::nextGroupIndex( int& index, bool forward ) const
{
    Q_UNUSED( index );
    Q_UNUSED( forward );
    return false;
}

#include "moc_QskInputCompositionModel.cpp"
