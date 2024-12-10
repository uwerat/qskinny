/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextField.h"
#include "QskTextFieldSkinlet.h"
#include "QskFontRole.h"
#include "QskQuick.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquicktextinput_p.h>
#include <private/qquicktextinput_p_p.h>
QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskTextField, Panel )
QSK_SUBCONTROL( QskTextField, Text )
QSK_SUBCONTROL( QskTextField, PlaceholderText )

QSK_SYSTEM_STATE( QskTextField, ReadOnly, QskAspect::FirstSystemState << 1 )
QSK_SYSTEM_STATE( QskTextField, Editing, QskAspect::FirstSystemState << 2 )

static inline void qskPropagateReadOnly( QskTextField* input )
{
    Q_EMIT input->readOnlyChanged( input->isReadOnly() );

    QEvent event( QEvent::ReadOnlyChange );
    QCoreApplication::sendEvent( input, &event );
}

static inline void qskBindSignals(
    const QQuickTextInput* input, QskTextField* field )
{
    QObject::connect( input, &QQuickTextInput::textChanged,
        field, [ field ] { Q_EMIT field->textChanged( field->text() ); } );

    QObject::connect( input, &QQuickTextInput::displayTextChanged,
        field, [ field ] { Q_EMIT field->displayTextChanged( field->displayText() ); } );

    QObject::connect( input, &QQuickTextInput::textEdited,
        field, [ field ] { Q_EMIT field->textEdited( field->text() ); } );

    QObject::connect( input, &QQuickTextInput::validatorChanged,
        field, &QskTextField::validatorChanged );

    QObject::connect( input, &QQuickTextInput::inputMaskChanged,
        field, &QskTextField::inputMaskChanged );

    QObject::connect( input, &QQuickTextInput::readOnlyChanged,
        field, [ field ] { qskPropagateReadOnly( field ); } );

    QObject::connect( input, &QQuickTextInput::overwriteModeChanged,
        field, &QskTextField::overwriteModeChanged );

    QObject::connect( input, &QQuickTextInput::maximumLengthChanged,
        field, &QskTextField::maximumLengthChanged );

    QObject::connect( input, &QQuickTextInput::wrapModeChanged,
        field, [ field ] { Q_EMIT field->wrapModeChanged( field->wrapMode() ); } );

    QObject::connect( input, &QQuickTextInput::echoModeChanged,
        field, [ field ] { Q_EMIT field->echoModeChanged( field->echoMode() ); } );

    QObject::connect( input, &QQuickTextInput::passwordCharacterChanged,
        field, &QskTextField::passwordCharacterChanged );

    QObject::connect( input, &QQuickTextInput::passwordMaskDelayChanged,
        field, &QskTextField::passwordMaskDelayChanged );

    QObject::connect( input, &QQuickItem::implicitWidthChanged,
        field, &QskControl::resetImplicitSize );

    QObject::connect( input, &QQuickItem::implicitHeightChanged,
        field, &QskControl::resetImplicitSize );
}

namespace
{
    class TextInput final : public QQuickTextInput
    {
        using Inherited = QQuickTextInput;

      public:
        TextInput( QskTextField* );

        void setEditing( bool on );

        inline void setAlignment( Qt::Alignment alignment )
        {
            setHAlign( ( HAlignment ) ( int( alignment ) & 0x0f ) );
            setVAlign( ( VAlignment ) ( int( alignment ) & 0xf0 ) );
        }

        bool fixup()
        {
            return QQuickTextInputPrivate::get( this )->fixup();
        }

