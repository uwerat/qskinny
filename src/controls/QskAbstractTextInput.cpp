/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskAbstractTextInput.h"
#include "QskFontRole.h"
#include "QskQuick.h"
#include "QskEvent.h"
#include "QskInternalMacros.h"

#include <qguiapplication.h>
#include <qstylehints.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qquicktextedit_p.h>
#include <private/qquicktextinput_p.h>

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
#include <qwindow.h>
#include <private/qeventpoint_p.h>
#endif

QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskAbstractTextInput, Text )

QSK_SYSTEM_STATE( QskAbstractTextInput, ReadOnly, QskAspect::FirstSystemState << 1 )
QSK_SYSTEM_STATE( QskAbstractTextInput, Editing, QskAspect::FirstSystemState << 2 )
QSK_SYSTEM_STATE( QskAbstractTextInput, Selected, QskAspect::FirstSystemState << 3 )

static inline QVariant qskInputMethodQuery(
    const QQuickItem* item, Qt::InputMethodQuery query, QVariant argument )
{
    if ( auto input = qobject_cast< const QQuickTextInput* >( item ) )
        return input->inputMethodQuery( query, argument );

    if ( auto edit = qobject_cast< const QQuickTextEdit* >( item ) )
        return edit->inputMethodQuery( query, argument );

    return QVariant();
}

