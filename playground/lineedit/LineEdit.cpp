/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLineEdit.h"

#include <QskAspect.h>

// VS2012+ disable keyword macroizing unless _ALLOW_KEYWORD_MACROS is set
#if ( _MSC_VER >= 1700 ) && !defined( _ALLOW_KEYWORD_MACROS )
#define _ALLOW_KEYWORD_MACROS
#endif

QSK_QT_PRIVATE_BEGIN
#define protected public
#define private public
#include <private/qquicktextinput_p.h>
#include <private/qquicktextinput_p_p.h>
#undef protected
#undef private
QSK_QT_PRIVATE_END

#define Q_P(ClassName) \
    auto p = reinterpret_cast< ClassName* >( this )

QSK_SUBCONTROL( QskLineEdit, Panel )
QSK_SUBCONTROL( QskLineEdit, Text )

class QskLineEditPrivate : public QQuickTextInputPrivate
{
    Q_DECLARE_PUBLIC(QskLineEdit)

    using Inherited = QQuickTextInputPrivate;
public:
    QLocale locale;

    void mirrorChange() override
    {
        Q_Q( QskControl );
        QEvent event( QEvent::LayoutDirectionChange );
        QCoreApplication::sendEvent( q, &event );
    }
};

QskLineEdit::QskLineEdit( QQuickItem* parent ):
    QskControl( *( new QskLineEditPrivate ), parent )
{
    Q_D( QskLineEdit );
    d->init();

    setActiveFocusOnTab( true );
}

QskLineEdit::~QskLineEdit()
{
}

void QskLineEdit::updateLayout()
{
    Inherited::updateLayout();

    const auto padding = edgeMetrics( Panel, QskAspect::Padding );

    Q_P( QQuickTextInput );
    p->QQuickTextInput::setLeftPadding( padding.left() );
    p->QQuickTextInput::setTopPadding( padding.top() );
    p->QQuickTextInput::setRightPadding( padding.right() );
    p->QQuickTextInput::setBottomPadding( padding.bottom() );
}

// Begin proxy methods

void QskLineEdit::componentComplete()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::componentComplete();
}

void QskLineEdit::positionAt( QQmlV4Function* args ) const
{
    Q_P( const QQuickTextInput );
    p->QQuickTextInput::positionAt( args );
}

QRectF QskLineEdit::positionToRectangle( int pos ) const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::positionToRectangle( pos );
}

void QskLineEdit::moveCursorSelection( int pos )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::moveCursorSelection( pos );
}

void QskLineEdit::moveCursorSelection( int pos, QskLineEdit::SelectionMode mode )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::moveCursorSelection(
        pos, static_cast< QQuickTextInput::SelectionMode >( mode ) );
}

QskLineEdit::RenderType QskLineEdit::renderType() const
{
    Q_P( const QQuickTextInput );
    return static_cast< QskLineEdit::RenderType >( p->QQuickTextInput::renderType() );
}

void QskLineEdit::setRenderType( RenderType renderType )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setRenderType( static_cast< QQuickTextInput::RenderType >( renderType ) );
}

QString QskLineEdit::text() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::text();
}

void QskLineEdit::setText( const QString& text )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setText( text );
}

int QskLineEdit::length() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::length();
}

#if 0

QFont QskLineEdit::font() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::font();
}

void QskLineEdit::setFont( const QFont& font )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setFont( font );
}

QColor QskLineEdit::color() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::color();
}

void QskLineEdit::setColor( const QColor& color )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setColor( color );
}

QColor QskLineEdit::selectionColor() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::selectionColor();
}

void QskLineEdit::setSelectionColor( const QColor& color )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setSelectionColor( color );
}

QColor QskLineEdit::selectedTextColor() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::selectedTextColor();
}

void QskLineEdit::setSelectedTextColor( const QColor& color )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setSelectedTextColor( color );
}

QskLineEdit::HAlignment QskLineEdit::hAlign() const
{
    Q_P( const QQuickTextInput );
    return static_cast< HAlignment >( p->QQuickTextInput::hAlign() );
}

void QskLineEdit::setHAlign( HAlignment alignment )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setHAlign( static_cast< QQuickTextInput::HAlignment >( alignment ) );
}

void QskLineEdit::resetHAlign()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::resetHAlign();
}

QskLineEdit::HAlignment QskLineEdit::effectiveHAlign() const
{
    Q_P( const QQuickTextInput );
    return static_cast< HAlignment >( p->QQuickTextInput::effectiveHAlign() );
}

QskLineEdit::VAlignment QskLineEdit::vAlign() const
{
    Q_P( const QQuickTextInput );
    return static_cast< VAlignment >( p->QQuickTextInput::vAlign() );
}

void QskLineEdit::setVAlign( QskLineEdit::VAlignment alignment )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setVAlign( static_cast< QQuickTextInput::VAlignment >( alignment ) );
}

QskLineEdit::WrapMode QskLineEdit::wrapMode() const
{
    Q_P( const QQuickTextInput );
    return static_cast< WrapMode >( p->QQuickTextInput::wrapMode() );
}

