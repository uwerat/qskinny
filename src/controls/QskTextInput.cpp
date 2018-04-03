/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextInput.h"
#include "QskTextOptions.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquicktextinput_p.h>
QSK_QT_PRIVATE_END

#include <QDebug>

namespace
{
    class TextInput final : public QQuickTextInput
    {
    public:
        TextInput( QQuickItem* parent ) :
            QQuickTextInput( parent )
        {
        }

        void setAlignment( Qt::Alignment alignment )
        {
            setHAlign( ( HAlignment ) ( int( alignment ) & 0x0f ) );
            setVAlign( ( VAlignment ) ( int( alignment ) & 0xf0 ) );
        }

    protected:
        virtual void inputMethodEvent( QInputMethodEvent* event ) override
        {
            QQuickTextInput::inputMethodEvent( event );
        }
    };
}

QSK_SUBCONTROL( QskTextInput, Panel )
QSK_SUBCONTROL( QskTextInput, Text )

class QskTextInput::PrivateData
{
public:
    TextInput* textInput;
};

QskTextInput::QskTextInput( QQuickItem* parent ):
    QskTextInput( QString(), parent )
{
}

QskTextInput::QskTextInput( const QString& text, QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
    auto input = new TextInput( this );

    connect( input, &QQuickTextInput::textChanged,
        this, [ this ] { Q_EMIT textChanged( this->text() ); } );

    connect( input, &QQuickTextInput::textEdited,
        this, [ this ] { Q_EMIT textEdited( this->text() ); } );

    connect( input, &QQuickTextInput::textChanged,
        this, [ this ] { Q_EMIT textChanged( this->text() ); } );

    connect( input, &QQuickTextInput::selectedTextChanged,
        this, [ this ] { Q_EMIT selectedTextChanged( selectedText() ); } );

    connect( input, &QQuickTextInput::validatorChanged,
        this, &QskTextInput::validatorChanged );

    connect( input, &QQuickTextInput::inputMaskChanged,
        this, &QskTextInput::inputMaskChanged );

    connect( input, &QQuickTextInput::readOnlyChanged,
        this, &QskTextInput::readOnlyChanged );

    connect( input, &QQuickTextInput::overwriteModeChanged,
        this, &QskTextInput::overwriteModeChanged );

    connect( input, &QQuickTextInput::maximumLengthChanged,
        this, &QskTextInput::maximumLengthChanged );

    connect( input, &QQuickTextInput::echoModeChanged,
        this, [ this ] { Q_EMIT echoModeChanged( echoMode() ); } );

    connect( input, &QQuickTextInput::autoScrollChanged,
        this, &QskTextInput::autoScrollChanged );

    connect( input, &QQuickTextInput::selectByMouseChanged,
        this, &QskTextInput::selectByMouseChanged );

    connect( input, &QQuickTextInput::persistentSelectionChanged,
        this, &QskTextInput::persistentSelectionChanged );

    connect( input, &QQuickItem::implicitWidthChanged,
        this, &QskControl::resetImplicitSize );

    connect( input, &QQuickItem::implicitHeightChanged,
        this, &QskControl::resetImplicitSize );

    input->setAlignment( alignment() );
    input->setFont( font() );
    input->setText( text );

    m_data->textInput = input;

    setPolishOnResize( true );
    setFocusPolicy( Qt::StrongFocus );
#if 1
    input->setActiveFocusOnTab( true );
#endif
    setAcceptedMouseButtons( Qt::LeftButton );

    initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Fixed );
}

QskTextInput::~QskTextInput()
{
}

QSizeF QskTextInput::contentsSizeHint() const
{
    using namespace QskAspect;

    const qreal w = m_data->textInput->implicitWidth();
    const qreal h = m_data->textInput->implicitHeight();

    const QSizeF minSize( metric( Panel | MinimumWidth ),
        metric( Panel | MinimumHeight ) );

    return outerBoxSize( Panel, QSizeF( w, h ) ).expandedTo( minSize );
}

void QskTextInput::updateLayout()
{
    qskSetItemGeometry( m_data->textInput, subControlRect( Text ) );
}

QString QskTextInput::text() const
{
    return m_data->textInput->text();
}

void QskTextInput::setText( const QString& text )
{
    m_data->textInput->setText( text );
}

int QskTextInput::fontRole() const
{
    return QskSkinnable::fontRole( Text );
}

void QskTextInput::setFontRole( int role )
{
    const int oldRole = fontRole();

    QskSkinnable::setFontRole( effectiveSubcontrol( Text ), role );

    if ( oldRole != role )
    {
#if 1
        m_data->textInput->setFont( font() );
#endif
        Q_EMIT fontRoleChanged();
    }
}

QskTextOptions QskTextInput::textOptions() const
{
    return QskTextOptions();
}

void QskTextInput::setTextOptions( const QskTextOptions& options )
{
    Q_UNUSED( options )
}