        bool hasAcceptableInput() const
        {
            /*
                we would like to call QQuickTextInputPrivate::hasAcceptableInput
                but unfortunately it is private, so we need to hack somthing
                together
             */

            auto that = const_cast< TextInput* >( this );
            auto d = QQuickTextInputPrivate::get( that );

            if ( d->m_validator )
            {
                QString text = displayText();
                int pos = d->m_cursor;

                const auto state = d->m_validator->validate( text, pos );
                if ( state != QValidator::Acceptable )
                    return false;
            }

            if ( d->m_maskData )
            {
                /*
                    We only want to do the check for the maskData here
                    and have to disable d->m_validator temporarily
                 */

                class Validator final : public QValidator
                {
                  public:
                    State validate( QString&, int& ) const override
                    {
                        return QValidator::Acceptable;
                    }
                };

                const auto validator = d->m_validator;

                const auto validInput = d->m_validInput;
                const auto acceptableInput = d->m_acceptableInput;

                d->m_acceptableInput = true;

                static Validator noValidator;
                that->setValidator( &noValidator ); // implicitly checking maskData
                that->setValidator( d->m_validator );

                const bool isAcceptable = d->m_acceptableInput;

                // restoring old values
                d->m_validInput = validInput;
                d->m_acceptableInput = acceptableInput;

                return isAcceptable;
            }

            return true;
        }

        void updateColors();
        void updateMetrics();

        inline bool handleEvent( QEvent* event )
        {
            return this->event( event );
        }

      protected:

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
        void geometryChange(
            const QRectF& newGeometry, const QRectF& oldGeometry ) override
        {
            Inherited::geometryChange( newGeometry, oldGeometry );
            updateClip();
        }
#else
        void geometryChanged(
            const QRectF& newGeometry, const QRectF& oldGeometry ) override
        {
            Inherited::geometryChanged( newGeometry, oldGeometry );
            updateClip();
        }
#endif

        void updateClip()
        {
            setClip( ( contentWidth() > width() ) ||
                ( contentHeight() > height() ) );
        }

        QSGNode* updatePaintNode(
            QSGNode* oldNode, UpdatePaintNodeData* data ) override
        {
            updateColors();
            return Inherited::updatePaintNode( oldNode, data );
        }
    };

    TextInput::TextInput( QskTextField* textField )
        : QQuickTextInput( textField )
    {
        classBegin();

        setActiveFocusOnTab( false );
        setFlag( ItemAcceptsInputMethod, false );
        setFocusOnPress( false );

        componentComplete();

        connect( this, &TextInput::contentSizeChanged,
            this, &TextInput::updateClip );
    }

    void TextInput::setEditing( bool on )
    {
        auto d = QQuickTextInputPrivate::get( this );

        if ( d->cursorVisible == on )
            return;

        setCursorVisible( on );
        d->setBlinkingCursorEnabled( on );

        if ( !on )
        {
            if ( d->m_passwordEchoEditing || d->m_passwordEchoTimer.isActive() )
                d->updatePasswordEchoEditing( false );
        }

        polish();
        update();
    }

    void TextInput::updateMetrics()
    {
        auto textField = static_cast< const QskTextField* >( parentItem() );

        setAlignment( textField->alignment() );
        setFont( textField->font() );
    }

    void TextInput::updateColors()
    {
        auto textField = static_cast< const QskTextField* >( parentItem() );
        auto d = QQuickTextInputPrivate::get( this );

        bool isDirty = false;

        QColor color;

        color = textField->color( QskTextField::Text );
        if ( d->color != color )
        {
            d->color = color;
            isDirty = true;
        }

        if ( d->hasSelectedText() )
        {
            color = textField->color( QskTextField::Panel | QskTextFieldSkinlet::Selected );
            if ( d->selectionColor != color )
            {
                d->selectionColor = color;
                isDirty = true;
            }

            color = textField->color( QskTextField::Text | QskTextFieldSkinlet::Selected );
            if ( d->selectedTextColor != color )
            {
                d->selectedTextColor = color;
                isDirty = true;
            }
        }

        if ( isDirty )
        {
            d->textLayoutDirty = true;
            d->updateType = QQuickTextInputPrivate::UpdatePaintNode;
            update();
        }
    }
}

class QskTextField::PrivateData
{
  public:
    TextInput* textInput;
    QString placeholderText;

    unsigned int activationModes : 3;
    bool hasPanel : 1;
};