void QskLineEdit::setWrapMode( QskLineEdit::WrapMode wrapMode )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setWrapMode( static_cast< QQuickTextInput::WrapMode >( wrapMode ) );
}

#endif

bool QskLineEdit::isReadOnly() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::isReadOnly();
}

void QskLineEdit::setReadOnly( bool readOnly )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setReadOnly( readOnly );
}

bool QskLineEdit::isCursorVisible() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::isCursorVisible();
}

void QskLineEdit::setCursorVisible( bool cursorVisible )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setCursorVisible( cursorVisible );
}

int QskLineEdit::cursorPosition() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::cursorPosition();
}

void QskLineEdit::setCursorPosition( int cursorPosition )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setCursorPosition( cursorPosition );
}

QRectF QskLineEdit::cursorRectangle() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::cursorRectangle();
}

int QskLineEdit::selectionStart() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::selectionStart();
}

int QskLineEdit::selectionEnd() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::selectionEnd();
}

QString QskLineEdit::selectedText() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::selectedText();
}

int QskLineEdit::maxLength() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::maxLength();
}

void QskLineEdit::setMaxLength( int maxLength )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setMaxLength( maxLength );
}

QValidator* QskLineEdit::validator() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::validator();
}

void QskLineEdit::setValidator( QValidator* validator )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setValidator( validator );
}

QString QskLineEdit::inputMask() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::inputMask();
}

void QskLineEdit::setInputMask( const QString& inputMask )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setInputMask( inputMask );
}

QskLineEdit::EchoMode QskLineEdit::echoMode() const
{
    Q_P( const QQuickTextInput );
    return static_cast< EchoMode >( p->QQuickTextInput::echoMode() );
}

void QskLineEdit::setEchoMode( QskLineEdit::EchoMode echoMode )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setEchoMode( static_cast< QQuickTextInput::EchoMode >( echoMode ) );
}

QString QskLineEdit::passwordCharacter() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::passwordCharacter();
}

void QskLineEdit::setPasswordCharacter( const QString& passordCharacter )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setPasswordCharacter( passordCharacter );
}

int QskLineEdit::passwordMaskDelay() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::passwordMaskDelay();
}

void QskLineEdit::setPasswordMaskDelay( int delay )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setPasswordMaskDelay( delay );
}

void QskLineEdit::resetPasswordMaskDelay()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::resetPasswordMaskDelay();
}

QString QskLineEdit::displayText() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::displayText();
}

#if 0

QQmlComponent* QskLineEdit::cursorDelegate() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::cursorDelegate();
}

void QskLineEdit::setCursorDelegate( QQmlComponent* component )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setCursorDelegate( component );
}

#endif

bool QskLineEdit::focusOnPress() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::focusOnPress();
}

void QskLineEdit::setFocusOnPress( bool focusOnPress )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setFocusOnPress( focusOnPress );
}

bool QskLineEdit::autoScroll() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::autoScroll();
}

void QskLineEdit::setAutoScroll( bool autoScroll )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setAutoScroll( autoScroll );
}

bool QskLineEdit::selectByMouse() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::selectByMouse();
}

void QskLineEdit::setSelectByMouse( bool selectByMouse )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setSelectByMouse( selectByMouse );
}

QskLineEdit::SelectionMode QskLineEdit::mouseSelectionMode() const
{
    Q_P( const QQuickTextInput );
    return static_cast< SelectionMode >( p->QQuickTextInput::mouseSelectionMode() );
}

void QskLineEdit::setMouseSelectionMode( SelectionMode selectionMode )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setMouseSelectionMode(
        static_cast< QQuickTextInput::SelectionMode >( selectionMode ) );
}

bool QskLineEdit::persistentSelection() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::persistentSelection();
}

void QskLineEdit::setPersistentSelection( bool persistentSelection )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setPersistentSelection( persistentSelection );
}

bool QskLineEdit::hasAcceptableInput() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::hasAcceptableInput();
}

QVariant QskLineEdit::inputMethodQuery( Qt::InputMethodQuery inputMethodQuery ) const
{
    Q_D( const QskLineEdit );
    Q_P( const QQuickTextInput );
    auto value = p->QQuickTextInput::inputMethodQuery( inputMethodQuery );
    if ( inputMethodQuery == Qt::ImPreferredLanguage && value.isNull() )
        return d->locale;
    return value;
}

QVariant QskLineEdit::inputMethodQuery(
    Qt::InputMethodQuery inputMethodQuery, QVariant argument ) const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::inputMethodQuery( inputMethodQuery, argument );
}

QRectF QskLineEdit::boundingRect() const
{
    // Special case: we use the control definition of boundingRect instead of
    // QQuickLineEdit's, because boundingRect is used in layout and rendering
    return Inherited::boundingRect();
}

QRectF QskLineEdit::clipRect() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::clipRect();
}

bool QskLineEdit::canPaste() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::canPaste();
}

bool QskLineEdit::canUndo() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::canUndo();
}

