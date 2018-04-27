/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextInput.h"

// VS2012+ disable keyword macroizing unless _ALLOW_KEYWORD_MACROS is set
#ifdef _MSC_VER
#if ( _MSC_VER >= 1700 ) && !defined( _ALLOW_KEYWORD_MACROS )
#define _ALLOW_KEYWORD_MACROS
#endif
#endif

QSK_QT_PRIVATE_BEGIN
// we need to access QQuickTextInputPrivate::hasAcceptableInput
#define private public
#include <private/qquicktextinput_p.h>
#include <private/qquicktextinput_p_p.h>
#undef private
QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskTextInput, Panel )
QSK_SUBCONTROL( QskTextInput, Text )
QSK_SUBCONTROL( QskTextInput, PanelSelected )
QSK_SUBCONTROL( QskTextInput, TextSelected )

QSK_STATE( QskTextInput, ReadOnly, QskAspect::FirstSystemState << 1 )
QSK_STATE( QskTextInput, Editing, QskAspect::FirstSystemState << 2 )

static inline void qskBindSignals( const QQuickTextInput* wrappedInput,
    QskTextInput* input )
{
    QObject::connect( wrappedInput, &QQuickTextInput::textChanged,
        input, [ input ] { input->Q_EMIT textChanged( input->text() ); } );

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    QObject::connect( wrappedInput, &QQuickTextInput::textEdited,
        input, [ input ] { input->Q_EMIT textEdited( input->text() ); } );
#endif

    QObject::connect( wrappedInput, &QQuickTextInput::textChanged,
        input, [ input ] { input->Q_EMIT textChanged( input->text() ); } );

    QObject::connect( wrappedInput, &QQuickTextInput::validatorChanged,
        input, &QskTextInput::validatorChanged );

    QObject::connect( wrappedInput, &QQuickTextInput::inputMaskChanged,
        input, &QskTextInput::inputMaskChanged );

    QObject::connect( wrappedInput, &QQuickTextInput::readOnlyChanged,
        input, &QskTextInput::readOnlyChanged );

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    QObject::connect( wrappedInput, &QQuickTextInput::overwriteModeChanged,
        input, &QskTextInput::overwriteModeChanged );
#endif

    QObject::connect( wrappedInput, &QQuickTextInput::maximumLengthChanged,
        input, &QskTextInput::maximumLengthChanged );

    QObject::connect( wrappedInput, &QQuickTextInput::echoModeChanged,
        input, [ input ] { input->Q_EMIT echoModeChanged( input->echoMode() ); } );

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
            auto d = QQuickTextInputPrivate::get( this );

            // QQuickTextInputPrivate::checkIsValid ???
            const auto state = static_cast< int >( d->hasAcceptableInput( d->m_text ) );

            bool isAcceptable = ( state == QValidator::Acceptable );
            if ( !isAcceptable )
                isAcceptable = d->fixup();

            return isAcceptable;
        }

        void updateColors();
        void updateMetrics();

        inline bool handleEvent( QEvent* event )
        {
            return this->event( event );
        }

    protected:
        virtual void geometryChanged(
            const QRectF& newGeometry, const QRectF& oldGeometry ) override
        {
            Inherited::geometryChanged( newGeometry, oldGeometry );
            updateClip();
        }

        void updateClip()
        {
            setClip( ( contentWidth() > width() ) ||
                ( contentHeight() > height() ) );
        }

        virtual QSGNode* updatePaintNode(
            QSGNode* oldNode, UpdatePaintNodeData* data ) override
        {
            updateColors();
            return Inherited::updatePaintNode( oldNode, data );
        }
    };

    TextInput::TextInput( QskTextInput* textInput ):
        QQuickTextInput( textInput )
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
        d->setBlinkingCursorEnabled( on );
#endif

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

        color = input->color( QskTextInput::Text );
        if ( d->color != color )
        {
            d->color = color;
            isDirty = true;
        }

        if ( d->hasSelectedText() )
        {
            color = input->color( QskTextInput::PanelSelected );
            if ( d->selectionColor != color )
            {
                d->selectionColor = color;
                isDirty = true;
            }

            color = input->color( QskTextInput::TextSelected );
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
    TextInput* textInput;
    QString description; // f.e. used as prompt in QskInputPanel

    unsigned int activationModes : 3;
};

QskTextInput::QskTextInput( QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
    m_data->activationModes = ActivationOnMouse | ActivationOnKey;

    setPolishOnResize( true );
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

    initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Fixed );
}

QskTextInput::QskTextInput( const QString& text, QQuickItem* parent ):
    QskTextInput( parent )
{
    m_data->textInput->setText( text );
}

