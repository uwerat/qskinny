/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextInput.h"
#include "QskEvent.h"
#include "QskFontRole.h"
#include "QskQuick.h"
#include "QskTextInputSkinlet.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquicktextinput_p.h>
#include <private/qquicktextinput_p_p.h>
QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskTextInput, Panel )
QSK_SUBCONTROL( QskTextInput, LeadingIcon )
QSK_SUBCONTROL( QskTextInput, LabelText )
QSK_SUBCONTROL( QskTextInput, InputText )
QSK_SUBCONTROL( QskTextInput, TrailingIconRipple )
QSK_SUBCONTROL( QskTextInput, TrailingIcon )
QSK_SUBCONTROL( QskTextInput, HintText )
QSK_SUBCONTROL( QskTextInput, SupportingText )
QSK_SUBCONTROL( QskTextInput, CharacterCount )

QSK_SYSTEM_STATE( QskTextInput, ReadOnly, QskAspect::FirstSystemState << 1 )
QSK_SYSTEM_STATE( QskTextInput, Editing, QskAspect::FirstSystemState << 2 )
QSK_SYSTEM_STATE( QskTextInput, Selected, QskAspect::FirstSystemState << 3 )
QSK_SYSTEM_STATE( QskTextInput, Error, QskAspect::FirstSystemState << 4 )
QSK_SYSTEM_STATE( QskTextInput, TextPopulated, QskAspect::LastUserState << 1 )

static inline void qskPropagateReadOnly( QskTextInput* input )
{
    Q_EMIT input->readOnlyChanged( input->isReadOnly() );

    QEvent event( QEvent::ReadOnlyChange );
    QCoreApplication::sendEvent( input, &event );
}

static inline void qskBindSignals(
    const QQuickTextInput* wrappedInput, QskTextInput* input )
{
    QObject::connect( wrappedInput, &QQuickTextInput::textChanged,
        input, [ input ] { Q_EMIT input->inputTextChanged( input->inputText() ); } );

    QObject::connect( wrappedInput, &QQuickTextInput::displayTextChanged,
        input, [ input ] { Q_EMIT input->displayTextChanged( input->displayText() ); } );

    QObject::connect( wrappedInput, &QQuickTextInput::textEdited,
        input, [ input ] { Q_EMIT input->textEdited( input->inputText() ); } );

    QObject::connect( wrappedInput, &QQuickTextInput::validatorChanged,
        input, &QskTextInput::validatorChanged );

    QObject::connect( wrappedInput, &QQuickTextInput::inputMaskChanged,
        input, &QskTextInput::inputMaskChanged );

    QObject::connect( wrappedInput, &QQuickTextInput::readOnlyChanged,
        input, [ input ] { qskPropagateReadOnly( input ); } );

    QObject::connect( wrappedInput, &QQuickTextInput::overwriteModeChanged,
        input, &QskTextInput::overwriteModeChanged );

    QObject::connect( wrappedInput, &QQuickTextInput::maximumLengthChanged,
        input, &QskTextInput::maximumLengthChanged );

    QObject::connect( wrappedInput, &QQuickTextInput::echoModeChanged,
        input, [ input ] { Q_EMIT input->echoModeChanged( input->echoMode() ); } );

    QObject::connect( wrappedInput, &QQuickTextInput::passwordCharacterChanged,
        input, &QskTextInput::passwordCharacterChanged );

    QObject::connect( wrappedInput, &QQuickTextInput::passwordMaskDelayChanged,
        input, &QskTextInput::passwordMaskDelayChanged );

    QObject::connect( wrappedInput, &QQuickItem::implicitWidthChanged,
        input, &QskControl::resetImplicitSize );

    QObject::connect( wrappedInput, &QQuickItem::implicitHeightChanged,
        input, &QskControl::resetImplicitSize );
}

namespace
{
    class TextInput final : public QQuickTextInput
    {
        using Inherited = QQuickTextInput;

      public:
        TextInput( QskTextInput* );

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

    TextInput::TextInput( QskTextInput* textInput )
        : QQuickTextInput( textInput )
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
        auto input = static_cast< const QskTextInput* >( parentItem() );

        setAlignment( input->alignment() );
        setFont( input->font() );
    }