inline void qskSetAlignment( QQuickItem* item, Qt::Alignment alignment )
{
    item->setProperty( "horizontalAlignment", int( alignment ) & 0x0f );
    item->setProperty( "verticalAlignment", int( alignment ) & 0xf0 );
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

static inline void qskForwardEvent( QQuickItem* item, QEvent* event )
{
    switch( static_cast< int >( event->type() ) )
    {
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        {
            const auto pos = item->position();

            auto mouseEvent = static_cast< QMouseEvent* >( event );

            /*
                As the event was sent for the parent item
                we have to translate the position into
                our coordinate system.
             */
            qskTranslateMouseEventPosition( mouseEvent, -pos );

            QMetaObject::invokeMethod( item, "handleEvent",
                Qt::DirectConnection, Q_ARG( QEvent*, event ) );

            qskTranslateMouseEventPosition( mouseEvent, pos );

            break;
        }
        default:
            QMetaObject::invokeMethod( item, "handleEvent",
                Qt::DirectConnection, Q_ARG( QEvent*, event ) );
    }
}

class QskAbstractTextInput::PrivateData
{
  public:
    ActivationModes activationModes;

    QQuickItem* input = nullptr;
    QQuickTextInput* textInput = nullptr;
    QQuickTextEdit* textEdit = nullptr;
};

#define INPUT_INVOKE(func) \
    ( m_data->textInput ? m_data->textInput->func() : m_data->textEdit->func() )

#define INPUT_INVOKE_ARG(func, arg) \
    ( m_data->textInput ? m_data->textInput->func( arg ) : m_data->textEdit->func( arg ) )
 
#define INPUT_CONNECT( func ) \
    m_data->textInput \
        ? connect( m_data->textInput, &QQuickTextInput::func, this, &QskAbstractTextInput::func ) \
        : connect( m_data->textEdit, &QQuickTextEdit::func, this, &QskAbstractTextInput::func )

#define INPUT_CONNECT1( func, get ) \
    do \
    { \
        auto f = [this]() { Q_EMIT func( get() ); }; \
        m_data->textInput \
            ? connect( m_data->textInput, &QQuickTextInput::func, this, f ) \
            : connect( m_data->textEdit, &QQuickTextEdit::func, this, f ); \
    } while ( false )

QskAbstractTextInput::QskAbstractTextInput( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    m_data->activationModes = ActivationOnMouse | ActivationOnKey;

    setPolishOnResize( true );

    setAcceptHoverEvents( true );
    setFocusPolicy( Qt::StrongFocus );

    setFlag( QQuickItem::ItemAcceptsInputMethod );
}

QskAbstractTextInput::~QskAbstractTextInput()
{
}

void QskAbstractTextInput::setup( QQuickItem* wrappedInput )
{
    m_data->input = wrappedInput;

    m_data->textInput = qobject_cast< QQuickTextInput* >( wrappedInput );
    m_data->textEdit = qobject_cast< QQuickTextEdit* >( wrappedInput );

    INPUT_CONNECT( textChanged );
    INPUT_CONNECT( preeditTextChanged );
    INPUT_CONNECT( readOnlyChanged );
    INPUT_CONNECT( overwriteModeChanged );
    INPUT_CONNECT( cursorVisibleChanged );
    INPUT_CONNECT1( cursorPositionChanged, cursorPosition );
    INPUT_CONNECT( selectByMouseChanged );
    INPUT_CONNECT1( persistentSelectionChanged, persistentSelection );
    INPUT_CONNECT1( wrapModeChanged, wrapMode );

    INPUT_CONNECT1( canUndoChanged, canUndo );
    INPUT_CONNECT1( canRedoChanged, canRedo );
    INPUT_CONNECT1( canPasteChanged, canPaste );

    INPUT_CONNECT1( inputMethodHintsChanged, inputMethodHints );
    INPUT_CONNECT1( inputMethodComposingChanged, isInputMethodComposing );

    /*
        Other properties offered from QQuickTextInput/QQuickTextEdit:

        - cursorRectangleChanged
        - cursorDelegateChanged

          The default implementation creates a QQuickRectangle for the cursor
          that can be replaced by a delegate.

          However the concept of QSkinny would be to customize the
          the cursor using skin hints and/or creating the scene graph node
          from the skinlet. TODO ...

        - colorChanged
        - fontChanged

          Set from the skin hints - might be important enough to offer a
          convenience API

        - selectionColorChanged
        - selectedTextColorChanged

          This properties are set from the skin hints and are not worth to appear
          at the public API

        - horizontalAlignmentChanged
        - verticalAlignmentChanged
        - effectiveHorizontalAlignmentChanged

          covered by QskAbstractTextInput::alignmentChanged

        - activeFocusOnPressChanged

          covered by QskAbstractTextInput::activationModesChanged

        - selectionStartChanged;
        - selectionEndChanged;
        - selectedTextChanged;

          Maybe there is a better API for the selection TODO ...

        - mouseSelectionModeChanged

          What to do with this mode. TODO ...

        - editingFinished

          This signal should never be emitted as it happens on
          events ( focusOut, commit keys ) that are handled in
          QskAbstractTextInput and indicated with editicgChanged( bool );
          ( Maybe having an assertion TODO ... )

        - contentSizeChanged

          Need to understand what this information might be good for.
          Maybe the sizeHints ....

        - renderTypeChanged

          This one is about the type of renderer to be used. This is similar
          to QskItem::PreferRasterForTextures and instead of having a flag in
          QskAbstractTextInput we might want to have a more general solution
          in QskItem. TODO ...
     */
}

QskAbstractTextInput::ActivationModes QskAbstractTextInput::activationModes() const
{
    return m_data->activationModes;
}

void QskAbstractTextInput::setActivationModes( ActivationModes modes )
{
    if ( m_data->activationModes != modes )
    {
        m_data->activationModes = modes;
        Q_EMIT activationModesChanged();
    }
}

bool QskAbstractTextInput::selectByMouse() const
{
    return INPUT_INVOKE( selectByMouse );
}

void QskAbstractTextInput::setSelectByMouse( bool on )
{
    INPUT_INVOKE_ARG( setSelectByMouse, on );
}

bool QskAbstractTextInput::persistentSelection() const
{
    return INPUT_INVOKE( persistentSelection );
}

void QskAbstractTextInput::setPersistentSelection( bool on )
{
    INPUT_INVOKE_ARG( setPersistentSelection, on );
}

int QskAbstractTextInput::length() const
{
    return INPUT_INVOKE( length );
}

QString QskAbstractTextInput::text() const
{
    return INPUT_INVOKE( text );
}

void QskAbstractTextInput::setText( const QString& text )
{
    INPUT_INVOKE_ARG( setText, text );
}

QString QskAbstractTextInput::preeditText() const
{
    return INPUT_INVOKE( preeditText );
}

void QskAbstractTextInput::clear()
{
    INPUT_INVOKE( clear );
}

void QskAbstractTextInput::selectAll()
{
    INPUT_INVOKE( selectAll );
}

void QskAbstractTextInput::deselect()
{
    INPUT_INVOKE( deselect );
}

bool QskAbstractTextInput::canUndo() const
{
    return INPUT_INVOKE( canUndo );
}

bool QskAbstractTextInput::canRedo() const
{
    return INPUT_INVOKE( canRedo );
}

bool QskAbstractTextInput::canPaste() const
{
    return INPUT_INVOKE( canPaste );
}

void QskAbstractTextInput::setFontRole( const QskFontRole& role )
{
    if ( setFontRoleHint( Text, role ) )
    {
        const auto queries = Qt::ImCursorRectangle | Qt::ImFont | Qt::ImAnchorRectangle;
        qskUpdateInputMethod( this, queries );

        Q_EMIT fontRoleChanged();
    }
}

void QskAbstractTextInput::resetFontRole()
{
    if ( resetFontRoleHint( Text ) )
    {
        const auto queries = Qt::ImCursorRectangle | Qt::ImFont | Qt::ImAnchorRectangle;
        qskUpdateInputMethod( this, queries );

        Q_EMIT fontRoleChanged();
    }
}

QskFontRole QskAbstractTextInput::fontRole() const
{
    return fontRoleHint( Text );
}

QFont QskAbstractTextInput::font() const
{
    return effectiveFont( Text );
}

QVariant QskAbstractTextInput::inputMethodQuery(
    Qt::InputMethodQuery query ) const
{
    return inputMethodQuery( query, QVariant() );
}

QVariant QskAbstractTextInput::inputMethodQuery(
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
            QVariant v = qskInputMethodQuery( m_data->input, query, argument );
#if 1
            if ( v.canConvert< QRectF >() )
                v.setValue( v.toRectF().translated( m_data->input->position() ) );
#endif
            return v;
        }
        default:
        {
            return qskInputMethodQuery( m_data->input, query, argument );
        }
    }
}

