/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputPanel.h"
#include "QskVirtualKeyboard.h"
#include "QskInputSuggestionBar.h"
#include "QskTextInput.h"
#include "QskTextLabel.h"
#include "QskLinearBox.h"

#include <QString>
#include <QLocale>
#include <QGuiApplication>

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

    QskLinearBox* layout;
    QskTextLabel* prompt;
    TextInput* textInput;
    QskInputSuggestionBar* suggestionBar;
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

    m_data->textInput = new TextInput();
    m_data->textInput->setVisible( m_data->hasInputProxy );

    m_data->suggestionBar = new QskInputSuggestionBar();
    m_data->suggestionBar->setVisible( false );

    m_data->keyboard = new QskVirtualKeyboard();

    auto layout = new QskLinearBox( Qt::Vertical, this );

    layout->addItem( m_data->prompt, Qt::AlignLeft | Qt::AlignHCenter );
    layout->addItem( m_data->textInput, Qt::AlignLeft | Qt::AlignHCenter );
    layout->addStretch( 10 );
    layout->addItem( m_data->suggestionBar );
    layout->addItem( m_data->keyboard );

    m_data->layout = layout;

    connect( m_data->suggestionBar, &QskInputSuggestionBar::suggested,
        this, &QskInputPanel::commitCandidate );

    connect( m_data->keyboard, &QskVirtualKeyboard::keySelected,
        this, &QskInputPanel::commitKey );
}

QskInputPanel::~QskInputPanel()
{
}

QskAspect::Subcontrol QskInputPanel::effectiveSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskBox::Panel )
        return QskInputPanel::Panel;

    return subControl;
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
        { m_data->prompt, m_data->textInput, m_data->suggestionBar };

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
        { m_data->prompt, m_data->textInput, m_data->suggestionBar };

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
    m_data->textInput->setVisible( on );

    auto prompt = m_data->prompt;

    if ( on )
        prompt->setVisible( !prompt->text().isEmpty() );
    else
        prompt->setVisible( false );
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

void QskInputPanel::commitKey( int key )
{
    QGuiApplication::inputMethod()->invokeAction(
        static_cast< QInputMethod::Action >( Compose ), key );
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

#include "moc_QskInputPanel.cpp"