QskTextField::QskTextField( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    m_data->activationModes = ActivationOnMouse | ActivationOnKey;
    m_data->hasPanel = true;

    setPolishOnResize( true );

    setAcceptHoverEvents( true );
    setFocusPolicy( Qt::StrongFocus );

    setFlag( QQuickItem::ItemAcceptsInputMethod );

    /*
        QQuickTextInput is a beast of almost 5k lines of code, we don't
        want to reimplement that - at least not now.
        So this is more or less a simple wrapper making everything
        conforming to qskinny.
     */

    m_data->textInput = new TextInput( this );
    qskBindSignals( m_data->textInput, this );

    setAcceptedMouseButtons( m_data->textInput->acceptedMouseButtons() );
    m_data->textInput->setAcceptedMouseButtons( Qt::NoButton );

    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Fixed );
}

QskTextField::QskTextField( const QString& text, QQuickItem* parent )
    : QskTextField( parent )
{
    m_data->textInput->setText( text );
}

QskTextField::~QskTextField()
{
}

void QskTextField::setPanel( bool on )
{
    if ( on != m_data->hasPanel )
    {
        m_data->hasPanel = on;

        resetImplicitSize();
        polish();
        update();
    }
}

bool QskTextField::hasPanel() const
{
    return m_data->hasPanel;
}

bool QskTextField::event( QEvent* event )
{
    if ( event->type() == QEvent::ShortcutOverride )
    {
        return m_data->textInput->handleEvent( event );
    }
    else if ( event->type() == QEvent::LocaleChange )
    {
        qskUpdateInputMethod( this, Qt::ImPreferredLanguage );
    }

    return Inherited::event( event );
}

void QskTextField::keyPressEvent( QKeyEvent* event )
{
    if ( isEditing() )
    {
        switch ( event->key() )
        {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            {
                if ( hasAcceptableInput() || fixup() )
                {
                    QGuiApplication::inputMethod()->commit();

                    if ( !( inputMethodHints() & Qt::ImhMultiLine ) )
                    {
                        setEditing( false );

                        // When returning from a virtual keyboard
                        qskForceActiveFocus( this, Qt::PopupFocusReason );
                    }
                }
                break;
            }
#if 1
            case Qt::Key_Escape:
            {
                setEditing( false );
                qskForceActiveFocus( this, Qt::PopupFocusReason );
                break;
            }
#endif
            default:
            {
                m_data->textInput->handleEvent( event );
            }
        }

        return;
    }

    if ( ( m_data->activationModes & ActivationOnKey ) && !event->isAutoRepeat() )
    {
        if ( event->key() == Qt::Key_Select || event->key() == Qt::Key_Space )
        {
            setEditing( true );
            return;
        }
    }

    Inherited::keyPressEvent( event );
}

void QskTextField::keyReleaseEvent( QKeyEvent* event )
{
    Inherited::keyReleaseEvent( event );
}

void QskTextField::mousePressEvent( QMouseEvent* event )
{
    m_data->textInput->handleEvent( event );

    if ( !isReadOnly() && !qGuiApp->styleHints()->setFocusOnTouchRelease() )
        setEditing( true );
}

void QskTextField::mouseMoveEvent( QMouseEvent* event )
{
    m_data->textInput->handleEvent( event );
}

void QskTextField::mouseReleaseEvent( QMouseEvent* event )
{
    m_data->textInput->handleEvent( event );

    if ( !isReadOnly() && qGuiApp->styleHints()->setFocusOnTouchRelease() )
        setEditing( true );
}

void QskTextField::mouseDoubleClickEvent( QMouseEvent* event )
{
    m_data->textInput->handleEvent( event );
}

void QskTextField::inputMethodEvent( QInputMethodEvent* event )
{
    m_data->textInput->handleEvent( event );
}

void QskTextField::focusInEvent( QFocusEvent* event )
{
    if ( m_data->activationModes & ActivationOnFocus )
    {
        switch ( event->reason() )
        {
            case Qt::ActiveWindowFocusReason:
            case Qt::PopupFocusReason:
                break;

            default:
#if 1
                // auto selecting the complete text ???
#endif
                setEditing( true );
        }
    }

    Inherited::focusInEvent( event );
}