Qt::InputMethodHints QskAbstractTextInput::inputMethodHints() const
{
    return INPUT_INVOKE( inputMethodHints );
}

void QskAbstractTextInput::setInputMethodHints( Qt::InputMethodHints hints )
{
    if ( inputMethodHints() != hints )
    {
        INPUT_INVOKE_ARG( setInputMethodHints, hints );
        qskUpdateInputMethod( this, Qt::ImHints );
    }
}

bool QskAbstractTextInput::event( QEvent* event )
{
    if ( event->type() == QEvent::LocaleChange )
        qskUpdateInputMethod( this, Qt::ImPreferredLanguage );

    if ( event->type() == QEvent::ShortcutOverride )
    {
        forwardEvent( event );
        return event->isAccepted();
    }

    return Inherited::event( event );
}

void QskAbstractTextInput::mousePressEvent( QMouseEvent* event )
{
    forwardEvent( event );

    if ( !isReadOnly() && !qGuiApp->styleHints()->setFocusOnTouchRelease() )
        setEditing( true );
}

void QskAbstractTextInput::mouseMoveEvent( QMouseEvent* event )
{
    forwardEvent( event );
}

void QskAbstractTextInput::mouseReleaseEvent( QMouseEvent* event )
{
    forwardEvent( event );

    if ( !isReadOnly() && qGuiApp->styleHints()->setFocusOnTouchRelease() )
        setEditing( true );
}

void QskAbstractTextInput::mouseDoubleClickEvent( QMouseEvent* event )
{
    forwardEvent( event );
}

void QskAbstractTextInput::keyPressEvent( QKeyEvent* event )
{
    if ( isEditing() )
    {
        switch ( event->key() )
        {
            case Qt::Key_Escape:
            {
                setEditing( false );
                break;
            }

            case Qt::Key_Enter:
            case Qt::Key_Return:
            {
                const auto hints = inputMethodQuery( Qt::ImHints ).toInt();
                if ( !( hints & Qt::ImhMultiLine ) )
                {
                    if ( auto input = m_data->textInput )
                    {
                        bool accept = input->hasAcceptableInput();
                        if ( !accept )
                        {
                            QMetaObject::invokeMethod( input, "fixup",
                                Qt::DirectConnection, Q_RETURN_ARG(bool, accept) );
                        }

                        if ( !accept )
                            return; // ignore
                    }

                    QGuiApplication::inputMethod()->commit();
                    setEditing( false );
                }

                break;
            }
        }

        if ( isEditing() )
        {
            forwardEvent( event );
        }
        else
        {
            // When returning from a virtual keyboard
            qskForceActiveFocus( this, Qt::PopupFocusReason );
        }
    }
    else
    {
        if ( ( activationModes() & ActivationOnKey ) && !event->isAutoRepeat() )
        {
            if ( event->key() == Qt::Key_Select || event->key() == Qt::Key_Space )
            {
                setEditing( true );
                return;
            }
        }

        Inherited::keyPressEvent( event );
    }
}

void QskAbstractTextInput::keyReleaseEvent( QKeyEvent* event )
{
    Inherited::keyReleaseEvent( event );
}

void QskAbstractTextInput::focusInEvent( QFocusEvent* event )
{
    if ( activationModes() & ActivationOnFocus )
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

void QskAbstractTextInput::focusOutEvent( QFocusEvent* event )
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
            deselect();
            setEditing( false );
        }
    }

    Inherited::focusOutEvent( event );
}

void QskAbstractTextInput::inputMethodEvent( QInputMethodEvent* event )
{
    const bool hadCursor = isCursorVisible();

    forwardEvent( event );

    if ( isCursorVisible() && !hadCursor )
    {
        /*
            The initial InputMethod events might be sent from the
            platform depending on focus. Unfortunately an
            empty dummy event ( = no attributes ) leads to showing
            the cursor.
         */
        if ( isEditing() )
            setCursorVisible( false );
    }
}

