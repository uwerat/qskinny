/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputPanel.h"
#include "QskVirtualKeyboard.h"
#include "QskInputSuggestionBar.h"
#include "QskLinearBox.h"

#include <QString>
#include <QLocale>

QSK_QT_PRIVATE_BEGIN
#include <private/qinputmethod_p.h>
QSK_QT_PRIVATE_END

#include <qpa/qplatformintegration.h>
#include <qpa/qplatforminputcontext.h>

QString qskNativeLocaleString( const QLocale& locale )
{
    switch( locale.language() )
    {
        case QLocale::Bulgarian:
            return QStringLiteral( "български език" );

        case QLocale::Czech:
            return QStringLiteral( "Čeština" );

        case QLocale::German:
            return QStringLiteral( "Deutsch" );

        case QLocale::Danish:
            return QStringLiteral( "Dansk" );

        case QLocale::Greek:
            return QStringLiteral( "Eλληνικά" );

        case QLocale::English:
        {
            switch( locale.country() )
            {
                case QLocale::Canada:
                case QLocale::UnitedStates:
                case QLocale::UnitedStatesMinorOutlyingIslands:
                case QLocale::UnitedStatesVirginIslands:
                    return QStringLiteral( "English (US)" );

                default:
                    return QStringLiteral( "English (UK)" );
            }

            break;
        }

        case QLocale::Spanish:
            return QStringLiteral( "Español" );

        case QLocale::Finnish:
            return QStringLiteral( "Suomi" );

        case QLocale::French:
            return QStringLiteral( "Français" );

        case QLocale::Hungarian:
            return QStringLiteral( "Magyar" );

        case QLocale::Italian:
            return QStringLiteral( "Italiano" );

        case QLocale::Japanese:
            return QStringLiteral( "日本語" );

        case QLocale::Latvian:
            return QStringLiteral( "Latviešu" );

        case QLocale::Lithuanian:
            return QStringLiteral( "Lietuvių" );

        case QLocale::Dutch:
            return QStringLiteral( "Nederlands" );

        case QLocale::Portuguese:
            return QStringLiteral( "Português" );

        case QLocale::Romanian:
            return QStringLiteral( "Română" );

        case QLocale::Russia:
            return QStringLiteral( "Русский" );

        case QLocale::Slovenian:
            return QStringLiteral( "Slovenščina" );

        case QLocale::Slovak:
            return QStringLiteral( "Slovenčina" );

        case QLocale::Turkish:
            return QStringLiteral( "Türkçe" );

        case QLocale::Chinese:
            return QStringLiteral( "中文" );

        default:
            return QLocale::languageToString( locale.language() );
    }
}

static inline QQuickItem* qskInputItem()
{
    QPlatformInputContext* inputContext;
#if 1
    inputContext = QGuiApplicationPrivate::platformIntegration()->inputContext();
#else
    // for some reason the gcc sanitizer does not like this one
    inputContext = QInputMethodPrivate::get( inputMethod )->platformInputContext();
#endif

    QQuickItem* item = nullptr;

    QMetaObject::invokeMethod( inputContext, "inputItem",
        Qt::DirectConnection, Q_RETURN_ARG( QQuickItem*, item ) );

    return item;
}

static inline void qskInstallEventFilter( QskInputPanel* panel, bool on )
{
    if ( on )
        qGuiApp->installEventFilter( panel );
    else
        qGuiApp->removeEventFilter( panel );
}

class QskInputPanel::PrivateData
{
public:
    QskInputSuggestionBar* suggestionBar;
    QskVirtualKeyboard* keyboard;
};

QskInputPanel::QskInputPanel( QQuickItem* parent ):
    QskControl( parent ),
    m_data( new PrivateData() )
{
    setAutoLayoutChildren( true );
    setFlag( ItemIsFocusScope, true );
#if 0
    // TODO ...
    setTabFence( true );
#endif

    auto layout = new QskLinearBox( Qt::Vertical, this );

    m_data->suggestionBar = new QskInputSuggestionBar( layout );
    m_data->suggestionBar->setVisible( false );

    connect( m_data->suggestionBar, &QskInputSuggestionBar::suggested,
        this, &QskInputPanel::commitCandidate );

    m_data->keyboard = new QskVirtualKeyboard( layout );

    connect( m_data->keyboard, &QskVirtualKeyboard::keySelected,
        this, &QskInputPanel::commitKey );
}

QskInputPanel::~QskInputPanel()
{
}

bool QskInputPanel::isCandidatesEnabled() const
{
    return m_data->suggestionBar->isVisible();
}

QVector< QString > QskInputPanel::candidates() const
{
    return m_data->suggestionBar->candidates();
}

void QskInputPanel::setCandidatesEnabled( bool on )
{
    m_data->suggestionBar->setVisible( on );
}

void QskInputPanel::setCandidates( const QVector< QString >& candidates )
{
    m_data->suggestionBar->setCandidates( candidates );
}

void QskInputPanel::commitCandidate( int index )
{
    m_data->suggestionBar->setCandidates( QVector< QString >() );

    QGuiApplication::inputMethod()->invokeAction(
        static_cast< QInputMethod::Action >( SelectCandidate ), index );
}

void QskInputPanel::commitKey( Qt::Key key )
{
    QGuiApplication::inputMethod()->invokeAction(
        static_cast< QInputMethod::Action >( Compose ), key );
}

void QskInputPanel::updateLayout()
{
    if ( !isInitiallyPainted() )
        qskInstallEventFilter( this, isVisible() );

    Inherited::updateLayout();
}

void QskInputPanel::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    switch( change )
    {
        case QQuickItem::ItemVisibleHasChanged:
        case QQuickItem::ItemSceneChange:
        {
            if ( isInitiallyPainted() )
                qskInstallEventFilter( this, isVisible() );

            break;
        }
        default:
            break;
    }

    Inherited::itemChange( change, value );
}

bool QskInputPanel::eventFilter( QObject* object, QEvent* event )
{
    if ( event->type() == QEvent::InputMethodQuery )
    {
        const auto item = qskInputItem();

        /*
            Qt/Quick expects that the item associated with the input context
            always has the focus. But this does not work, when a virtual
            keyboard is used, where you can navigate and select inside.
            So we have to fix the receiver.

            Maybe QEvent::EnterEditFocus is good for something ??
         */

        if ( item && ( object != item ) && qskIsAncestorOf( this, item ) )
        {
            QGuiApplication::sendEvent( item, event );
            return true;
        }
    }

    return Inherited::eventFilter( object, event );
}

#include "moc_QskInputPanel.cpp"