void QskTextInput::setAlignment( Qt::Alignment alignment )
{
    if ( alignment != this->alignment() )
    {
        const auto subControl = effectiveSubcontrol( Text );
        setFlagHint( subControl | QskAspect::Alignment, alignment );

        m_data->textInput->setAlignment( alignment );

        Q_EMIT alignmentChanged();
    }
}

Qt::Alignment QskTextInput::alignment() const
{
    return flagHint< Qt::Alignment >(
        Text | QskAspect::Alignment, Qt::AlignLeft | Qt::AlignTop );
}

QFont QskTextInput::font() const
{
    return effectiveFont( QskTextInput::Text );
}

bool QskTextInput::isReadOnly() const
{
    return m_data->textInput->isReadOnly();
}

void QskTextInput::setReadOnly( bool on )
{
    m_data->textInput->setReadOnly( on );
}

bool QskTextInput::isCursorVisible() const
{
    return m_data->textInput->isCursorVisible();
}

void QskTextInput::setCursorVisible( bool on )
{
    m_data->textInput->setCursorVisible( on );
}

int QskTextInput::cursorPosition() const
{
    return m_data->textInput->cursorPosition();
}

void QskTextInput::setCursorPosition(int pos)
{
    m_data->textInput->setCursorPosition( pos );
}

int QskTextInput::selectionStart() const
{
    return m_data->textInput->selectionStart();
}

int QskTextInput::selectionEnd() const
{
    return m_data->textInput->selectionEnd();
}

QString QskTextInput::selectedText() const
{
    return m_data->textInput->selectedText();
}

int QskTextInput::maxLength() const
{
    return m_data->textInput->maxLength();
}

void QskTextInput::setMaxLength(int length)
{
    m_data->textInput->setMaxLength( length );
}

QValidator* QskTextInput::validator() const
{
    return m_data->textInput->validator();
}

void QskTextInput::setValidator( QValidator* validator )
{
    m_data->textInput->setValidator( validator );
}

QString QskTextInput::inputMask() const
{
    return m_data->textInput->inputMask();
}

void QskTextInput::setInputMask( const QString& mask )
{
    m_data->textInput->setInputMask( mask );
}

QskTextInput::EchoMode QskTextInput::echoMode() const
{
    const auto mode = m_data->textInput->echoMode();
    return static_cast< QskTextInput::EchoMode >( mode );
}

void QskTextInput::setEchoMode( EchoMode mode )
{
    m_data->textInput->setEchoMode(
        static_cast< QQuickTextInput::EchoMode >( mode ) );
}

QString QskTextInput::displayText() const
{
    return m_data->textInput->displayText();
}

QString QskTextInput::preeditText() const
{
    return m_data->textInput->preeditText();
}

bool QskTextInput::overwriteMode() const
{
    return m_data->textInput->overwriteMode();
}

void QskTextInput::setOverwriteMode( bool overwrite )
{
    m_data->textInput->setOverwriteMode( overwrite );
}

bool QskTextInput::autoScroll() const
{
    return m_data->textInput->autoScroll();
}

void QskTextInput::setAutoScroll(bool on)
{
    m_data->textInput->setAutoScroll( on );
}

bool QskTextInput::selectByMouse() const
{
    return m_data->textInput->selectByMouse();
}

void QskTextInput::setSelectByMouse(bool on)
{
    m_data->textInput->setSelectByMouse( on );
}

QskTextInput::SelectionMode QskTextInput::mouseSelectionMode() const
{
    const auto mode = m_data->textInput->mouseSelectionMode();
    return static_cast< SelectionMode >( mode );
}

void QskTextInput::setMouseSelectionMode( SelectionMode mode )
{
    m_data->textInput->setMouseSelectionMode(
        static_cast< QQuickTextInput::SelectionMode >( mode ) );
}

bool QskTextInput::persistentSelection() const
{
    return m_data->textInput->persistentSelection();
}

void QskTextInput::setPersistentSelection(bool persist)
{
    m_data->textInput->setPersistentSelection( persist );
}

bool QskTextInput::hasAcceptableInput() const
{
    return m_data->textInput->hasAcceptableInput();
}

QVariant QskTextInput::inputMethodQuery(
    Qt::InputMethodQuery property) const
{
    return m_data->textInput->inputMethodQuery( property );
}

QVariant QskTextInput::inputMethodQuery(
    Qt::InputMethodQuery query, QVariant argument) const
{
    return m_data->textInput->inputMethodQuery( query, argument );
}

bool QskTextInput::canUndo() const
{
    return m_data->textInput->canUndo();
}

bool QskTextInput::canRedo() const
{
    return m_data->textInput->canRedo();
}

bool QskTextInput::isInputMethodComposing() const
{
    return m_data->textInput->isInputMethodComposing();
}

Qt::InputMethodHints QskTextInput::inputMethodHints() const
{
    return m_data->textInput->inputMethodHints();
}

void QskTextInput::setInputMethodHints(Qt::InputMethodHints hints )
{
    m_data->textInput->setInputMethodHints( hints );
}

#include "moc_QskTextInput.cpp"
