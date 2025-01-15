/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskAbstractTextInput.h"
#include "QskFontRole.h"
#include "QskQuick.h"
#include "QskEvent.h"
#include "QskInternalMacros.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquicktextedit_p.h>
#include <private/qquicktextinput_p.h>
#include <private/qquicktextinput_p_p.h>

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
#include <private/qeventpoint_p.h>
#endif

QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskAbstractTextInput, Text )

QSK_SYSTEM_STATE( QskAbstractTextInput, ReadOnly, QskAspect::FirstSystemState << 1 )
QSK_SYSTEM_STATE( QskAbstractTextInput, Editing, QskAspect::FirstSystemState << 2 )
QSK_SYSTEM_STATE( QskAbstractTextInput, Selected, QskAspect::FirstSystemState << 3 )

static inline void qskUpdateInputMethodFont( const QskAbstractTextInput* input )
{
    const auto queries = Qt::ImCursorRectangle | Qt::ImFont | Qt::ImAnchorRectangle;
    qskUpdateInputMethod( input, queries );
}

static inline QVariant qskInputMethodQuery(
    const QQuickItem* item, Qt::InputMethodQuery query, QVariant argument )
{
    if ( auto input = qobject_cast< const QQuickTextInput* >( item ) )
        return input->inputMethodQuery( query, argument );

    if ( auto edit = qobject_cast< const QQuickTextEdit* >( item ) )
        return edit->inputMethodQuery( query, argument );

    return QVariant();
}

static inline Qt::InputMethodHints qskInputMethodHints( const QQuickItem* item )
{
    if ( auto input = qobject_cast< const QQuickTextInput* >( item ) )
        return input->inputMethodHints();

    if ( auto edit = qobject_cast< const QQuickTextEdit* >( item ) )
        return edit->inputMethodHints();

    return Qt::InputMethodHints();
}

static inline void qskSetInputMethodHints(
    QQuickItem* item, Qt::InputMethodHints hints )
{
    if ( auto input = qobject_cast< QQuickTextInput* >( item ) )
        input->setInputMethodHints( hints );
    else if ( auto edit = qobject_cast< QQuickTextEdit* >( item ) )
        edit->setInputMethodHints( hints );
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
};

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
    return wrappedInput()->property( "selectByMouse" ).value< bool >();
}

void QskAbstractTextInput::setSelectByMouse( bool on )
{
    wrappedInput()->setProperty( "selectByMouse", on );
}

QString QskAbstractTextInput::text() const
{
    return wrappedInput()->property( "text" ).value< QString >();
}

void QskAbstractTextInput::setText( const QString& text )
{
    wrappedInput()->setProperty( "text", text );
}

QString QskAbstractTextInput::preeditText() const
{
    return wrappedInput()->property( "preeditText" ).value< QString >();
}

void QskAbstractTextInput::clear()
{
    QMetaObject::invokeMethod( wrappedInput(), "clear" );
}

void QskAbstractTextInput::selectAll()
{
    QMetaObject::invokeMethod( wrappedInput(), "selectAll" );
}

void QskAbstractTextInput::deselect()
{
    QMetaObject::invokeMethod( wrappedInput(), "deselect" );
}

bool QskAbstractTextInput::canUndo() const
{
    return wrappedInput()->property( "canUndo" ).value< bool >();
}

bool QskAbstractTextInput::canRedo() const
{
    return wrappedInput()->property( "canRedo" ).value< bool >();
}

void QskAbstractTextInput::setFontRole( const QskFontRole& role )
{
    if ( setFontRoleHint( Text, role ) )
    {
        qskUpdateInputMethodFont( this );
        Q_EMIT fontRoleChanged();
    }
}

void QskAbstractTextInput::resetFontRole()
{
    if ( resetFontRoleHint( Text ) )
    {
        qskUpdateInputMethodFont( this );
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
    Qt::InputMethodQuery property ) const
{
    return inputMethodQuery( property, QVariant() );
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
            QVariant v = qskInputMethodQuery( wrappedInput(), query, argument );
#if 1
            if ( v.canConvert< QRectF >() )
                v.setValue( v.toRectF().translated( wrappedInput()->position() ) );
#endif
            return v;
        }
        default:
        {
            return qskInputMethodQuery( wrappedInput(), query, argument );
        }
    }
}

Qt::InputMethodHints QskAbstractTextInput::inputMethodHints() const
{
    return qskInputMethodHints( wrappedInput() );
}