bool QskAbstractTextInput::isReadOnly() const
{
    return INPUT_INVOKE( isReadOnly );
}

void QskAbstractTextInput::setReadOnly( bool on )
{
    if ( on == isReadOnly() )
        return;

#if 1
    // do we want to be able to restore the previous policy ?
    setFocusPolicy( Qt::NoFocus );
#endif

    INPUT_INVOKE_ARG( setReadOnly, on );

    // we are killing user settings here ?
    m_data->input->setFlag( QQuickItem::ItemAcceptsInputMethod, !on );

#if QT_VERSION >= QT_VERSION_CHECK( 6, 2, 0 )
    qskUpdateInputMethod( this, Qt::ImReadOnly );
#else
    qskUpdateInputMethod( this, Qt::ImEnabled );
#endif

    setSkinStateFlag( ReadOnly, on );
}

bool QskAbstractTextInput::isInputMethodComposing() const
{
    return INPUT_INVOKE( isInputMethodComposing );
}

bool QskAbstractTextInput::isEditing() const
{
    return hasSkinState( Editing );
}

void QskAbstractTextInput::setEditing( bool on )
{
    if ( ( on == isEditing() ) || ( on && isReadOnly() ) )
        return;

    setSkinStateFlag( Editing, on );

    QMetaObject::invokeMethod( m_data->input, "setEditing",
        Qt::DirectConnection, Q_ARG( bool, on ) );

    qskInputMethodSetVisible( this, on );
    Q_EMIT editingChanged( on );
}

bool QskAbstractTextInput::overwriteMode() const
{
    return INPUT_INVOKE( overwriteMode );
}

void QskAbstractTextInput::setOverwriteMode( bool on )
{
    INPUT_INVOKE_ARG( setOverwriteMode, on );
}

int QskAbstractTextInput::cursorPosition() const
{
    return INPUT_INVOKE( cursorPosition );
}

void QskAbstractTextInput::setCursorPosition( int pos )
{
    INPUT_INVOKE_ARG( setCursorPosition, pos );
}

bool QskAbstractTextInput::isCursorVisible() const
{
    return INPUT_INVOKE( isCursorVisible );
}

void QskAbstractTextInput::setCursorVisible( bool on )
{
    INPUT_INVOKE_ARG( setCursorVisible, on );
}

void QskAbstractTextInput::setWrapMode( QskTextOptions::WrapMode wrapMode )
{
    if ( m_data->textInput )
    {
        auto mode = static_cast< QQuickTextInput::WrapMode >( wrapMode );
        m_data->textInput->setWrapMode( mode );
    }
    else
    {
        auto mode = static_cast< QQuickTextEdit::WrapMode >( wrapMode );
        m_data->textEdit->setWrapMode( mode );
    }
}

QskTextOptions::WrapMode QskAbstractTextInput::wrapMode() const
{
    if ( m_data->textInput )
        return static_cast< QskTextOptions::WrapMode >( m_data->textInput->wrapMode() );
    else
        return static_cast< QskTextOptions::WrapMode >( m_data->textEdit->wrapMode() );
}

QSizeF QskAbstractTextInput::unwrappedTextSize() const
{
    const auto w = INPUT_INVOKE( contentWidth );
    const auto h = INPUT_INVOKE( contentHeight );

    return QSizeF( w, h );
}

void QskAbstractTextInput::setAlignment( Qt::Alignment alignment )
{
    if ( setAlignmentHint( Text, alignment ) )
    {
        qskSetAlignment( m_data->input, alignment );
        Q_EMIT alignmentChanged();
    }
}

void QskAbstractTextInput::resetAlignment()
{
    if ( resetAlignmentHint( Text ) )
    {
        qskSetAlignment( m_data->input, alignment() );
        Q_EMIT alignmentChanged();
    }
}

Qt::Alignment QskAbstractTextInput::alignment() const
{
    Qt::Alignment alignment = Qt::AlignLeft;
    if ( qobject_cast< const QQuickTextEdit* >( m_data->input ) )
        alignment |= Qt::AlignTop;
    else
        alignment |= Qt::AlignVCenter;

    return alignmentHint( Text, alignment );
}

void QskAbstractTextInput::updateLayout()
{
    QMetaObject::invokeMethod( m_data->input, "updateMetrics" );
    qskSetItemGeometry( m_data->input, subControlRect( Text ) );
}

void QskAbstractTextInput::updateNode( QSGNode* node )
{
    QMetaObject::invokeMethod( m_data->input, "updateColors" );
    Inherited::updateNode( node );
}

void QskAbstractTextInput::forwardEvent( QEvent* event )
{
    qskForwardEvent( m_data->input, event );
}

#include "moc_QskAbstractTextInput.cpp"
