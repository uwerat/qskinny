/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TEXT_INPUT_H
#define QSK_TEXT_INPUT_H

#include "QskControl.h"

class QValidator;

class QSK_EXPORT QskTextInput : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text WRITE setText NOTIFY textChanged )

    Q_PROPERTY( QString description READ description
        WRITE setDescription NOTIFY descriptionChanged )

    Q_PROPERTY( int fontRole READ fontRole
        WRITE setFontRole NOTIFY fontRoleChanged )

    Q_PROPERTY( Qt::Alignment alignment READ alignment
        WRITE setAlignment NOTIFY alignmentChanged )

    Q_PROPERTY( ActivationModes activationModes READ activationModes
        WRITE setActivationModes NOTIFY activationModesChanged )

    Q_PROPERTY( bool editing READ isEditing
        WRITE setEditing NOTIFY editingChanged )

    Q_PROPERTY( EchoMode echoMode READ echoMode
        WRITE setEchoMode NOTIFY echoModeChanged )

    Q_PROPERTY( QString passwordCharacter READ passwordCharacter
        WRITE setPasswordCharacter RESET resetPasswordCharacter
        NOTIFY passwordCharacterChanged )

    Q_PROPERTY( int passwordMaskDelay READ passwordMaskDelay
        WRITE setPasswordMaskDelay RESET resetPasswordMaskDelay
        NOTIFY passwordMaskDelayChanged )

    using Inherited = QskControl;

public:
    QSK_SUBCONTROLS( Panel, Text, PanelSelected, TextSelected )
    QSK_STATES( ReadOnly, Editing )

    enum ActivationMode
    {
        NoActivation,

        ActivationOnFocus = 1 << 0,
        ActivationOnMouse = 1 << 1,
        ActivationOnKey = 1 << 2,

        ActivationOnInput = ActivationOnMouse | ActivationOnKey,
        ActivationOnAll = ActivationOnFocus | ActivationOnMouse | ActivationOnKey
    };

    Q_ENUM( ActivationMode )
    Q_DECLARE_FLAGS( ActivationModes, ActivationMode )

    enum EchoMode
    {
        Normal,
        NoEcho,
        Password,
        PasswordEchoOnEdit
    };

    Q_ENUM(EchoMode)

    QskTextInput( QQuickItem* parent = nullptr );
    QskTextInput( const QString& text, QQuickItem* parent = nullptr );

    virtual ~QskTextInput();

    void setupFrom( const QQuickItem* );

    QString text() const;

    void setDescription( const QString& );
    QString description() const;

    void setFontRole( int role );
    int fontRole() const;

    void setAlignment( Qt::Alignment );
    Qt::Alignment alignment() const;

    void setActivationModes( ActivationModes );
    ActivationModes activationModes() const;

    bool isEditing() const;

    QFont font() const;

    bool isReadOnly() const;
    void setReadOnly(bool);

    int cursorPosition() const;
    void setCursorPosition( int );

    int maxLength() const;
    void setMaxLength( int );

    QValidator* validator() const;
    void setValidator( QValidator* );

    QString inputMask() const;
    void setInputMask( const QString& );

    EchoMode echoMode() const;
    void setEchoMode( EchoMode );

    QString passwordCharacter() const;
    void setPasswordCharacter( const QString& );
    void resetPasswordCharacter();

    int passwordMaskDelay() const;
    void setPasswordMaskDelay( int );
    void resetPasswordMaskDelay();

    QString displayText() const;
    QString preeditText() const;

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    bool overwriteMode() const;
    void setOverwriteMode( bool );
#endif

    virtual bool hasAcceptableInput() const;
    virtual bool fixup();

    virtual QVariant inputMethodQuery( Qt::InputMethodQuery ) const override;
    QVariant inputMethodQuery( Qt::InputMethodQuery, QVariant argument) const;

    bool canUndo() const;
    bool canRedo() const;

    Qt::InputMethodHints inputMethodHints() const;
    void setInputMethodHints( Qt::InputMethodHints );

    void ensureVisible( int position );

    virtual QSizeF contentsSizeHint() const override;

public Q_SLOTS:
    void setText( const QString& );
    void setEditing( bool );

Q_SIGNALS:
    void editingChanged( bool );

    void activationModesChanged();
    void readOnlyChanged( bool );

    void textChanged( const QString& );
    void textEdited( const QString& );

    void descriptionChanged( const QString& );

    void textOptionsChanged();
    void fontRoleChanged();
    void alignmentChanged();

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    void overwriteModeChanged( bool );
#endif

    void maximumLengthChanged( int );

    void echoModeChanged( EchoMode );
    void passwordMaskDelayChanged();
    void passwordCharacterChanged();

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
    virtual void updateNode( QSGNode*) override;

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( QskTextInput::ActivationModes )
Q_DECLARE_METATYPE( QskTextInput::ActivationModes )

#endif
