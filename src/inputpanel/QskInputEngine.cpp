/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputEngine.h"
#include "QskTextPredictor.h"

#include <QPointer>
#include <QVector>
#include <QString>

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

class QskInputEngine::PrivateData
{
public:
    QPointer< QskTextPredictor > predictor;
    QString preedit;
};

QskInputEngine::QskInputEngine( QObject* parent ):
    QObject( parent ),
    m_data( new PrivateData() )
{
}

QskInputEngine::~QskInputEngine()
{
}

void QskInputEngine::setPredictor( QskTextPredictor* predictor )
{
    if ( predictor == m_data->predictor )
        return;

    reset();

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
            this, &QskInputEngine::predictionChanged );
    }

    m_data->predictor = predictor;
}

QskTextPredictor* QskInputEngine::predictor() const
{
    return m_data->predictor;
}

QVector< QString > QskInputEngine::prediction() const
{
    QVector< QString > candidates;

    if ( const auto predictor = m_data->predictor )
    {
        const auto count = predictor->candidateCount();
        candidates.reserve( count );

        for( int i = 0; i < count; i++ )
            candidates += predictor->candidate( i );
    }

    return candidates;
}

QskInputEngine::Result QskInputEngine::processKey( int key,
    Qt::InputMethodHints inputHints, int spaceLeft )
{
    QskInputEngine::Result result;

    auto& preedit = m_data->preedit;

    QskTextPredictor* predictor = nullptr;
    if ( !( inputHints & Qt::ImhHiddenText ) )
        predictor = m_data->predictor;

    /*
        First we have to handle the control keys
     */
    switch ( key )
    {
        case Qt::Key_Backspace:
        case Qt::Key_Muhenkan:
        {
            if ( predictor )
            {
                if ( !preedit.isEmpty() )
                {
                    preedit.chop( 1 );

                    result.text = preedit;
                    result.isFinal = false;

                    predictor->request( preedit );

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
                if ( !preedit.isEmpty() )
                {
                    if ( spaceLeft )
                    {
                        result.text = preedit.left( spaceLeft );
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
                if ( !preedit.isEmpty() && spaceLeft)
                {
                    preedit = preedit.left( spaceLeft );

                    result.text = preedit;
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
        preedit += text;

        predictor->request( preedit );

        if ( predictor->candidateCount() > 0 )
        {
            result.text = preedit;
            result.isFinal = false;
        }
        else
        {
            result.text = preedit.left( spaceLeft );
            result.isFinal = true;

            preedit.clear();
        }
    }
    else
    {
        result.text = text;
        result.isFinal = true;
    }

    return result;
}

QString QskInputEngine::predictiveText( int index ) const
{
    if ( m_data->predictor )
        return m_data->predictor->candidate( index );

    return QString();
}

void QskInputEngine::reset()
{
    if ( m_data->predictor )
        m_data->predictor->reset();

    m_data->preedit.clear();
}

#include "moc_QskInputEngine.cpp"