void QskTextField::focusOutEvent( QFocusEvent* event )
{
    switch ( event->reason() )
    {
        case Qt::ActiveWindowFocusReason:
        case Qt::PopupFocusReason:
        {
            break;
        }
        default:
        {
            m_data->textInput->deselect();
            setEditing( false );
        }
    }

    Inherited::focusOutEvent( event );
}

void QskTextField::updateLayout()
{
    m_data->textInput->updateMetrics();
    qskSetItemGeometry( m_data->textInput, subControlRect( Text ) );
}

void QskTextField::updateNode( QSGNode* node )
{
    m_data->textInput->updateColors();
    Inherited::updateNode( node );
}

QString QskTextField::text() const
{
    return m_data->textInput->text();
}

void QskTextField::setText( const QString& text )
{
    m_data->textInput->setText( text );
}

void QskTextField::setPlaceholderText( const QString& text )
{
    if ( m_data->placeholderText != text )
    {
        m_data->placeholderText = text;
        Q_EMIT placeholderTextChanged( text );
    }
}

QString QskTextField::placeholderText() const
{
    return m_data->placeholderText;
}

QskTextField::ActivationModes QskTextField::activationModes() const
{
    return static_cast< QskTextField::ActivationModes >( m_data->activationModes );
}

void QskTextField::setActivationModes( ActivationModes modes )
{
    if ( static_cast< ActivationModes >( m_data->activationModes ) != modes )
    {
        m_data->activationModes = modes;
        Q_EMIT activationModesChanged();
    }
}

static inline void qskUpdateInputMethodFont( const QskTextField* input )
{
    const auto queries = Qt::ImCursorRectangle | Qt::ImFont | Qt::ImAnchorRectangle;
    qskUpdateInputMethod( input, queries );
}

void QskTextField::setFontRole( const QskFontRole& role )
{
    if ( setFontRoleHint( Text, role ) )
    {
        qskUpdateInputMethodFont( this );
        Q_EMIT fontRoleChanged();
    }
}

void QskTextField::resetFontRole()
{
    if ( resetFontRoleHint( Text ) )
    {
        qskUpdateInputMethodFont( this );
        Q_EMIT fontRoleChanged();
    }
}

QskFontRole QskTextField::fontRole() const
{
    return fontRoleHint( Text );
}

void QskTextField::setAlignment( Qt::Alignment alignment )
{
    if ( setAlignmentHint( Text, alignment ) )
    {
        m_data->textInput->setAlignment( alignment );
        Q_EMIT alignmentChanged();
    }
}

void QskTextField::resetAlignment()
{
    if ( resetAlignmentHint( Text ) )
    {
        m_data->textInput->setAlignment( alignment() );
        Q_EMIT alignmentChanged();
    }
}

Qt::Alignment QskTextField::alignment() const
{
    return alignmentHint( Text, Qt::AlignLeft | Qt::AlignTop );
}

void QskTextField::setWrapMode( QskTextOptions::WrapMode wrapMode )
{
    m_data->textInput->setWrapMode(
        static_cast< QQuickTextInput::WrapMode >( wrapMode ) );
}

QskTextOptions::WrapMode QskTextField::wrapMode() const
{
    return static_cast< QskTextOptions::WrapMode >(
        m_data->textInput->wrapMode() );
}

QFont QskTextField::font() const
{
    return effectiveFont( QskTextField::Text );
}

bool QskTextField::isReadOnly() const
{
    return m_data->textInput->isReadOnly();
}

void QskTextField::setReadOnly( bool on )
{
    auto input = m_data->textInput;

    if ( input->isReadOnly() == on )
        return;

#if 1
    // do we want to be able to restore the previous policy ?
    setFocusPolicy( Qt::NoFocus );
#endif

    input->setReadOnly( on );

    // we are killing user settings here ?
    input->setFlag( QQuickItem::ItemAcceptsInputMethod, !on );
    qskUpdateInputMethod( this, Qt::ImEnabled );

    setSkinStateFlag( ReadOnly, on );
}