void QskAbstractTextInput::setInputMethodHints( Qt::InputMethodHints hints )
{
    if ( qskInputMethodHints( wrappedInput() ) != hints )
    {
        qskSetInputMethodHints( wrappedInput(), hints );
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
                    if ( acceptInput() )
                    {
                        QGuiApplication::inputMethod()->commit();
                        setEditing( false );
                    }
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
    return wrappedInput()->property( "readOnly" ).value< bool >();
}

void QskAbstractTextInput::setReadOnly( bool on )
{
    if ( on == isReadOnly() )
        return;

#if 1
    // do we want to be able to restore the previous policy ?
    setFocusPolicy( Qt::NoFocus );
#endif

    auto input = wrappedInput();
    input->setProperty( "readOnly", on );

    // we are killing user settings here ?
    input->setFlag( QQuickItem::ItemAcceptsInputMethod, !on );

#if QT_VERSION >= QT_VERSION_CHECK( 6, 2, 0 )
    qskUpdateInputMethod( this, Qt::ImReadOnly );
#else
    qskUpdateInputMethod( this, Qt::ImEnabled );
#endif

    setSkinStateFlag( ReadOnly, on );
}

bool QskAbstractTextInput::isEditing() const
{
    return hasSkinState( Editing );
}

void QskAbstractTextInput::setEditing( bool on )
{
    if ( isReadOnly() || on == isEditing() )
        return;

    setSkinStateFlag( Editing, on );

    auto input = wrappedInput();

    if ( input->property( "cursorVisible" ).value< bool >() != on )
    {
        input->setProperty( "cursorVisible", on );

        if ( auto textInput = qobject_cast< QQuickTextInput* >( input ) )
        {
            auto d = QQuickTextInputPrivate::get( textInput );
            d->setBlinkingCursorEnabled( on );

            if ( !on )
            {
                if ( d->m_passwordEchoEditing || d->m_passwordEchoTimer.isActive() )
                    d->updatePasswordEchoEditing( false );
            }
        }

        input->polish();
        input->update();
    }

    if ( !on && acceptInput() )
        QMetaObject::invokeMethod( input, "editingFinished" );

    qskInputMethodSetVisible( this, on );
    Q_EMIT editingChanged( on );
}

bool QskAbstractTextInput::acceptInput()
{
    return true;
}

bool QskAbstractTextInput::overwriteMode() const
{
    return wrappedInput()->property( "overwriteMode" ).value< bool >();
}

void QskAbstractTextInput::setOverwriteMode( bool overwrite )
{
    wrappedInput()->setProperty( "overwriteMode", overwrite );
}

int QskAbstractTextInput::cursorPosition() const
{
    return wrappedInput()->property( "cursorPosition" ).value< int >();
}

void QskAbstractTextInput::setCursorPosition( int pos )
{
    wrappedInput()->setProperty( "cursorPosition", pos );
}

bool QskAbstractTextInput::isCursorVisible() const
{
    return wrappedInput()->property( "cursorVisible" ).value< bool >();
}

void QskAbstractTextInput::setCursorVisible( bool on )
{
    wrappedInput()->setProperty( "cursorVisible", on );
}

void QskAbstractTextInput::setWrapMode( QskTextOptions::WrapMode wrapMode )
{
    wrappedInput()->setProperty( "wrapMode", static_cast< int >( wrapMode ) );
}

QskTextOptions::WrapMode QskAbstractTextInput::wrapMode() const
{
    const auto mode = wrappedInput()->property( "wrapMode" ).value< int >();
    return static_cast< QskTextOptions::WrapMode >( mode );
}

void QskAbstractTextInput::setAlignment( Qt::Alignment alignment )
{
    if ( setAlignmentHint( Text, alignment ) )
    {
        qskSetAlignment( wrappedInput(), alignment );
        Q_EMIT alignmentChanged();
    }
}

void QskAbstractTextInput::resetAlignment()
{
    if ( resetAlignmentHint( Text ) )
    {
        qskSetAlignment( wrappedInput(), alignment() );
        Q_EMIT alignmentChanged();
    }
}

Qt::Alignment QskAbstractTextInput::alignment() const
{
    Qt::Alignment alignment = Qt::AlignLeft;
    if ( qobject_cast< const QQuickTextEdit* >( wrappedInput() ) )
        alignment |= Qt::AlignTop;
    else
        alignment |= Qt::AlignVCenter;

    return alignmentHint( Text, alignment );
}

void QskAbstractTextInput::updateLayout()
{
    QMetaObject::invokeMethod( wrappedInput(), "updateMetrics" );
    qskSetItemGeometry( wrappedInput(), subControlRect( Text ) );
}

void QskAbstractTextInput::updateNode( QSGNode* node )
{
    QMetaObject::invokeMethod( wrappedInput(), "updateColors" );
    Inherited::updateNode( node );
}

const QQuickItem* QskAbstractTextInput::wrappedInput() const
{
    auto that = const_cast< QskAbstractTextInput* >( this );
    return that->wrappedInput();
}

void QskAbstractTextInput::forwardEvent( QEvent* event )
{
    qskForwardEvent( wrappedInput(), event );
}

#include "moc_QskAbstractTextInput.cpp"
