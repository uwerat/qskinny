/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextInput.h"
#include "QskTextInputSkinlet.h"
#include "QskFontRole.h"
#include "QskQuick.h"
#include "QskInternalMacros.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquicktextinput_p.h>
#include <private/qquicktextinput_p_p.h>

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
#include <private/qeventpoint_p.h>
#endif

QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskTextInput, Text )
QSK_SUBCONTROL( QskTextInput, TextPanel )

QSK_SYSTEM_STATE( QskTextInput, ReadOnly, QskAspect::FirstSystemState << 1 )
QSK_SYSTEM_STATE( QskTextInput, Editing, QskAspect::FirstSystemState << 2 )
QSK_SYSTEM_STATE( QskTextInput, Error, QskAspect::FirstSystemState << 4 )

static inline void qskPropagateReadOnly( QskTextInput* input )
{
    Q_EMIT input->readOnlyChanged( input->isReadOnly() );

    QEvent event( QEvent::ReadOnlyChange );
    QCoreApplication::sendEvent( input, &event );
}

static inline void qskTranslateMouseEventPosition(
    QMouseEvent* mouseEvent, const QPointF& offset ) 
{
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    auto& point = mouseEvent->point(0);

    QMutableEventPoint::setPosition(
        point, point.position() + offset );
#else
    mouseEvent->setLocalPos( mouseEvent->localPos() + offset );
#endif
}

static inline void qskBindSignals(
    const QQuickTextInput* wrappedInput, QskTextInput* input )
{
    QObject::connect( wrappedInput, &QQuickTextInput::textChanged,
        input, [ input ] { Q_EMIT input->textChanged( input->text() ); } );

    QObject::connect( wrappedInput, &QQuickTextInput::displayTextChanged,
        input, [ input ] { Q_EMIT input->displayTextChanged( input->displayText() ); } );

    QObject::connect( wrappedInput, &QQuickTextInput::textEdited,
        input, [ input ] { Q_EMIT input->textEdited( input->text() ); } );

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

    QObject::connect( wrappedInput, &QQuickTextInput::wrapModeChanged,
        input, [ input ] { Q_EMIT input->wrapModeChanged( input->wrapMode() ); } );

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
    class QuickTextInput final : public QQuickTextInput
    {
        using Inherited = QQuickTextInput;

      public:
        QuickTextInput( QskTextInput* );

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

        void updateColors();
        void updateMetrics();

        inline bool handleEvent( QEvent* event )
        {
            bool ok;

            switch( static_cast< int >( event->type() ) )
            {
                case QEvent::MouseButtonDblClick:
                case QEvent::MouseButtonPress:
                case QEvent::MouseButtonRelease:
                case QEvent::MouseMove:
                {
                    auto mouseEvent = static_cast< QMouseEvent* >( event );

                    /*
                        As the event was sent for the parent item
                        we have to translate the position into
                        our coordinate system.
                     */
                    qskTranslateMouseEventPosition( mouseEvent, -position() );
                    ok = this->event( mouseEvent );
                    qskTranslateMouseEventPosition( mouseEvent, position() );

                    break;
                }
                default:
                    ok = this->event( event );
            }

            return ok;
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

    QuickTextInput::QuickTextInput( QskTextInput* textInput )
        : QQuickTextInput( textInput )
    {
        classBegin();

        setActiveFocusOnTab( false );
        setFlag( ItemAcceptsInputMethod, false );
        setFocusOnPress( false );
        setSelectByMouse( true );

        componentComplete();

        connect( this, &QQuickTextInput::contentSizeChanged,
            this, &QuickTextInput::updateClip );
    }

    void QuickTextInput::setEditing( bool on )
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

    void QuickTextInput::updateMetrics()
    {
        auto textInput = static_cast< const QskTextInput* >( parentItem() );

        setAlignment( textInput->alignment() );
        setFont( textInput->font() );
    }

    void QuickTextInput::updateColors()
    {
        using Q = QskTextInput;

        auto input = static_cast< const QskTextInput* >( parentItem() );

        setColor( input->color( Q::Text ) );

        const auto state = QskTextInputSkinlet::Selected;

        setSelectionColor( input->color( Q::TextPanel | state ) );
        setSelectedTextColor( input->color( Q::Text | state ) );
    }
}

class QskTextInput::PrivateData
{
  public:
    QuickTextInput* wrappedInput;

    ActivationModes activationModes;
};

QskTextInput::QskTextInput( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    m_data->activationModes = ActivationOnMouse | ActivationOnKey;

    setPolishOnResize( true );

    setAcceptHoverEvents( true );
    setFocusPolicy( Qt::StrongFocus );

    setFlag( QQuickItem::ItemAcceptsInputMethod );

    /*
        QQuickTextInput is a beast of almost 5k lines of code, we don't
        want to reimplement that - at least not now.
        So QskTextInput is more or less a simple wrapper making everything
        conforming to qskinny.
     */

    m_data->wrappedInput = new QuickTextInput( this );
    qskBindSignals( m_data->wrappedInput, this );

    setAcceptedMouseButtons( m_data->wrappedInput->acceptedMouseButtons() );
    m_data->wrappedInput->setAcceptedMouseButtons( Qt::NoButton );

    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Fixed );
}

QskTextInput::~QskTextInput()
{
}

bool QskTextInput::event( QEvent* event )
{
    if ( event->type() == QEvent::ShortcutOverride )
    {
        return m_data->wrappedInput->handleEvent( event );
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
                const auto hints = inputMethodQuery( Qt::ImHints ).toInt();
                if ( !( hints & Qt::ImhMultiLine ) )
                {
                    if ( hasAcceptableInput() || fixup() )
                    {
                        QGuiApplication::inputMethod()->commit();
                        setEditing( false );
                    }
                }

                break;
            }
#if 1
            case Qt::Key_Escape:
            {
                setEditing( false );
                break;
            }
#endif
        }

        if ( isEditing() )
        {
            m_data->wrappedInput->handleEvent( event );
        }
        else
        {
            // When returning from a virtual keyboard
            qskForceActiveFocus( this, Qt::PopupFocusReason );
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
    m_data->wrappedInput->handleEvent( event );

    if ( !isReadOnly() && !qGuiApp->styleHints()->setFocusOnTouchRelease() )
        setEditing( true );
}

void QskTextInput::mouseMoveEvent( QMouseEvent* event )
{
    m_data->wrappedInput->handleEvent( event );
}

void QskTextInput::mouseReleaseEvent( QMouseEvent* event )
{
    m_data->wrappedInput->handleEvent( event );

    if ( !isReadOnly() && qGuiApp->styleHints()->setFocusOnTouchRelease() )
        setEditing( true );
}

void QskTextInput::mouseDoubleClickEvent( QMouseEvent* event )
{
    m_data->wrappedInput->handleEvent( event );
}

void QskTextInput::inputMethodEvent( QInputMethodEvent* event )
{
    m_data->wrappedInput->handleEvent( event );
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
            m_data->wrappedInput->deselect();
            setEditing( false );
        }
    }

    Inherited::focusOutEvent( event );
}