QskTextInput::~QskTextInput()
{
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
        switch( event->key() )
        {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            {
                if ( fixup() )
                {
                    QGuiApplication::inputMethod()->commit();

                    if ( !( inputMethodHints() & Qt::ImhMultiLine ) )
                        setEditing( false );
                }
                break;
            }
#if 1
            case Qt::Key_Escape:
            {
                QGuiApplication::inputMethod()->hide();
                setEditing( false );

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
    m_data->textInput->handleEvent( event );

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
        switch( event->reason() )
        {
            case Qt::ActiveWindowFocusReason:
            case Qt::PopupFocusReason:
                break;

            default:
                setEditing( true );
        }
    }

    Inherited::focusInEvent( event );
}

void QskTextInput::focusOutEvent( QFocusEvent* event )
{
#if 1
    if ( event->reason() != Qt::ActiveWindowFocusReason
        && event->reason() != Qt::PopupFocusReason )
    {
        m_data->textInput->deselect();
    }
#endif

    if ( m_data->activationModes & ActivationOnFocus )
    {
#if 0
        if ( !hasFocus() )
        {
            setEditing( false );
        }
#endif
    }

    Inherited::focusOutEvent( event );
}

QSizeF QskTextInput::contentsSizeHint() const
{
    using namespace QskAspect;

    auto input = m_data->textInput;

    input->updateMetrics();

    const qreal w = input->implicitWidth();
    const qreal h = input->implicitHeight();

    const QSizeF minSize( metric( Panel | MinimumWidth ),
        metric( Panel | MinimumHeight ) );

    return outerBoxSize( Panel, QSizeF( w, h ) ).expandedTo( minSize );
}

void QskTextInput::updateLayout()
{
    auto input = m_data->textInput;

    input->updateMetrics();
    qskSetItemGeometry( input, subControlRect( Text ) );
}

void QskTextInput::updateNode( QSGNode* node )
{
    m_data->textInput->updateColors();
    Inherited::updateNode( node );
}

QString QskTextInput::text() const
{
    return m_data->textInput->text();
}

void QskTextInput::setText( const QString& text )
{
    m_data->textInput->setText( text );
}

void QskTextInput::setDescription( const QString& text )
{
    if ( m_data->description != text )
    {
        m_data->description = text;
        Q_EMIT descriptionChanged( text );
    }
}

QString QskTextInput::description() const
{
    return m_data->description;
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
        polish();
        resetImplicitSize();

        Qt::InputMethodQueries queries = Qt::ImCursorRectangle | Qt::ImFont;
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
        queries |= Qt::ImAnchorRectangle;
#endif

        qskUpdateInputMethod( this, queries );
        Q_EMIT fontRoleChanged();
    }
}

void QskTextInput::setAlignment( Qt::Alignment alignment )
{
    if ( alignment != this->alignment() )
    {
        const auto subControl = effectiveSubcontrol( Text );
        setFlagHint( subControl | QskAspect::Alignment, alignment );

        m_data->textInput->setAlignment( alignment );

        polish();

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
    if ( m_data->textInput->isReadOnly() == on )
        return;

#if 1
    // do we want to be able to restore the previous policy ?
    setFocusPolicy( Qt::NoFocus );
#endif

    m_data->textInput->setReadOnly( on );

    // we are killing user settings here ?
    m_data->textInput->setFlag( QQuickItem::ItemAcceptsInputMethod, !on );
    qskUpdateInputMethod( this, Qt::ImEnabled );

    setSkinStateFlag( ReadOnly, true );
}

void QskTextInput::setEditing( bool on )
{
    if ( isReadOnly() || on == ( skinState() & Editing ) )
        return;

    setSkinStateFlag( Editing, on );
    m_data->textInput->setEditing( on );

    auto inputMethod = QGuiApplication::inputMethod();

    if ( on )
    {
#if 0
        updateInputMethod(Qt::ImCursorRectangle | Qt::ImAnchorRectangle);
        QGuiApplication::inputMethod()->inputDirection
#endif
        inputMethod->show();
    }
    else
    {
        auto d = QQuickTextInputPrivate::get( m_data->textInput );

        const auto status = d->hasAcceptableInput( d->m_text );
        if ( status == QQuickTextInputPrivate::AcceptableInput )
        {
            if ( fixup() )
                m_data->textInput->Q_EMIT editingFinished();
        }

#if 0
        inputMethod->reset();
#endif
        inputMethod->hide();
#if 1
        qskForceActiveFocus( this, Qt::PopupFocusReason );
#endif
    }

    Q_EMIT editingChanged( on );
}

bool QskTextInput::isEditing() const
{
    return skinState() & Editing;
}

void QskTextInput::ensureVisible( int position )
{
    m_data->textInput->ensureVisible( position );
}

int QskTextInput::cursorPosition() const
{
    return m_data->textInput->cursorPosition();
}

void QskTextInput::setCursorPosition(int pos)
{
    m_data->textInput->setCursorPosition( pos );
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
    if ( mode != echoMode() )
    {
        m_data->textInput->setEchoMode(
            static_cast< QQuickTextInput::EchoMode >( mode ) );

        qskUpdateInputMethod( this, Qt::ImHints );
    }
}

QString QskTextInput::displayText() const
{
    return m_data->textInput->displayText();
}

QString QskTextInput::preeditText() const
{
    auto d = QQuickTextInputPrivate::get( m_data->textInput );
    return d->m_textLayout.preeditAreaText();
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)

bool QskTextInput::overwriteMode() const
{
    return m_data->textInput->overwriteMode();
}

void QskTextInput::setOverwriteMode( bool overwrite )
{
    m_data->textInput->setOverwriteMode( overwrite );
}

#endif

bool QskTextInput::hasAcceptableInput() const
{
    return m_data->textInput->hasAcceptableInput();
}

bool QskTextInput::fixup()
{
    return m_data->textInput->fixup();
}

QVariant QskTextInput::inputMethodQuery(
    Qt::InputMethodQuery property) const
{
    return inputMethodQuery( property, QVariant() );
}

QVariant QskTextInput::inputMethodQuery(
    Qt::InputMethodQuery query, QVariant argument) const
{
    switch( query )
    {
        case Qt::ImEnabled:
        {
            return QVariant( (bool)( flags() & ItemAcceptsInputMethod ) );
        }
        case Qt::ImFont:
        {
            return font();
        }
        case Qt::ImPreferredLanguage:
        {
            return locale();
        }
        case Qt::ImCursorRectangle:
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
        case Qt::ImInputItemClipRectangle:
#endif
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

void QskTextInput::setInputMethodHints(Qt::InputMethodHints hints )
{
    if ( m_data->textInput->inputMethodHints() != hints )
    {
        m_data->textInput->setInputMethodHints( hints );
        qskUpdateInputMethod( this, Qt::ImHints );
    }
}

#include "moc_QskTextInput.cpp"
