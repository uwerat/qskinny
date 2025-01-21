/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskAbstractTextInput.h"
#include "QskFontRole.h"
#include "QskQuick.h"
#include "QskEvent.h"
#include "QskMetaInvokable.h"
#include "QskInternalMacros.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquicktextedit_p.h>
#include <private/qquicktextinput_p.h>
#include <private/qquicktextinput_p_p.h>

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
#include <private/qeventpoint_p.h>
#endif

#include <qhash.h>

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

namespace
{
    class PropertyBinder : public QObject
    {
        Q_OBJECT

      public:
        void setup( const QObject*, const QMetaObject*, QObject* );

      private:
        Q_INVOKABLE void forwardNotification() const;

        typedef QPair< int, QMetaMethod > Binding;
        typedef QHash< int, Binding > BindingTable;

        Binding bindingOf( const QMetaObject*, int methodIndex ) const;
        static QMap< const QMetaObject*, BindingTable > m_tables;
    };

    QMap< const QMetaObject*, PropertyBinder::BindingTable > PropertyBinder::m_tables;

    void PropertyBinder::setup( const QObject* input,
        const QMetaObject* mo, QObject* proxy )
    {
        setParent( proxy );

        auto& hash = m_tables[ mo ];

        if ( hash.isEmpty() )
        {
            for ( int i = mo->propertyOffset(); i < mo->propertyCount(); i++ )
            {
                const auto property = mo->property( i );

                const auto signalIndex = property.notifySignalIndex();
                if ( signalIndex >= 0 )
                {
                    const auto moProxy = proxy->metaObject();

                    const int idx = moProxy->indexOfProperty( property.name() );
                    if ( idx >= 0 )
                    {
                        const auto method = moProxy->property( idx ).notifySignal();
                        if ( method.isValid() )
                            hash.insert( signalIndex, { i, method } );
                    }
                    else
                    {
                        // qDebug() << "Missing" << property.name();
                    }
                }
            }
        }

        const int idx = staticMetaObject.indexOfMethod( "forwardNotification()" );
        const auto forwardMethod = staticMetaObject.method( idx );

        for ( auto it = hash.constBegin(); it != hash.constEnd(); ++it )
        {
            connect( input, input->metaObject()->method( it.key() ),
                this, forwardMethod, Qt::DirectConnection );
        }
    }

    void PropertyBinder::forwardNotification() const
    {
        if ( thread() != QThread::currentThread() )
            return;

        const auto mo = sender()->metaObject();

        const auto binding = bindingOf( mo, senderSignalIndex() );
        if ( !binding.second.isValid() )
            return;

        const auto property = mo->property( binding.first );

        const auto value = property.read( sender() );
        void* args[3] = { nullptr, const_cast< void* >( value.data() ), nullptr };

        qskInvokeMetaMethod( parent(), binding.second, args, Qt::DirectConnection );
    }

    PropertyBinder::Binding PropertyBinder::bindingOf(
        const QMetaObject* metaObject, int methodIndex ) const
    {
        const auto method = metaObject->method( methodIndex );

        const auto& hash = m_tables[ method.enclosingMetaObject() ];
        return hash.value( methodIndex );
    }
}

class QskAbstractTextInput::PrivateData
{
  public:
    ActivationModes activationModes;
    PropertyBinder binder;

    QQuickItem* input = nullptr;
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

void QskAbstractTextInput::setup( QQuickItem* wrappedInput,
    const QMetaObject* metaObject )
{
    m_data->input = wrappedInput;
    m_data->binder.setup( wrappedInput, metaObject, this );
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
    return m_data->input->property( "selectByMouse" ).value< bool >();
}

void QskAbstractTextInput::setSelectByMouse( bool on )
{
    m_data->input->setProperty( "selectByMouse", on );
}

QString QskAbstractTextInput::text() const
{
    return m_data->input->property( "text" ).value< QString >();
}

void QskAbstractTextInput::setText( const QString& text )
{
    m_data->input->setProperty( "text", text );
}

QString QskAbstractTextInput::preeditText() const
{
    return m_data->input->property( "preeditText" ).value< QString >();
}

void QskAbstractTextInput::clear()
{
    QMetaObject::invokeMethod( m_data->input, "clear" );
}

void QskAbstractTextInput::selectAll()
{
    QMetaObject::invokeMethod( m_data->input, "selectAll" );
}

void QskAbstractTextInput::deselect()
{
    QMetaObject::invokeMethod( m_data->input, "deselect" );
}

bool QskAbstractTextInput::canUndo() const
{
    return m_data->input->property( "canUndo" ).value< bool >();
}

bool QskAbstractTextInput::canRedo() const
{
    return m_data->input->property( "canRedo" ).value< bool >();
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
    return qskInputMethodHints( m_data->input );
}

void QskAbstractTextInput::setInputMethodHints( Qt::InputMethodHints hints )
{
    if ( qskInputMethodHints( m_data->input ) != hints )
    {
        qskSetInputMethodHints( m_data->input, hints );
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
    return m_data->input->property( "readOnly" ).value< bool >();
}

void QskAbstractTextInput::setReadOnly( bool on )
{
    if ( on == isReadOnly() )
        return;

#if 1
    // do we want to be able to restore the previous policy ?
    setFocusPolicy( Qt::NoFocus );
#endif

    auto input = m_data->input;
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

    auto input = m_data->input;

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
    return m_data->input->property( "overwriteMode" ).value< bool >();
}

void QskAbstractTextInput::setOverwriteMode( bool overwrite )
{
    m_data->input->setProperty( "overwriteMode", overwrite );
}

int QskAbstractTextInput::cursorPosition() const
{
    return m_data->input->property( "cursorPosition" ).value< int >();
}

void QskAbstractTextInput::setCursorPosition( int pos )
{
    m_data->input->setProperty( "cursorPosition", pos );
}

bool QskAbstractTextInput::isCursorVisible() const
{
    return m_data->input->property( "cursorVisible" ).value< bool >();
}

void QskAbstractTextInput::setCursorVisible( bool on )
{
    m_data->input->setProperty( "cursorVisible", on );
}

void QskAbstractTextInput::setWrapMode( QskTextOptions::WrapMode wrapMode )
{
    m_data->input->setProperty( "wrapMode", static_cast< int >( wrapMode ) );
}

QskTextOptions::WrapMode QskAbstractTextInput::wrapMode() const
{
    const auto mode = m_data->input->property( "wrapMode" ).value< int >();
    return static_cast< QskTextOptions::WrapMode >( mode );
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

#include "QskAbstractTextInput.moc"
#include "moc_QskAbstractTextInput.cpp"