void QskTextField::setEditing( bool on )
{
    if ( isReadOnly() || on == isEditing() )
        return;

    setSkinStateFlag( Editing, on );
    m_data->textInput->setEditing( on );

    if ( on )
    {
#if 0
        updateInputMethod( Qt::ImCursorRectangle | Qt::ImAnchorRectangle );
        QGuiApplication::inputMethod()->inputDirection
#endif
        qskInputMethodSetVisible( this, true );
    }
    else
    {
        if ( hasAcceptableInput() || fixup() )
            Q_EMIT m_data->textInput->editingFinished();

#if 0
        inputMethod->reset();
#endif
        qskInputMethodSetVisible( this, false );
    }

    Q_EMIT editingChanged( on );
}

bool QskTextField::isEditing() const
{
    return hasSkinState( Editing );
}

void QskTextField::ensureVisible( int position )
{
    m_data->textInput->ensureVisible( position );
}

int QskTextField::cursorPosition() const
{
    return m_data->textInput->cursorPosition();
}

void QskTextField::setCursorPosition( int pos )
{
    m_data->textInput->setCursorPosition( pos );
}

int QskTextField::maxLength() const
{
    return m_data->textInput->maxLength();
}

void QskTextField::setMaxLength( int length )
{
    m_data->textInput->setMaxLength( length );
}

QValidator* QskTextField::validator() const
{
    return m_data->textInput->validator();
}

void QskTextField::setValidator( QValidator* validator )
{
    m_data->textInput->setValidator( validator );
}

QString QskTextField::inputMask() const
{
    return m_data->textInput->inputMask();
}

void QskTextField::setInputMask( const QString& mask )
{
    m_data->textInput->setInputMask( mask );
}

QskTextField::EchoMode QskTextField::echoMode() const
{
    const auto mode = m_data->textInput->echoMode();
    return static_cast< QskTextField::EchoMode >( mode );
}

void QskTextField::setEchoMode( EchoMode mode )
{
    if ( mode != echoMode() )
    {
        m_data->textInput->setEchoMode(
            static_cast< QQuickTextInput::EchoMode >( mode ) );

        qskUpdateInputMethod( this, Qt::ImHints );
    }
}

QString QskTextField::passwordCharacter() const
{
    return m_data->textInput->passwordCharacter();
}

void QskTextField::setPasswordCharacter( const QString& text )
{
    m_data->textInput->setPasswordCharacter( text );
}

void QskTextField::resetPasswordCharacter()
{
    m_data->textInput->setPasswordCharacter(
        QGuiApplication::styleHints()->passwordMaskCharacter() );
}

int QskTextField::passwordMaskDelay() const
{
    return m_data->textInput->passwordMaskDelay();
}

void QskTextField::setPasswordMaskDelay( int ms )
{
    m_data->textInput->setPasswordMaskDelay( ms );
}

void QskTextField::resetPasswordMaskDelay()
{
    m_data->textInput->resetPasswordMaskDelay();
}

QString QskTextField::displayText() const
{
    return m_data->textInput->displayText();
}

QString QskTextField::preeditText() const
{
    const auto d = QQuickTextInputPrivate::get( m_data->textInput );
    return d->m_textLayout.preeditAreaText();
}

bool QskTextField::overwriteMode() const
{
    return m_data->textInput->overwriteMode();
}

void QskTextField::setOverwriteMode( bool overwrite )
{
    m_data->textInput->setOverwriteMode( overwrite );
}

bool QskTextField::hasAcceptableInput() const
{
    return m_data->textInput->hasAcceptableInput();
}

bool QskTextField::fixup()
{
    return m_data->textInput->fixup();
}

QVariant QskTextField::inputMethodQuery(
    Qt::InputMethodQuery property ) const
{
    return inputMethodQuery( property, QVariant() );
}