bool QskLineEdit::canRedo() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::canRedo();
}

bool QskLineEdit::isInputMethodComposing() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::isInputMethodComposing();
}

Qt::InputMethodHints QskLineEdit::inputMethodHints() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::inputMethodHints();
}

void QskLineEdit::setInputMethodHints( Qt::InputMethodHints inputMethodHints )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setInputMethodHints( inputMethodHints );
}

QString QskLineEdit::getText( int start, int end ) const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::getText( start, end );
}

qreal QskLineEdit::contentWidth() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::contentWidth();
}

qreal QskLineEdit::contentHeight() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::contentHeight();
}

#if 0

qreal QskLineEdit::padding() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::padding();
}

void QskLineEdit::setPadding( qreal padding )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setPadding( padding );
}

void QskLineEdit::resetPadding()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::resetPadding();
}

qreal QskLineEdit::topPadding() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::padding();
}

void QskLineEdit::setTopPadding( qreal topPadding )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setTopPadding( topPadding );
}

void QskLineEdit::resetTopPadding()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::resetTopPadding();
}

qreal QskLineEdit::leftPadding() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::leftPadding();
}

void QskLineEdit::setLeftPadding( qreal leftPadding )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setLeftPadding( leftPadding );
}

void QskLineEdit::resetLeftPadding()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::resetLeftPadding();
}

qreal QskLineEdit::rightPadding() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::leftPadding();
}

void QskLineEdit::setRightPadding( qreal rightPadding )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setRightPadding( rightPadding );
}

void QskLineEdit::resetRightPadding()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::resetRightPadding();
}

qreal QskLineEdit::bottomPadding() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::leftPadding();
}

void QskLineEdit::setBottomPadding( qreal bottomPadding )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setBottomPadding( bottomPadding );
}

void QskLineEdit::resetBottomPadding()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::resetBottomPadding();
}

#endif

void QskLineEdit::geometryChanged( const QRectF& newGeometry, const QRectF& oldGeometry )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::geometryChanged( newGeometry, oldGeometry );
}

void QskLineEdit::mousePressEvent( QMouseEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::mousePressEvent( event );
}

void QskLineEdit::mouseMoveEvent( QMouseEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::mouseMoveEvent( event );
}

void QskLineEdit::mouseReleaseEvent( QMouseEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::mouseReleaseEvent( event );
}

void QskLineEdit::mouseDoubleClickEvent( QMouseEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::mouseDoubleClickEvent( event );
}

void QskLineEdit::keyPressEvent( QKeyEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::keyPressEvent( event );
}

void QskLineEdit::inputMethodEvent( QInputMethodEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::inputMethodEvent( event );
}

void QskLineEdit::mouseUngrabEvent()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::mouseUngrabEvent();
}

bool QskLineEdit::event( QEvent* event )
{
    Q_P( QQuickTextInput );
    return p->QQuickTextInput::event( event );
}

void QskLineEdit::focusOutEvent( QFocusEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::focusOutEvent( event );
}

void QskLineEdit::focusInEvent( QFocusEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::focusInEvent( event );
}

void QskLineEdit::timerEvent( QTimerEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::timerEvent( event );
}

void QskLineEdit::selectAll()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::selectAll();
}

void QskLineEdit::selectWord()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::selectWord();
}

void QskLineEdit::select( int start, int end )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::select( start, end );
}

void QskLineEdit::deselect()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::deselect();
}

bool QskLineEdit::isRightToLeft( int start, int end )
{
    Q_P( QQuickTextInput );
    return p->QQuickTextInput::isRightToLeft( start, end );
}

#ifndef QT_NO_CLIPBOARD

void QskLineEdit::cut()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::cut();
}

void QskLineEdit::copy()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::copy();
}

void QskLineEdit::paste()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::paste();
}

#endif // QT_NO_CLIPBOARD

void QskLineEdit::undo()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::undo();
}

void QskLineEdit::redo()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::redo();
}

void QskLineEdit::insert( int position, const QString& text )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::insert( position, text );
}

void QskLineEdit::remove( int start, int end )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::remove( start, end );
}

void QskLineEdit::ensureVisible( int position )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::ensureVisible( position );
}

void QskLineEdit::selectionChanged()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::selectionChanged();
}

void QskLineEdit::createCursor()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::createCursor();
}

void QskLineEdit::updateCursorRectangle( bool scroll )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::updateCursorRectangle( scroll );
}

void QskLineEdit::q_canPasteChanged()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::q_canPasteChanged();
}

void QskLineEdit::q_updateAlignment()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::q_updateAlignment();
}

void QskLineEdit::triggerPreprocess()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::triggerPreprocess();
}

#ifndef QT_NO_VALIDATOR
void QskLineEdit::q_validatorChanged()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::q_validatorChanged();
}
#endif

// end proxy methods

QSGNode* QskLineEdit::updateTextInputNode( QSGNode* node )
{
    Q_P( QQuickTextInput );
    return p->QQuickTextInput::updatePaintNode( node, nullptr );
}

#include "moc_QskLineEdit.cpp"
