/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextEdit.h"
#include "QskTextEditSkinlet.h"
#include "QskFontRole.h"
#include "QskInternalMacros.h"
#include "QskQuick.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquicktextedit_p.h>
#include <private/qquicktextedit_p_p.h>

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
#include <private/qeventpoint_p.h>
#endif

QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskTextEdit, TextPanel )
QSK_SUBCONTROL( QskTextEdit, Text )

QSK_SYSTEM_STATE( QskTextEdit, ReadOnly, QskAspect::FirstSystemState << 1 )
QSK_SYSTEM_STATE( QskTextEdit, Editing, QskAspect::FirstSystemState << 2 )
QSK_SYSTEM_STATE( QskTextEdit, Error, QskAspect::FirstSystemState << 4 )

static inline void qskPropagateReadOnly( QskTextEdit* edit )
{
    Q_EMIT edit->readOnlyChanged( edit->isReadOnly() );

    QEvent event( QEvent::ReadOnlyChange );
    QCoreApplication::sendEvent( edit, &event );
}

static inline void qskTranslateMouseEventPosition(
    QMouseEvent* mouseEvent, const QPointF& offset )
{
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    auto& point = mouseEvent->point( 0 );

    QMutableEventPoint::setPosition(
        point, point.position() + offset );
#else
    mouseEvent->setLocalPos( mouseEvent->localPos() + offset );
#endif
}

static inline void qskBindSignals(
    const QQuickTextEdit* wrappedEdit, QskTextEdit* edit )
{
    QObject::connect( wrappedEdit, &QQuickTextEdit::textChanged,
        edit, [ edit ] { Q_EMIT edit->textChanged( edit->text() ); } );

    QObject::connect( wrappedEdit, &QQuickTextEdit::preeditTextChanged,
        edit, [ edit ] { Q_EMIT edit->preeditTextChanged( edit->preeditText() ); } );

    QObject::connect( wrappedEdit, &QQuickTextEdit::readOnlyChanged,
        edit, [ edit ] { qskPropagateReadOnly( edit ); } );

    QObject::connect( wrappedEdit, &QQuickTextEdit::overwriteModeChanged,
        edit, &QskTextEdit::overwriteModeChanged );

    QObject::connect( wrappedEdit, &QQuickTextEdit::wrapModeChanged,
        edit, [ edit ] { Q_EMIT edit->wrapModeChanged( edit->wrapMode() ); } );

    QObject::connect( wrappedEdit, &QQuickTextEdit::lineCountChanged,
        [ edit ] { Q_EMIT edit->lineCountChanged( edit->lineCount() ); } );

    QObject::connect( wrappedEdit, &QQuickTextEdit::textFormatChanged,
        edit, [ edit ]( QQuickTextEdit::TextFormat format )
    {
        Q_EMIT edit->textFormatChanged( static_cast< QskTextOptions::TextFormat >( format ) );
    } );

    QObject::connect( wrappedEdit, &QQuickTextEdit::selectByMouseChanged,
        edit, &QskTextEdit::selectByMouseChanged );

    QObject::connect( wrappedEdit, &QQuickTextEdit::tabStopDistanceChanged,
        edit, &QskTextEdit::tabStopDistanceChanged );


    QObject::connect( wrappedEdit, &QQuickItem::implicitWidthChanged,
        edit, &QskControl::resetImplicitSize );

    QObject::connect( wrappedEdit, &QQuickItem::implicitHeightChanged,
        edit, &QskControl::resetImplicitSize );
}

namespace
{
    class QuickTextEdit final : public QQuickTextEdit
    {
        using Inherited = QQuickTextEdit;

      public:
        QuickTextEdit( QskTextEdit* );

        void setEditing( bool on );