QVariant QskTextField::inputMethodQuery(
    Qt::InputMethodQuery query, const QVariant& argument ) const
{
    switch ( query )
    {
        case Qt::ImEnabled:
        {
            return QVariant( ( bool ) ( flags() & ItemAcceptsInputMethod ) );
        }
        case Qt::ImFont:
        {
            return font();
        }
        case Qt::ImPreferredLanguage:
        {
            return locale();
        }
        case Qt::ImInputItemClipRectangle:
        case Qt::ImCursorRectangle:
        {
            QVariant v = m_data->textInput->inputMethodQuery( query, argument );
#if 1
            if ( v.canConvert< QRectF >() )
                v.setValue( v.toRectF().translated( m_data->textInput->position() ) );
#endif
            return v;
        }
        default:
        {
            return m_data->textInput->inputMethodQuery( query, argument );
        }
    }
}

bool QskTextField::canUndo() const
{
    return m_data->textInput->canUndo();
}

bool QskTextField::canRedo() const
{
    return m_data->textInput->canRedo();
}

Qt::InputMethodHints QskTextField::inputMethodHints() const
{
    return m_data->textInput->inputMethodHints();
}

void QskTextField::setInputMethodHints( Qt::InputMethodHints hints )
{
    if ( m_data->textInput->inputMethodHints() != hints )
    {
        m_data->textInput->setInputMethodHints( hints );
        qskUpdateInputMethod( this, Qt::ImHints );
    }
}

void QskTextField::setupFrom( const QQuickItem* item )
{
    if ( item == nullptr )
        return;

    // finding attributes from the input hints of item

    int maxCharacters = 32767;
    QskTextField::EchoMode echoMode = QskTextField::Normal;

    Qt::InputMethodQueries queries = Qt::ImQueryAll;
    queries &= ~Qt::ImEnabled;

    QInputMethodQueryEvent event( queries );
    QCoreApplication::sendEvent( const_cast< QQuickItem* >( item ), &event );

    if ( event.queries() & Qt::ImHints )
    {
        const auto hints = static_cast< Qt::InputMethodHints >(
            event.value( Qt::ImHints ).toInt() );

        if ( hints & Qt::ImhHiddenText )
            echoMode = QskTextField::NoEcho;
    }

    if ( event.queries() & Qt::ImMaximumTextLength )
    {
        // needs to be handled before Qt::ImCursorPosition !

        const auto max = event.value( Qt::ImMaximumTextLength ).toInt();
        maxCharacters = qBound( 0, max, maxCharacters );
    }

    setMaxLength( maxCharacters );

    if ( event.queries() & Qt::ImSurroundingText )
    {
        const auto text = event.value( Qt::ImSurroundingText ).toString();
        setText( text );
    }

    if ( event.queries() & Qt::ImCursorPosition )
    {
        const auto pos = event.value( Qt::ImCursorPosition ).toInt();
        setCursorPosition( pos );
    }

    if ( event.queries() & Qt::ImCurrentSelection )
    {
#if 0
        const auto text = event.value( Qt::ImCurrentSelection ).toString();
        if ( !text.isEmpty() )
        {
        }
#endif
    }

    int passwordMaskDelay = -1;
    QString passwordCharacter;

    if ( echoMode == QskTextField::NoEcho )
    {
        /*
             Qt::ImhHiddenText does not provide information
             to decide between NoEcho/Password, or provides
             more details about how to deal with hidden inputs.
             So we try to find out more from trying some properties.
         */

        QVariant value;

        value = item->property( "passwordMaskDelay" );
        if ( value.canConvert< int >() )
            passwordMaskDelay = value.toInt();

        value = item->property( "passwordCharacter" );
        if ( value.canConvert< QString >() )
            passwordCharacter = value.toString();

        value = item->property( "echoMode" );
        if ( value.canConvert< int >() )
        {
            const auto mode = value.toInt();
            if ( mode == QskTextField::Password )
                echoMode = QskTextField::Password;
        }
    }

    if ( passwordMaskDelay >= 0 )
        setPasswordMaskDelay( passwordMaskDelay );
    else
        resetPasswordMaskDelay();

    if ( !passwordCharacter.isEmpty() )
        setPasswordCharacter( passwordCharacter );
    else
        resetPasswordCharacter();

    setEchoMode( echoMode );
}

#include "moc_QskTextField.cpp"
