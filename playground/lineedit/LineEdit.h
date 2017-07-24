/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef _LINE_EDIT_H
#define _LINE_EDIT_H

#include "QskControl.h"

#include <QTextOption>

class QValidator;
class QQuickTextInput;

class LineEditPrivate;
class LineEdit : public QskControl
{
    Q_OBJECT

#if 1 // begin QQuickTextInput properties

#if 0
    // properties we hide from C++, as they are clash with QskSkinnable skin hints

    Q_PRIVATE_PROPERTY( p_func(), QColor color READ color
        WRITE setColor NOTIFY colorChanged )

    Q_PRIVATE_PROPERTY( p_func(), QColor selectionColor READ selectionColor
        WRITE setSelectionColor NOTIFY selectionColorChanged )

    Q_PRIVATE_PROPERTY( p_func(), QColor selectedTextColor READ selectedTextColor
        WRITE setSelectedTextColor NOTIFY selectedTextColorChanged )

    Q_PRIVATE_PROPERTY( p_func(), QFont font READ font
        WRITE setFont NOTIFY fontChanged )

    Q_PRIVATE_PROPERTY( p_func(), HAlignment horizontalAlignment READ hAlign
        WRITE setHAlign RESET resetHAlign NOTIFY horizontalAlignmentChanged )

    Q_PRIVATE_PROPERTY( p_func(), HAlignment effectiveHorizontalAlignment
        READ effectiveHAlign NOTIFY effectiveHorizontalAlignmentChanged )

    Q_PRIVATE_PROPERTY( p_func(), VAlignment verticalAlignment READ vAlign
        WRITE setVAlign NOTIFY verticalAlignmentChanged )

    Q_PRIVATE_PROPERTY( p_func(), qreal padding READ padding
        WRITE setPadding RESET resetPadding NOTIFY paddingChanged )

    Q_PRIVATE_PROPERTY( p_func(), qreal topPadding READ topPadding
        WRITE setTopPadding RESET resetTopPadding NOTIFY topPaddingChanged )

    Q_PRIVATE_PROPERTY( p_func(), qreal leftPadding READ leftPadding
        WRITE setLeftPadding RESET resetLeftPadding NOTIFY leftPaddingChanged )

    Q_PRIVATE_PROPERTY( p_func(), qreal rightPadding READ rightPadding
        WRITE setRightPadding RESET resetRightPadding NOTIFY rightPaddingChanged )

    Q_PRIVATE_PROPERTY( p_func(), qreal bottomPadding READ bottomPadding
        WRITE setBottomPadding RESET resetBottomPadding NOTIFY bottomPaddingChanged )

    // text options
    Q_PRIVATE_PROPERTY( p_func(), WrapMode wrapMode READ wrapMode
        WRITE setWrapMode NOTIFY wrapModeChanged )

