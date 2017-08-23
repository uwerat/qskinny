/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "LineEdit.h"
#include "LineEditSkinlet.h"

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

QSK_SUBCONTROL( LineEdit, Panel )
QSK_SUBCONTROL( LineEdit, Text )

class LineEditPrivate : public QQuickTextInputPrivate
{
    Q_DECLARE_PUBLIC(LineEdit)

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

LineEdit::LineEdit( QQuickItem* parent ):
    QskControl( *( new LineEditPrivate ), parent )
{
    Q_D( LineEdit );
    d->init();

    setActiveFocusOnTab( true );
#if 1
    auto skinlet = new LineEditSkinlet();
    skinlet->setOwnedBySkinnable( true );

    setSkinlet( skinlet );
#endif
}

LineEdit::~LineEdit()
{
}

void LineEdit::updateLayout()
{
    Inherited::updateLayout();

    const QMarginsF padding = marginsHint( Panel | QskAspect::Padding );

    Q_P( QQuickTextInput );
    p->QQuickTextInput::setLeftPadding( padding.left() );
    p->QQuickTextInput::setTopPadding( padding.top() );
    p->QQuickTextInput::setRightPadding( padding.right() );
    p->QQuickTextInput::setBottomPadding( padding.bottom() );
}

// Begin proxy methods

void LineEdit::componentComplete()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::componentComplete();
}

void LineEdit::positionAt( QQmlV4Function* args ) const
{
    Q_P( const QQuickTextInput );
    p->QQuickTextInput::positionAt( args );
}

QRectF LineEdit::positionToRectangle( int pos ) const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::positionToRectangle( pos );
}

void LineEdit::moveCursorSelection( int pos )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::moveCursorSelection( pos );
}

void LineEdit::moveCursorSelection( int pos, LineEdit::SelectionMode mode )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::moveCursorSelection(
        pos, static_cast< QQuickTextInput::SelectionMode >( mode ) );
}

LineEdit::RenderType LineEdit::renderType() const
{
    Q_P( const QQuickTextInput );
    return static_cast< LineEdit::RenderType >( p->QQuickTextInput::renderType() );
}

void LineEdit::setRenderType( RenderType renderType )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setRenderType( static_cast< QQuickTextInput::RenderType >( renderType ) );
}

QString LineEdit::text() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::text();
}

void LineEdit::setText( const QString& text )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setText( text );
}

int LineEdit::length() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::length();
}

#if 0

QFont LineEdit::font() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::font();
}

void LineEdit::setFont( const QFont& font )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setFont( font );
}

QColor LineEdit::color() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::color();
}

void LineEdit::setColor( const QColor& color )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setColor( color );
}

QColor LineEdit::selectionColor() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::selectionColor();
}

void LineEdit::setSelectionColor( const QColor& color )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setSelectionColor( color );
}

QColor LineEdit::selectedTextColor() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::selectedTextColor();
}

void LineEdit::setSelectedTextColor( const QColor& color )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setSelectedTextColor( color );
}

LineEdit::HAlignment LineEdit::hAlign() const
{
    Q_P( const QQuickTextInput );
    return static_cast< HAlignment >( p->QQuickTextInput::hAlign() );
}

void LineEdit::setHAlign( HAlignment alignment )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setHAlign( static_cast< QQuickTextInput::HAlignment >( alignment ) );
}

void LineEdit::resetHAlign()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::resetHAlign();
}

LineEdit::HAlignment LineEdit::effectiveHAlign() const
{
    Q_P( const QQuickTextInput );
    return static_cast< HAlignment >( p->QQuickTextInput::effectiveHAlign() );
}

LineEdit::VAlignment LineEdit::vAlign() const
{
    Q_P( const QQuickTextInput );
    return static_cast< VAlignment >( p->QQuickTextInput::vAlign() );
}

void LineEdit::setVAlign( LineEdit::VAlignment alignment )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setVAlign( static_cast< QQuickTextInput::VAlignment >( alignment ) );
}

LineEdit::WrapMode LineEdit::wrapMode() const
{
    Q_P( const QQuickTextInput );
    return static_cast< WrapMode >( p->QQuickTextInput::wrapMode() );
}

void LineEdit::setWrapMode( LineEdit::WrapMode wrapMode )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setWrapMode( static_cast< QQuickTextInput::WrapMode >( wrapMode ) );
}

#endif

bool LineEdit::isReadOnly() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::isReadOnly();
}

void LineEdit::setReadOnly( bool readOnly )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setReadOnly( readOnly );
}

bool LineEdit::isCursorVisible() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::isCursorVisible();
}