void QskTextInput::updateLayout()
{
    m_data->wrappedInput->updateMetrics();
    qskSetItemGeometry( m_data->wrappedInput, subControlRect( Text ) );
}

void QskTextInput::updateNode( QSGNode* node )
{
    m_data->wrappedInput->updateColors();
    Inherited::updateNode( node );
}

QString QskTextInput::text() const
{
    return m_data->wrappedInput->text();
}

void QskTextInput::setText( const QString& text )
{
    m_data->wrappedInput->setText( text );
}

void QskTextInput::clear()
{
    m_data->wrappedInput->clear();
}

void QskTextInput::selectAll()
{
    m_data->wrappedInput->selectAll();
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
    if ( setFontRoleHint( Text, role ) )
    {
        qskUpdateInputMethodFont( this );
        Q_EMIT fontRoleChanged();
    }
}

void QskTextInput::resetFontRole()
{
    if ( resetFontRoleHint( Text ) )
    {
        qskUpdateInputMethodFont( this );
        Q_EMIT fontRoleChanged();
    }
}

QskFontRole QskTextInput::fontRole() const
{
    return fontRoleHint( Text );
}

void QskTextInput::setAlignment( Qt::Alignment alignment )
{
    if ( setAlignmentHint( Text, alignment ) )
    {
        m_data->wrappedInput->setAlignment( alignment );
        Q_EMIT alignmentChanged();
    }
}

void QskTextInput::resetAlignment()
{
    if ( resetAlignmentHint( Text ) )
    {
        m_data->wrappedInput->setAlignment( alignment() );
        Q_EMIT alignmentChanged();
    }
}

Qt::Alignment QskTextInput::alignment() const
{
    return alignmentHint( Text, Qt::AlignLeft | Qt::AlignTop );
}

void QskTextInput::setWrapMode( QskTextOptions::WrapMode wrapMode )
{
    m_data->wrappedInput->setWrapMode(
        static_cast< QQuickTextInput::WrapMode >( wrapMode ) );
}

QskTextOptions::WrapMode QskTextInput::wrapMode() const
{
    return static_cast< QskTextOptions::WrapMode >(
        m_data->wrappedInput->wrapMode() );
}