    // skinlet
    Q_PRIVATE_PROPERTY( p_func(), QQmlComponent* cursorDelegate READ cursorDelegate
        WRITE setCursorDelegate NOTIFY cursorDelegateChanged )
#endif

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(int length READ length NOTIFY textChanged)

    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(bool cursorVisible READ isCursorVisible WRITE setCursorVisible NOTIFY cursorVisibleChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(QRectF cursorRectangle READ cursorRectangle NOTIFY cursorRectangleChanged)
    Q_PROPERTY(int selectionStart READ selectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd NOTIFY selectionEndChanged)
    Q_PROPERTY(QString selectedText READ selectedText NOTIFY selectedTextChanged)

    Q_PROPERTY(int maximumLength READ maxLength WRITE setMaxLength NOTIFY maximumLengthChanged)
    Q_PROPERTY(QValidator* validator READ validator WRITE setValidator NOTIFY validatorChanged)
    Q_PROPERTY(QString inputMask READ inputMask WRITE setInputMask NOTIFY inputMaskChanged)
    Q_PROPERTY(Qt::InputMethodHints inputMethodHints READ inputMethodHints WRITE setInputMethodHints NOTIFY inputMethodHintsChanged)

    Q_PROPERTY(bool acceptableInput READ hasAcceptableInput NOTIFY acceptableInputChanged)
    Q_PROPERTY(EchoMode echoMode READ echoMode WRITE setEchoMode NOTIFY echoModeChanged)
    Q_PROPERTY(bool activeFocusOnPress READ focusOnPress WRITE setFocusOnPress NOTIFY activeFocusOnPressChanged)
    Q_PROPERTY(QString passwordCharacter READ passwordCharacter WRITE setPasswordCharacter NOTIFY passwordCharacterChanged)
    Q_PROPERTY(int passwordMaskDelay READ passwordMaskDelay WRITE setPasswordMaskDelay RESET resetPasswordMaskDelay NOTIFY passwordMaskDelayChanged)
    Q_PROPERTY(QString displayText READ displayText NOTIFY displayTextChanged)
    Q_PROPERTY(bool autoScroll READ autoScroll WRITE setAutoScroll NOTIFY autoScrollChanged)
    Q_PROPERTY(bool selectByMouse READ selectByMouse WRITE setSelectByMouse NOTIFY selectByMouseChanged)
    Q_PROPERTY(SelectionMode mouseSelectionMode READ mouseSelectionMode WRITE setMouseSelectionMode NOTIFY mouseSelectionModeChanged)
    Q_PROPERTY(bool persistentSelection READ persistentSelection WRITE setPersistentSelection NOTIFY persistentSelectionChanged)
    Q_PROPERTY(bool canPaste READ canPaste NOTIFY canPasteChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(bool inputMethodComposing READ isInputMethodComposing NOTIFY inputMethodComposingChanged)
    Q_PROPERTY(qreal contentWidth READ contentWidth NOTIFY contentSizeChanged)
    Q_PROPERTY(qreal contentHeight READ contentHeight NOTIFY contentSizeChanged)
    Q_PROPERTY(RenderType renderType READ renderType WRITE setRenderType NOTIFY renderTypeChanged)
#endif

    using Inherited = QskControl;

public:
    QSK_SUBCONTROLS( Panel, Text )

    LineEdit( QQuickItem* parent = nullptr );
    ~LineEdit();

    virtual void updateLayout() override;

#if 1 // begin QQuickTextInput API
    enum EchoMode
    {
        //To match QLineEdit::EchoMode
        Normal,
        NoEcho,
        Password,
        PasswordEchoOnEdit
    };
    Q_ENUM(EchoMode)

#if 0 // provided by other means
    enum HAlignment
    {
        AlignLeft = Qt::AlignLeft,
        AlignRight = Qt::AlignRight,
        AlignHCenter = Qt::AlignHCenter
    };
    Q_ENUM(HAlignment)

    enum VAlignment
    {
        AlignTop = Qt::AlignTop,
        AlignBottom = Qt::AlignBottom,
        AlignVCenter = Qt::AlignVCenter
    };
    Q_ENUM(VAlignment)

    enum WrapMode
    {
        NoWrap = QTextOption::NoWrap,
        WordWrap = QTextOption::WordWrap,
        WrapAnywhere = QTextOption::WrapAnywhere,
        WrapAtWordBoundaryOrAnywhere = QTextOption::WrapAtWordBoundaryOrAnywhere, // COMPAT
        Wrap = QTextOption::WrapAtWordBoundaryOrAnywhere
    };
    Q_ENUM(WrapMode)
#endif

    enum SelectionMode
    {
        SelectCharacters,
        SelectWords
    };
    Q_ENUM(SelectionMode)

    enum CursorPosition
    {
        CursorBetweenCharacters,
        CursorOnCharacter
    };
    Q_ENUM(CursorPosition)

    enum RenderType { QtRendering, NativeRendering };
    Q_ENUM(RenderType)

#ifndef QT_NO_QML
    virtual void componentComplete() override;

    Q_INVOKABLE void positionAt( QQmlV4Function* ) const;
#endif

    Q_INVOKABLE QRectF positionToRectangle( int ) const;
    Q_INVOKABLE void moveCursorSelection( int );
    Q_INVOKABLE void moveCursorSelection( int, SelectionMode );

    RenderType renderType() const;
    void setRenderType( RenderType );

    QString text() const;
    void setText( const QString& );

    int length() const;

#if 0 // Equivalents provided by QskSkinnable

    QFont font() const;
    void setFont( const QFont& font );

    QColor color() const;
    void setColor( const QColor& );

    QColor selectionColor() const;
    void setSelectionColor( const QColor& );

    QColor selectedTextColor() const;
    void setSelectedTextColor( const QColor& );

    HAlignment hAlign() const;
    void setHAlign( HAlignment );
    void resetHAlign();
    HAlignment effectiveHAlign() const;

    VAlignment vAlign() const;
    void setVAlign( VAlignment );

    WrapMode wrapMode() const;
    void setWrapMode( WrapMode );

    qreal padding() const;
    void setPadding(qreal padding);
    void resetPadding();

    qreal topPadding() const;
    void setTopPadding(qreal padding);
    void resetTopPadding();

    qreal leftPadding() const;
    void setLeftPadding(qreal padding);
    void resetLeftPadding();

    qreal rightPadding() const;
    void setRightPadding(qreal padding);
    void resetRightPadding();

    qreal bottomPadding() const;
    void setBottomPadding(qreal padding);
    void resetBottomPadding();

#endif

    bool isReadOnly() const;
    void setReadOnly( bool );

    bool isCursorVisible() const;
    void setCursorVisible( bool );

    int cursorPosition() const;
    void setCursorPosition( int );

    QRectF cursorRectangle() const;

    int selectionStart() const;
    int selectionEnd() const;

    QString selectedText() const;

    int maxLength() const;
    void setMaxLength( int );

    QValidator* validator() const;
    void setValidator( QValidator* );

    QString inputMask() const;
    void setInputMask( const QString& );

    EchoMode echoMode() const;
    void setEchoMode( EchoMode echo );

    QString passwordCharacter() const;
    void setPasswordCharacter(const QString& str);

    int passwordMaskDelay() const;
    void setPasswordMaskDelay(int delay);
    void resetPasswordMaskDelay();

    QString displayText() const;

#if 0 // provided by skinlet
    QQmlComponent* cursorDelegate() const;
    void setCursorDelegate(QQmlComponent*);
#endif

    bool focusOnPress() const;
    void setFocusOnPress(bool);

    bool autoScroll() const;
    void setAutoScroll(bool);

    bool selectByMouse() const;
    void setSelectByMouse(bool);

    SelectionMode mouseSelectionMode() const;
    void setMouseSelectionMode(SelectionMode mode);

    bool persistentSelection() const;
    void setPersistentSelection(bool persist);

    bool hasAcceptableInput() const;

#ifndef QT_NO_IM
    QVariant inputMethodQuery(Qt::InputMethodQuery property) const Q_DECL_OVERRIDE;
    Q_INVOKABLE QVariant inputMethodQuery(Qt::InputMethodQuery query, QVariant argument) const;
#endif

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QRectF clipRect() const Q_DECL_OVERRIDE;

    bool canPaste() const;

    bool canUndo() const;
    bool canRedo() const;

    bool isInputMethodComposing() const;

    Qt::InputMethodHints inputMethodHints() const;
    void setInputMethodHints(Qt::InputMethodHints hints);

    Q_INVOKABLE QString getText(int start, int end) const;

    qreal contentWidth() const;
    qreal contentHeight() const;

protected:
    virtual void geometryChanged( const QRectF&, const QRectF& ) override;

    virtual void mousePressEvent( QMouseEvent* ) override;
    virtual void mouseMoveEvent( QMouseEvent* ) override;
    virtual void mouseReleaseEvent( QMouseEvent* ) override;
    virtual void mouseDoubleClickEvent( QMouseEvent* ) override;
    virtual void keyPressEvent( QKeyEvent* ) override;
#ifndef QT_NO_IM
    virtual void inputMethodEvent( QInputMethodEvent* ) override;
#endif
    virtual void mouseUngrabEvent() override;
    virtual bool event(QEvent* ) override;
    virtual void focusOutEvent(QFocusEvent* ) override;
    virtual void focusInEvent(QFocusEvent* ) override;
    virtual void timerEvent(QTimerEvent* ) override;

Q_SIGNALS:
    void textChanged();
    void cursorPositionChanged();
    void cursorRectangleChanged();
    void selectionStartChanged();
    void selectionEndChanged();
    void selectedTextChanged();
    void accepted();
    void acceptableInputChanged();
    void editingFinished();
#ifdef QSK_MAKEDLL
    void colorChanged();
    void selectionColorChanged();
    void selectedTextColorChanged();
    void fontChanged(const QFont& );
    void horizontalAlignmentChanged( int alignment );
    void verticalAlignmentChanged( int alignment );
    void wrapModeChanged();
#endif
    void readOnlyChanged(bool isReadOnly);
    void cursorVisibleChanged(bool isCursorVisible);
    void cursorDelegateChanged();
    void maximumLengthChanged(int maximumLength);
    void validatorChanged();
    void inputMaskChanged(const QString& inputMask);
    void echoModeChanged(LineEdit::EchoMode echoMode);
    void passwordCharacterChanged();
    void passwordMaskDelayChanged(int delay);
    void displayTextChanged();
    void activeFocusOnPressChanged(bool activeFocusOnPress);
    void autoScrollChanged(bool autoScroll);
    void selectByMouseChanged(bool selectByMouse);
    void mouseSelectionModeChanged(LineEdit::SelectionMode mode);
    void persistentSelectionChanged();
    void canPasteChanged();
    void canUndoChanged();
    void canRedoChanged();
    void inputMethodComposingChanged();
    void effectiveHorizontalAlignmentChanged();
    void contentSizeChanged();
    void inputMethodHintsChanged();
    void renderTypeChanged();
#ifdef QSK_MAKEDLL
    void paddingChanged();
    void topPaddingChanged();
    void leftPaddingChanged();
    void rightPaddingChanged();
    void bottomPaddingChanged();
#endif

    void localeChanged();
    void fontChanged();

public Q_SLOTS:
    void selectAll();
    void selectWord();
    void select( int, int );
    void deselect();
    bool isRightToLeft( int, int );
#ifndef QT_NO_CLIPBOARD
    void cut();
    void copy();
    void paste();
#endif
    void undo();
    void redo();
    void insert( int, const QString& );
    void remove( int, int );
    void ensureVisible(int position);

private Q_SLOTS:
    void selectionChanged();
    void createCursor();
    void updateCursorRectangle( bool = true );
    void q_canPasteChanged();
    void q_updateAlignment();
    void triggerPreprocess();

#ifndef QT_NO_VALIDATOR
    void q_validatorChanged();
#endif

#endif // end QQuickTextInput API

private:
    QSGNode* updateTextInputNode( QSGNode* );

    QQuickTextInput* p_func() const;

    Q_DECLARE_PRIVATE( LineEdit )

    friend class LineEditSkinlet;
};

#endif