void LineEdit::setCursorVisible( bool cursorVisible )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setCursorVisible( cursorVisible );
}

int LineEdit::cursorPosition() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::cursorPosition();
}

void LineEdit::setCursorPosition( int cursorPosition )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setCursorPosition( cursorPosition );
}

QRectF LineEdit::cursorRectangle() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::cursorRectangle();
}

int LineEdit::selectionStart() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::selectionStart();
}

int LineEdit::selectionEnd() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::selectionEnd();
}

QString LineEdit::selectedText() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::selectedText();
}

int LineEdit::maxLength() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::maxLength();
}

void LineEdit::setMaxLength( int maxLength )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setMaxLength( maxLength );
}

QValidator* LineEdit::validator() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::validator();
}

void LineEdit::setValidator( QValidator* validator )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setValidator( validator );
}

QString LineEdit::inputMask() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::inputMask();
}

void LineEdit::setInputMask( const QString& inputMask )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setInputMask( inputMask );
}

LineEdit::EchoMode LineEdit::echoMode() const
{
    Q_P( const QQuickTextInput );
    return static_cast< EchoMode >( p->QQuickTextInput::echoMode() );
}

void LineEdit::setEchoMode( LineEdit::EchoMode echoMode )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setEchoMode( static_cast< QQuickTextInput::EchoMode >( echoMode ) );
}

QString LineEdit::passwordCharacter() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::passwordCharacter();
}

void LineEdit::setPasswordCharacter( const QString& passordCharacter )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setPasswordCharacter( passordCharacter );
}

int LineEdit::passwordMaskDelay() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::passwordMaskDelay();
}

void LineEdit::setPasswordMaskDelay( int delay )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setPasswordMaskDelay( delay );
}

void LineEdit::resetPasswordMaskDelay()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::resetPasswordMaskDelay();
}

QString LineEdit::displayText() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::displayText();
}

#if 0

QQmlComponent* LineEdit::cursorDelegate() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::cursorDelegate();
}

void LineEdit::setCursorDelegate( QQmlComponent* component )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setCursorDelegate( component );
}

#endif

bool LineEdit::focusOnPress() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::focusOnPress();
}

void LineEdit::setFocusOnPress( bool focusOnPress )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setFocusOnPress( focusOnPress );
}

bool LineEdit::autoScroll() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::autoScroll();
}

void LineEdit::setAutoScroll( bool autoScroll )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setAutoScroll( autoScroll );
}

bool LineEdit::selectByMouse() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::selectByMouse();
}

void LineEdit::setSelectByMouse( bool selectByMouse )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setSelectByMouse( selectByMouse );
}

LineEdit::SelectionMode LineEdit::mouseSelectionMode() const
{
    Q_P( const QQuickTextInput );
    return static_cast< SelectionMode >( p->QQuickTextInput::mouseSelectionMode() );
}

void LineEdit::setMouseSelectionMode( SelectionMode selectionMode )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setMouseSelectionMode(
        static_cast< QQuickTextInput::SelectionMode >( selectionMode ) );
}

bool LineEdit::persistentSelection() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::persistentSelection();
}

void LineEdit::setPersistentSelection( bool persistentSelection )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setPersistentSelection( persistentSelection );
}

bool LineEdit::hasAcceptableInput() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::hasAcceptableInput();
}

QVariant LineEdit::inputMethodQuery( Qt::InputMethodQuery inputMethodQuery ) const
{
    Q_D( const LineEdit );
    Q_P( const QQuickTextInput );
    auto value = p->QQuickTextInput::inputMethodQuery( inputMethodQuery );
    if ( inputMethodQuery == Qt::ImPreferredLanguage && value.isNull() )
        return d->locale;
    return value;
}

QVariant LineEdit::inputMethodQuery(
    Qt::InputMethodQuery inputMethodQuery, QVariant argument ) const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::inputMethodQuery( inputMethodQuery, argument );
}

QRectF LineEdit::boundingRect() const
{
    // Special case: we use the control definition of boundingRect instead of
    // QQuickLineEdit's, because boundingRect is used in layout and rendering
    return Inherited::boundingRect();
}

QRectF LineEdit::clipRect() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::clipRect();
}

bool LineEdit::canPaste() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::canPaste();
}

bool LineEdit::canUndo() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::canUndo();
}

bool LineEdit::canRedo() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::canRedo();
}

bool LineEdit::isInputMethodComposing() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::isInputMethodComposing();
}

Qt::InputMethodHints LineEdit::inputMethodHints() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::inputMethodHints();
}

void LineEdit::setInputMethodHints( Qt::InputMethodHints inputMethodHints )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setInputMethodHints( inputMethodHints );
}