void QskTextInput::setSelectByMouse( bool on )
{
    m_data->wrappedInput->setSelectByMouse( on );
}

bool QskTextInput::selectByMouse() const
{
    return m_data->wrappedInput->selectByMouse();
}

QFont QskTextInput::font() const
{
    return effectiveFont( QskTextInput::Text );
}

bool QskTextInput::isReadOnly() const
{
    return m_data->wrappedInput->isReadOnly();
}

void QskTextInput::setReadOnly( bool on )
{
    auto input = m_data->wrappedInput;

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
#if 0
    qskUpdateInputMethod( this, Qt::ImReadOnly ); // since 6.2
#endif

    setSkinStateFlag( ReadOnly, on );
}

void QskTextInput::setEditing( bool on )
{
    if ( isReadOnly() || on == isEditing() )
        return;

    setSkinStateFlag( Editing, on );
    m_data->wrappedInput->setEditing( on );

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
            Q_EMIT m_data->wrappedInput->editingFinished();

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
    m_data->wrappedInput->ensureVisible( position );
}

int QskTextInput::cursorPosition() const
{
    return m_data->wrappedInput->cursorPosition();
}

void QskTextInput::setCursorPosition( int pos )
{
    m_data->wrappedInput->setCursorPosition( pos );
}

int QskTextInput::maxLength() const
{
    return m_data->wrappedInput->maxLength();
}

void QskTextInput::setMaxLength( int length )
{
    m_data->wrappedInput->setMaxLength( length );
}

QValidator* QskTextInput::validator() const
{
    return m_data->wrappedInput->validator();
}

void QskTextInput::setValidator( QValidator* validator )
{
    m_data->wrappedInput->setValidator( validator );
}

QString QskTextInput::inputMask() const
{
    return m_data->wrappedInput->inputMask();
}

void QskTextInput::setInputMask( const QString& mask )
{
    m_data->wrappedInput->setInputMask( mask );
}

QskTextInput::EchoMode QskTextInput::echoMode() const
{
    const auto mode = m_data->wrappedInput->echoMode();
    return static_cast< QskTextInput::EchoMode >( mode );
}

void QskTextInput::setEchoMode( EchoMode mode )
{
    if ( mode != echoMode() )
    {
        m_data->wrappedInput->setEchoMode(
            static_cast< QQuickTextInput::EchoMode >( mode ) );

        qskUpdateInputMethod( this, Qt::ImHints );
    }
}

QString QskTextInput::passwordCharacter() const
{
    return m_data->wrappedInput->passwordCharacter();
}

void QskTextInput::setPasswordCharacter( const QString& text )
{
    m_data->wrappedInput->setPasswordCharacter( text );
}

void QskTextInput::resetPasswordCharacter()
{
    m_data->wrappedInput->setPasswordCharacter(
        QGuiApplication::styleHints()->passwordMaskCharacter() );
}

int QskTextInput::passwordMaskDelay() const
{
    return m_data->wrappedInput->passwordMaskDelay();
}

void QskTextInput::setPasswordMaskDelay( int ms )
{
    m_data->wrappedInput->setPasswordMaskDelay( ms );
}

void QskTextInput::resetPasswordMaskDelay()
{
    m_data->wrappedInput->resetPasswordMaskDelay();
}

QString QskTextInput::displayText() const
{
    return m_data->wrappedInput->displayText();
}

QString QskTextInput::preeditText() const
{
    const auto d = QQuickTextInputPrivate::get( m_data->wrappedInput );
    return d->m_textLayout.preeditAreaText();
}

bool QskTextInput::overwriteMode() const
{
    return m_data->wrappedInput->overwriteMode();
}

void QskTextInput::setOverwriteMode( bool overwrite )
{
    m_data->wrappedInput->setOverwriteMode( overwrite );
}

bool QskTextInput::hasAcceptableInput() const
{
    return m_data->wrappedInput->hasAcceptableInput();
}

bool QskTextInput::fixup()
{
    return m_data->wrappedInput->fixup();
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
            QVariant v = m_data->wrappedInput->inputMethodQuery( query, argument );
#if 1
            if ( v.canConvert< QRectF >() )
                v.setValue( v.toRectF().translated( m_data->wrappedInput->position() ) );
#endif
            return v;
        }
        default:
        {
            return m_data->wrappedInput->inputMethodQuery( query, argument );
        }
    }
}

Qt::InputMethodHints QskTextInput::inputMethodHints() const
{
    return m_data->wrappedInput->inputMethodHints();
}

void QskTextInput::setInputMethodHints( Qt::InputMethodHints hints )
{
    if ( m_data->wrappedInput->inputMethodHints() != hints )
    {
        m_data->wrappedInput->setInputMethodHints( hints );
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