        inline void setAlignment( Qt::Alignment alignment )
        {
            setHAlign( ( HAlignment ) ( int( alignment ) & 0x0f ) );
            setVAlign( ( VAlignment ) ( int( alignment ) & 0xf0 ) );
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

        inline bool hasSelectedText() const
        {
            return !selectedText().isEmpty();
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

    QuickTextEdit::QuickTextEdit( QskTextEdit* textField )
        : QQuickTextEdit( textField )
    {
        classBegin();

        setActiveFocusOnTab( false );
        setFlag( ItemAcceptsInputMethod, false );
        setFocusOnPress( false );
        setSelectByMouse( true );

        componentComplete();

        connect( this, &QuickTextEdit::contentSizeChanged,
            this, &QuickTextEdit::updateClip );
    }

    void QuickTextEdit::setEditing( bool on )
    {
        auto d = QQuickTextEditPrivate::get( this );

        if ( d->cursorVisible == on )
            return;

        setCursorVisible( on );

        polish();
        update();
    }

    void QuickTextEdit::updateMetrics()
    {
        auto textEdit = static_cast< const QskTextEdit* >( parentItem() );

        setAlignment( textEdit->alignment() );
        setFont( textEdit->font() );
    }

    void QuickTextEdit::updateColors()
    {
        using Q = QskTextEdit;

        auto input = static_cast< const Q* >( parentItem() );

        setColor( input->color( Q::Text ) );

        const auto state = QskTextEditSkinlet::Selected;

        setSelectionColor( input->color( Q::TextPanel | state ) );
        setSelectedTextColor( input->color( Q::Text | state ) );
    }
}

class QskTextEdit::PrivateData
{
  public:
    QuickTextEdit* wrappedEdit;

    ActivationModes activationModes;
};

QskTextEdit::QskTextEdit( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    m_data->activationModes = ActivationOnMouse | ActivationOnKey;

    setPolishOnResize( true );

    setAcceptHoverEvents( true );
    setFocusPolicy( Qt::StrongFocus );

    setFlag( QQuickItem::ItemAcceptsInputMethod );

    /*
        QQuickTextEdit is a beast of almost 3.5k lines of code, we don't
        want to reimplement that - at least not now.
        So this is more or less a simple wrapper making everything
        conforming to qskinny.
     */

    m_data->wrappedEdit = new QuickTextEdit( this );
    qskBindSignals( m_data->wrappedEdit, this );

    setAcceptedMouseButtons( m_data->wrappedEdit->acceptedMouseButtons() );
    m_data->wrappedEdit->setAcceptedMouseButtons( Qt::NoButton );

    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );
}

QskTextEdit::~QskTextEdit()
{
}

bool QskTextEdit::event( QEvent* event )
{
    if ( event->type() == QEvent::ShortcutOverride )
    {
        return m_data->wrappedEdit->handleEvent( event );
    }
    else if ( event->type() == QEvent::LocaleChange )
    {
        qskUpdateInputMethod( this, Qt::ImPreferredLanguage );
    }

    return Inherited::event( event );
}

void QskTextEdit::keyPressEvent( QKeyEvent* event )
{
    if ( isEditing() )
    {
        switch ( event->key() )
        {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            {
                QGuiApplication::inputMethod()->commit();

                if ( !( inputMethodHints() & Qt::ImhMultiLine ) )
                {
                    setEditing( false );

                    // When returning from a virtual keyboard
                    qskForceActiveFocus( this, Qt::PopupFocusReason );
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
                m_data->wrappedEdit->handleEvent( event );
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

void QskTextEdit::keyReleaseEvent( QKeyEvent* event )
{
    Inherited::keyReleaseEvent( event );
}

void QskTextEdit::mousePressEvent( QMouseEvent* event )
{
    m_data->wrappedEdit->handleEvent( event );

    if ( !isReadOnly() && !qGuiApp->styleHints()->setFocusOnTouchRelease() )
        setEditing( true );
}

void QskTextEdit::mouseMoveEvent( QMouseEvent* event )
{
    m_data->wrappedEdit->handleEvent( event );
}

void QskTextEdit::mouseReleaseEvent( QMouseEvent* event )
{
    m_data->wrappedEdit->handleEvent( event );

    if ( !isReadOnly() && qGuiApp->styleHints()->setFocusOnTouchRelease() )
        setEditing( true );
}

void QskTextEdit::mouseDoubleClickEvent( QMouseEvent* event )
{
    m_data->wrappedEdit->handleEvent( event );
}

void QskTextEdit::inputMethodEvent( QInputMethodEvent* event )
{
    m_data->wrappedEdit->handleEvent( event );
}

void QskTextEdit::focusInEvent( QFocusEvent* event )
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

void QskTextEdit::focusOutEvent( QFocusEvent* event )
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
            m_data->wrappedEdit->deselect();
            setEditing( false );
        }
    }

    Inherited::focusOutEvent( event );
}

void QskTextEdit::updateLayout()
{
    m_data->wrappedEdit->updateMetrics();
    qskSetItemGeometry( m_data->wrappedEdit, subControlRect( Text ) );
}

void QskTextEdit::updateNode( QSGNode* node )
{
    m_data->wrappedEdit->updateColors();
    Inherited::updateNode( node );
}

QString QskTextEdit::text() const
{
    return m_data->wrappedEdit->text();
}

void QskTextEdit::setText( const QString& text )
{
    m_data->wrappedEdit->setText( text );
}

void QskTextEdit::clear()
{
    m_data->wrappedEdit->clear();
}

void QskTextEdit::selectAll()
{
    m_data->wrappedEdit->selectAll();
}

QskTextEdit::ActivationModes QskTextEdit::activationModes() const
{
    return static_cast< QskTextEdit::ActivationModes >( m_data->activationModes );
}

void QskTextEdit::setActivationModes( ActivationModes modes )
{
    if ( static_cast< ActivationModes >( m_data->activationModes ) != modes )
    {
        m_data->activationModes = modes;
        Q_EMIT activationModesChanged();
    }
}

static inline void qskUpdateInputMethodFont( const QskTextEdit* input )
{
    const auto queries = Qt::ImCursorRectangle | Qt::ImFont | Qt::ImAnchorRectangle;
    qskUpdateInputMethod( input, queries );
}

void QskTextEdit::setFontRole( const QskFontRole& role )
{
    if ( setFontRoleHint( Text, role ) )
    {
        qskUpdateInputMethodFont( this );
        Q_EMIT fontRoleChanged();
    }
}

void QskTextEdit::resetFontRole()
{
    if ( resetFontRoleHint( Text ) )
    {
        qskUpdateInputMethodFont( this );
        Q_EMIT fontRoleChanged();
    }
}

QskFontRole QskTextEdit::fontRole() const
{
    return fontRoleHint( Text );
}

void QskTextEdit::setAlignment( Qt::Alignment alignment )
{
    if ( setAlignmentHint( Text, alignment ) )
    {
        m_data->wrappedEdit->setAlignment( alignment );
        Q_EMIT alignmentChanged();
    }
}

void QskTextEdit::resetAlignment()
{
    if ( resetAlignmentHint( Text ) )
    {
        m_data->wrappedEdit->setAlignment( alignment() );
        Q_EMIT alignmentChanged();
    }
}

Qt::Alignment QskTextEdit::alignment() const
{
    return alignmentHint( Text, Qt::AlignLeft | Qt::AlignTop );
}

void QskTextEdit::setWrapMode( QskTextOptions::WrapMode wrapMode )
{
    m_data->wrappedEdit->setWrapMode(
        static_cast< QQuickTextEdit::WrapMode >( wrapMode ) );
}

QskTextOptions::WrapMode QskTextEdit::wrapMode() const
{
    return static_cast< QskTextOptions::WrapMode >(
        m_data->wrappedEdit->wrapMode() );
}

void QskTextEdit::setSelectByMouse( bool on )
{
    m_data->wrappedEdit->setSelectByMouse( on );
}

bool QskTextEdit::selectByMouse() const
{
    return m_data->wrappedEdit->selectByMouse();
}

QFont QskTextEdit::font() const
{
    return effectiveFont( QskTextEdit::Text );
}

bool QskTextEdit::isReadOnly() const
{
    return m_data->wrappedEdit->isReadOnly();
}

void QskTextEdit::setReadOnly( bool on )
{
    auto edit = m_data->wrappedEdit;

    if ( edit->isReadOnly() == on )
        return;

#if 1
    // do we want to be able to restore the previous policy ?
    setFocusPolicy( Qt::NoFocus );
#endif

    edit->setReadOnly( on );

    // we are killing user settings here ?
    edit->setFlag( QQuickItem::ItemAcceptsInputMethod, !on );
    qskUpdateInputMethod( this, Qt::ImEnabled );

    setSkinStateFlag( ReadOnly, on );
}

void QskTextEdit::setEditing( bool on )
{
    if ( isReadOnly() || on == isEditing() )
        return;

    setSkinStateFlag( Editing, on );
    m_data->wrappedEdit->setEditing( on );

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
        Q_EMIT m_data->wrappedEdit->editingFinished();

#if 0
        inputMethod->reset();
#endif
        qskInputMethodSetVisible( this, false );
    }

    Q_EMIT editingChanged( on );
}

bool QskTextEdit::isEditing() const
{
    return hasSkinState( Editing );
}

int QskTextEdit::cursorPosition() const
{
    return m_data->wrappedEdit->cursorPosition();
}

void QskTextEdit::setCursorPosition( int pos )
{
    m_data->wrappedEdit->setCursorPosition( pos );
}

QString QskTextEdit::preeditText() const
{
    return m_data->wrappedEdit->preeditText();
}

bool QskTextEdit::overwriteMode() const
{
    return m_data->wrappedEdit->overwriteMode();
}

void QskTextEdit::setOverwriteMode( bool overwrite )
{
    m_data->wrappedEdit->setOverwriteMode( overwrite );
}

void QskTextEdit::setTextFormat( QskTextOptions::TextFormat textFormat )
{
    m_data->wrappedEdit->setTextFormat(
        static_cast< QQuickTextEdit::TextFormat >( textFormat ) );
}

QskTextOptions::TextFormat QskTextEdit::textFormat() const
{
    return static_cast< QskTextOptions::TextFormat >(
        m_data->wrappedEdit->textFormat() );
}

int QskTextEdit::lineCount() const
{
    return m_data->wrappedEdit->lineCount();
}

QVariant QskTextEdit::inputMethodQuery(
    Qt::InputMethodQuery property ) const
{
    return inputMethodQuery( property, QVariant() );
}

QVariant QskTextEdit::inputMethodQuery(
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
            QVariant v = m_data->wrappedEdit->inputMethodQuery( query, argument );
#if 1
            if ( v.canConvert< QRectF >() )
                v.setValue( v.toRectF().translated( m_data->wrappedEdit->position() ) );
#endif
            return v;
        }
        default:
        {
            return m_data->wrappedEdit->inputMethodQuery( query, argument );
        }
    }
}

bool QskTextEdit::canUndo() const
{
    return m_data->wrappedEdit->canUndo();
}

bool QskTextEdit::canRedo() const
{
    return m_data->wrappedEdit->canRedo();
}

Qt::InputMethodHints QskTextEdit::inputMethodHints() const
{
    return m_data->wrappedEdit->inputMethodHints();
}

void QskTextEdit::setInputMethodHints( Qt::InputMethodHints hints )
{
    if ( m_data->wrappedEdit->inputMethodHints() != hints )
    {
        m_data->wrappedEdit->setInputMethodHints( hints );
        qskUpdateInputMethod( this, Qt::ImHints );
    }
}

int QskTextEdit::tabStopDistance() const
{
    return m_data->wrappedEdit->tabStopDistance();
}

void QskTextEdit::setTabStopDistance( qreal distance )
{
    m_data->wrappedEdit->setTabStopDistance( distance );
}

void QskTextEdit::setupFrom( const QQuickItem* item )
{
    if ( item == nullptr )
        return;

    // finding attributes from the input hints of item

    int maxCharacters = 32767;

    Qt::InputMethodQueries queries = Qt::ImQueryAll;
    queries &= ~Qt::ImEnabled;

    QInputMethodQueryEvent event( queries );
    QCoreApplication::sendEvent( const_cast< QQuickItem* >( item ), &event );

    if ( event.queries() & Qt::ImMaximumTextLength )
    {
        // needs to be handled before Qt::ImCursorPosition !

        const auto max = event.value( Qt::ImMaximumTextLength ).toInt();
        maxCharacters = qBound( 0, max, maxCharacters );
    }

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
}

#include "moc_QskTextEdit.cpp"