    void TextInput::updateColors()
    {
        auto input = static_cast< const QskTextInput* >( parentItem() );
        auto d = QQuickTextInputPrivate::get( this );

        bool isDirty = false;

        QColor color;

        color = input->color( QskTextInput::InputText );
        if ( d->color != color )
        {
            d->color = color;
            isDirty = true;
        }

        if ( d->hasSelectedText() )
        {
            color = input->color( QskTextInput::Panel | QskTextInput::Selected );
            if ( d->selectionColor != color )
            {
                d->selectionColor = color;
                isDirty = true;
            }

            color = input->color( QskTextInput::InputText | QskTextInput::Selected );
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

class QskTextInput::PrivateData
{
  public:
    PrivateData()
        : emphasis( NoEmphasis )
    {
    }

    TextInput* textInput;
    QString labelText;
    QString hintText;
    QString supportingText;

    unsigned int activationModes : 3;
    bool hasPanel : 1;
    int emphasis : 4;
};

QskTextInput::QskTextInput( QQuickItem* parent )
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

    connect( m_data->textInput, &QQuickTextInput::textChanged, this, [this]()
    {
        setSkinStateFlag( TextPopulated, !m_data->textInput->text().isEmpty() );
        update(); // character count might have changed
    } );
}

QskTextInput::QskTextInput( const QString& text, QQuickItem* parent )
    : QskTextInput( parent )
{
    setInputText( text );
}

QskTextInput::~QskTextInput()
{
}

void QskTextInput::setPanel( bool on )
{
    if ( on != m_data->hasPanel )
    {
        m_data->hasPanel = on;

        resetImplicitSize();
        polish();
        update();
    }
}

bool QskTextInput::hasPanel() const
{
    return m_data->hasPanel;
}

bool QskTextInput::event( QEvent* event )
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

void QskTextInput::keyPressEvent( QKeyEvent* event )
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

void QskTextInput::keyReleaseEvent( QKeyEvent* event )
{
    Inherited::keyReleaseEvent( event );
}

void QskTextInput::mousePressEvent( QMouseEvent* event )
{
    if( !isReadOnly() && subControlContentsRect( TrailingIcon ).contains( event->pos() ) )
    {
        setInputText( {} );
    }
    else
    {
        m_data->textInput->handleEvent( event );
    }

    if ( !isReadOnly() && !qGuiApp->styleHints()->setFocusOnTouchRelease() )
        setEditing( true );
}

void QskTextInput::mouseMoveEvent( QMouseEvent* event )
{
    m_data->textInput->handleEvent( event );
}

void QskTextInput::mouseReleaseEvent( QMouseEvent* event )
{
    m_data->textInput->handleEvent( event );

    if ( !isReadOnly() && qGuiApp->styleHints()->setFocusOnTouchRelease() )
        setEditing( true );
}

void QskTextInput::mouseDoubleClickEvent( QMouseEvent* event )
{
    m_data->textInput->handleEvent( event );
}

void QskTextInput::inputMethodEvent( QInputMethodEvent* event )
{
    m_data->textInput->handleEvent( event );
}

void QskTextInput::focusInEvent( QFocusEvent* event )
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

void QskTextInput::focusOutEvent( QFocusEvent* event )
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

void QskTextInput::hoverEnterEvent( QHoverEvent* event )
{
    using A = QskAspect;

    setSkinHint( TrailingIconRipple | Hovered | A::Metric | A::Position, qskHoverPosition( event ) );
    update();

    Inherited::hoverEnterEvent( event );
}

void QskTextInput::hoverMoveEvent( QHoverEvent* event )
{
    using A = QskAspect;

    setSkinHint( TrailingIconRipple | Hovered | A::Metric | A::Position, qskHoverPosition( event ) );
    update();

    Inherited::hoverMoveEvent( event );
}

void QskTextInput::hoverLeaveEvent( QHoverEvent* event )
{
    using A = QskAspect;

    setSkinHint( TrailingIconRipple | Hovered | A::Metric | A::Position, QPointF() );
    update();

    Inherited::hoverLeaveEvent( event );
}

QSizeF QskTextInput::layoutSizeHint( Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    auto input = m_data->textInput;

    input->updateMetrics();

    QSizeF hint( input->implicitWidth(), input->implicitHeight() );

    if ( m_data->hasPanel )
    {
        hint = outerBoxSize( Panel, hint );
        hint = hint.expandedTo( strutSizeHint( Panel ) );
    }

    const auto textInputSkinlet = static_cast< const QskTextInputSkinlet* >( effectiveSkinlet() );
    return textInputSkinlet->adjustSizeHint( this, which, hint );
}

void QskTextInput::updateLayout()
{
    auto input = m_data->textInput;

    input->updateMetrics();
    qskSetItemGeometry( input, subControlRect( InputText ) );
}

void QskTextInput::updateNode( QSGNode* node )
{
    m_data->textInput->updateColors();
    m_data->textInput->updateMetrics();
    Inherited::updateNode( node );
}

void QskTextInput::setEmphasis( Emphasis emphasis )
{
    if ( emphasis != m_data->emphasis )
    {
        m_data->emphasis = emphasis;

        resetImplicitSize();
        update();

        Q_EMIT emphasisChanged( emphasis );
    }
}

QskTextInput::Emphasis QskTextInput::emphasis() const
{
    return static_cast< Emphasis >( m_data->emphasis );
}

QString QskTextInput::inputText() const
{
    return m_data->textInput->text();
}

void QskTextInput::setInputText( const QString& text )
{
    m_data->textInput->setText( text );
}

QString QskTextInput::labelText() const
{
    return m_data->labelText;
}

void QskTextInput::setLabelText( const QString& text )
{
    if ( m_data->labelText != text )
    {
        m_data->labelText = text;
        Q_EMIT labelTextChanged( text );
    }
}

QskGraphic QskTextInput::leadingIcon() const
{
    return symbolHint( LeadingIcon );
}

void QskTextInput::setLeadingIcon( const QskGraphic& icon )
{
    setSymbolHint( LeadingIcon, icon );
}

void QskTextInput::setHintText( const QString& text )
{
    if ( m_data->hintText != text )
    {
        m_data->hintText = text;
        Q_EMIT hintTextChanged( text );
    }
}

QString QskTextInput::hintText() const
{
    return m_data->hintText;
}

void QskTextInput::setSupportingText( const QString& text )
{
    if ( m_data->supportingText != text )
    {
        m_data->supportingText = text;
        Q_EMIT supportingTextChanged( text );
    }
}

QString QskTextInput::supportingText() const
{
    return m_data->supportingText;
}

QskTextInput::ActivationModes QskTextInput::activationModes() const
{
    return static_cast< QskTextInput::ActivationModes >( m_data->activationModes );
}

void QskTextInput::setActivationModes( ActivationModes modes )
{
    if ( static_cast< ActivationModes >( m_data->activationModes ) != modes )
    {
        m_data->activationModes = modes;
        Q_EMIT activationModesChanged();
    }
}

static inline void qskUpdateInputMethodFont( const QskTextInput* input )
{
    const auto queries = Qt::ImCursorRectangle | Qt::ImFont | Qt::ImAnchorRectangle;
    qskUpdateInputMethod( input, queries );
}

void QskTextInput::setFontRole( const QskFontRole& role )
{
    if ( setFontRoleHint( InputText, role ) )
    {
        qskUpdateInputMethodFont( this );
        Q_EMIT fontRoleChanged();
    }
}

void QskTextInput::resetFontRole()
{
    if ( resetFontRoleHint( InputText ) )
    {
        qskUpdateInputMethodFont( this );
        Q_EMIT fontRoleChanged();
    }
}

QskFontRole QskTextInput::fontRole() const
{
    return fontRoleHint( InputText );
}

void QskTextInput::setAlignment( Qt::Alignment alignment )
{
    if ( setAlignmentHint( InputText, alignment ) )
    {
        m_data->textInput->setAlignment( alignment );
        Q_EMIT alignmentChanged();
    }
}

void QskTextInput::resetAlignment()
{
    if ( resetAlignmentHint( InputText ) )
    {
        m_data->textInput->setAlignment( alignment() );
        Q_EMIT alignmentChanged();
    }
}

Qt::Alignment QskTextInput::alignment() const
{
    return alignmentHint( InputText, Qt::AlignLeft | Qt::AlignTop );
}

QFont QskTextInput::font() const
{
    return effectiveFont( QskTextInput::InputText );
}

bool QskTextInput::isReadOnly() const
{
    return m_data->textInput->isReadOnly();
}

void QskTextInput::setReadOnly( bool on )
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

void QskTextInput::setEditing( bool on )
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

bool QskTextInput::isEditing() const
{
    return hasSkinState( Editing );
}

void QskTextInput::ensureVisible( int position )
{
    m_data->textInput->ensureVisible( position );
}

QskAspect::Variation QskTextInput::effectiveVariation() const
{
    switch( m_data->emphasis )
    {
        case LowEmphasis:
            return QskAspect::Small;

        default:
            return QskAspect::NoVariation;
    }
}

int QskTextInput::cursorPosition() const
{
    return m_data->textInput->cursorPosition();
}

void QskTextInput::setCursorPosition( int pos )
{
    m_data->textInput->setCursorPosition( pos );
}

int QskTextInput::maxLength() const
{
    return m_data->textInput->maxLength();
}

void QskTextInput::setMaxLength( int length )
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
    if ( mode != echoMode() )
    {
        m_data->textInput->setEchoMode(
            static_cast< QQuickTextInput::EchoMode >( mode ) );

        qskUpdateInputMethod( this, Qt::ImHints );
    }
}

QString QskTextInput::passwordCharacter() const
{
    return m_data->textInput->passwordCharacter();
}

void QskTextInput::setPasswordCharacter( const QString& text )
{
    m_data->textInput->setPasswordCharacter( text );
}

void QskTextInput::resetPasswordCharacter()
{
    m_data->textInput->setPasswordCharacter(
        QGuiApplication::styleHints()->passwordMaskCharacter() );
}

int QskTextInput::passwordMaskDelay() const
{
    return m_data->textInput->passwordMaskDelay();
}

void QskTextInput::setPasswordMaskDelay( int ms )
{
    m_data->textInput->setPasswordMaskDelay( ms );
}

void QskTextInput::resetPasswordMaskDelay()
{
    m_data->textInput->resetPasswordMaskDelay();
}

QString QskTextInput::displayText() const
{
    return m_data->textInput->displayText();
}

QString QskTextInput::preeditText() const
{
    const auto d = QQuickTextInputPrivate::get( m_data->textInput );
    return d->m_textLayout.preeditAreaText();
}

bool QskTextInput::overwriteMode() const
{
    return m_data->textInput->overwriteMode();
}

void QskTextInput::setOverwriteMode( bool overwrite )
{
    m_data->textInput->setOverwriteMode( overwrite );
}

bool QskTextInput::hasAcceptableInput() const
{
    return m_data->textInput->hasAcceptableInput();
}

bool QskTextInput::fixup()
{
    return m_data->textInput->fixup();
}

QVariant QskTextInput::inputMethodQuery(
    Qt::InputMethodQuery property ) const
{
    return inputMethodQuery( property, QVariant() );
}

QVariant QskTextInput::inputMethodQuery(
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

bool QskTextInput::canUndo() const
{
    return m_data->textInput->canUndo();
}

bool QskTextInput::canRedo() const
{
    return m_data->textInput->canRedo();
}

Qt::InputMethodHints QskTextInput::inputMethodHints() const
{
    return m_data->textInput->inputMethodHints();
}

void QskTextInput::setInputMethodHints( Qt::InputMethodHints hints )
{
    if ( m_data->textInput->inputMethodHints() != hints )
    {
        m_data->textInput->setInputMethodHints( hints );
        qskUpdateInputMethod( this, Qt::ImHints );
    }
}

void QskTextInput::setupFrom( const QQuickItem* item )
{
    if ( item == nullptr )
        return;

    // finding attributes from the input hints of item

    int maxCharacters = 32767;
    QskTextInput::EchoMode echoMode = QskTextInput::Normal;

    Qt::InputMethodQueries queries = Qt::ImQueryAll;
    queries &= ~Qt::ImEnabled;

    QInputMethodQueryEvent event( queries );
    QCoreApplication::sendEvent( const_cast< QQuickItem* >( item ), &event );

    if ( event.queries() & Qt::ImHints )
    {
        const auto hints = static_cast< Qt::InputMethodHints >(
            event.value( Qt::ImHints ).toInt() );

        if ( hints & Qt::ImhHiddenText )
            echoMode = QskTextInput::NoEcho;
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
        setInputText( text );
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

    if ( echoMode == QskTextInput::NoEcho )
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
            if ( mode == QskTextInput::Password )
                echoMode = QskTextInput::Password;
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

#include "moc_QskTextInput.cpp"
