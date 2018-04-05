/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TEXT_INPUT_H
#define QSK_TEXT_INPUT_H

#include "QskControl.h"

class QskTextOptions;
class QValidator;

class QSK_EXPORT QskTextInput : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text WRITE setText NOTIFY textChanged )

    Q_PROPERTY( int fontRole READ fontRole
        WRITE setFontRole NOTIFY fontRoleChanged )

    Q_PROPERTY( QskTextOptions textOptions READ textOptions
        WRITE setTextOptions NOTIFY textOptionsChanged )

    Q_PROPERTY( Qt::Alignment alignment READ alignment
        WRITE setAlignment NOTIFY alignmentChanged )

    using Inherited = QskControl;

public:
    QSK_SUBCONTROLS( Panel, Text )

    enum EchoMode
    {
        Normal,
        NoEcho,
        Password,
        PasswordEchoOnEdit
    };
    Q_ENUM(EchoMode)

    enum SelectionMode
    {
        SelectCharacters,
        SelectWords
    };
    Q_ENUM(SelectionMode)

    QskTextInput( QQuickItem* parent = nullptr );
    QskTextInput( const QString& text, QQuickItem* parent = nullptr );

    virtual ~QskTextInput();

    QString text() const;

    void setFontRole( int role );
    int fontRole() const;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    void setAlignment( Qt::Alignment );
    Qt::Alignment alignment() const;

    virtual QSizeF contentsSizeHint() const override;

    QFont font() const;

    bool isReadOnly() const;
    void setReadOnly(bool);

    bool isCursorVisible() const;
    void setCursorVisible( bool );

    int cursorPosition() const;
    void setCursorPosition( int );

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
    void setEchoMode( EchoMode );

    QString displayText() const;
    QString preeditText() const;

    bool overwriteMode() const;
    void setOverwriteMode( bool );

    bool autoScroll() const;
    void setAutoScroll(bool);

    bool selectByMouse() const;
    void setSelectByMouse(bool);

    SelectionMode mouseSelectionMode() const;
    void setMouseSelectionMode( SelectionMode );

    bool persistentSelection() const;
    void setPersistentSelection( bool );

    bool hasAcceptableInput() const;

    virtual QVariant inputMethodQuery( Qt::InputMethodQuery ) const override;
    QVariant inputMethodQuery( Qt::InputMethodQuery, QVariant argument) const;

    bool canUndo() const;
    bool canRedo() const;

    bool isInputMethodComposing() const;

    Qt::InputMethodHints inputMethodHints() const;
    void setInputMethodHints( Qt::InputMethodHints );

public Q_SLOTS:
    void setText( const QString& );

Q_SIGNALS:
    void textChanged( const QString& );
    void textEdited( const QString& );

    void textOptionsChanged();
    void fontRoleChanged();
    void alignmentChanged();

    void readOnlyChanged( bool );

    void accepted();
    void editingFinished();

    void selectedTextChanged( const QString& );

    void overwriteModeChanged( bool );
    void maximumLengthChanged( int );
    void echoModeChanged( EchoMode );
    void autoScrollChanged( bool );
    void selectByMouseChanged( bool );
    void persistentSelectionChanged();

    void validatorChanged();
    void inputMaskChanged( const QString& );

protected:
    virtual bool event( QEvent* ) override;

    virtual void inputMethodEvent( QInputMethodEvent* ) override;

    virtual void focusInEvent( QFocusEvent* ) override;
    virtual void focusOutEvent( QFocusEvent* ) override;

    virtual void mousePressEvent( QMouseEvent* ) override;
    virtual void mouseMoveEvent( QMouseEvent* ) override;
    virtual void mouseReleaseEvent( QMouseEvent* ) override;
    virtual void mouseDoubleClickEvent( QMouseEvent* ) override;

    virtual void keyPressEvent( QKeyEvent* ) override;
    virtual void keyReleaseEvent( QKeyEvent* ) override;

    virtual void updateLayout() override;

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