QString LineEdit::getText( int start, int end ) const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::getText( start, end );
}

qreal LineEdit::contentWidth() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::contentWidth();
}

qreal LineEdit::contentHeight() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::contentHeight();
}

#if 0

qreal LineEdit::padding() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::padding();
}

void LineEdit::setPadding( qreal padding )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setPadding( padding );
}

void LineEdit::resetPadding()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::resetPadding();
}

qreal LineEdit::topPadding() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::padding();
}

void LineEdit::setTopPadding( qreal topPadding )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setTopPadding( topPadding );
}

void LineEdit::resetTopPadding()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::resetTopPadding();
}

qreal LineEdit::leftPadding() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::leftPadding();
}

void LineEdit::setLeftPadding( qreal leftPadding )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setLeftPadding( leftPadding );
}

void LineEdit::resetLeftPadding()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::resetLeftPadding();
}

qreal LineEdit::rightPadding() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::leftPadding();
}

void LineEdit::setRightPadding( qreal rightPadding )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setRightPadding( rightPadding );
}

void LineEdit::resetRightPadding()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::resetRightPadding();
}

qreal LineEdit::bottomPadding() const
{
    Q_P( const QQuickTextInput );
    return p->QQuickTextInput::leftPadding();
}

void LineEdit::setBottomPadding( qreal bottomPadding )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::setBottomPadding( bottomPadding );
}

void LineEdit::resetBottomPadding()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::resetBottomPadding();
}

#endif

void LineEdit::geometryChanged( const QRectF& newGeometry, const QRectF& oldGeometry )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::geometryChanged( newGeometry, oldGeometry );
}

void LineEdit::mousePressEvent( QMouseEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::mousePressEvent( event );
}

void LineEdit::mouseMoveEvent( QMouseEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::mouseMoveEvent( event );
}

void LineEdit::mouseReleaseEvent( QMouseEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::mouseReleaseEvent( event );
}

void LineEdit::mouseDoubleClickEvent( QMouseEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::mouseDoubleClickEvent( event );
}

void LineEdit::keyPressEvent( QKeyEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::keyPressEvent( event );
}

void LineEdit::inputMethodEvent( QInputMethodEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::inputMethodEvent( event );
}

void LineEdit::mouseUngrabEvent()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::mouseUngrabEvent();
}

bool LineEdit::event( QEvent* event )
{
    Q_P( QQuickTextInput );
    return p->QQuickTextInput::event( event );
}

void LineEdit::focusOutEvent( QFocusEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::focusOutEvent( event );
}

void LineEdit::focusInEvent( QFocusEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::focusInEvent( event );
}

void LineEdit::timerEvent( QTimerEvent* event )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::timerEvent( event );
}

void LineEdit::selectAll()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::selectAll();
}

void LineEdit::selectWord()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::selectWord();
}

void LineEdit::select( int start, int end )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::select( start, end );
}

void LineEdit::deselect()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::deselect();
}

bool LineEdit::isRightToLeft( int start, int end )
{
    Q_P( QQuickTextInput );
    return p->QQuickTextInput::isRightToLeft( start, end );
}

#ifndef QT_NO_CLIPBOARD

void LineEdit::cut()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::cut();
}

void LineEdit::copy()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::copy();
}

void LineEdit::paste()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::paste();
}

#endif // QT_NO_CLIPBOARD

void LineEdit::undo()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::undo();
}

void LineEdit::redo()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::redo();
}

void LineEdit::insert( int position, const QString& text )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::insert( position, text );
}

void LineEdit::remove( int start, int end )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::remove( start, end );
}

void LineEdit::ensureVisible( int position )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::ensureVisible( position );
}

void LineEdit::selectionChanged()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::selectionChanged();
}

void LineEdit::createCursor()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::createCursor();
}

void LineEdit::updateCursorRectangle( bool scroll )
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::updateCursorRectangle( scroll );
}

void LineEdit::q_canPasteChanged()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::q_canPasteChanged();
}

void LineEdit::q_updateAlignment()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::q_updateAlignment();
}

void LineEdit::triggerPreprocess()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::triggerPreprocess();
}

#ifndef QT_NO_VALIDATOR
void LineEdit::q_validatorChanged()
{
    Q_P( QQuickTextInput );
    p->QQuickTextInput::q_validatorChanged();
}
#endif

// end proxy methods

QSGNode* LineEdit::updateTextInputNode( QSGNode* node )
{
    Q_P( QQuickTextInput );
    return p->QQuickTextInput::updatePaintNode( node, nullptr );
}

#include "moc_LineEdit.cpp"
