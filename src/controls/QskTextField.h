/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_FIELD_H
#define QSK_TEXT_FIELD_H

#include "QskControl.h"
#include "QskTextOptions.h"

class QValidator;
class QskFontRole;

class QSK_EXPORT QskTextField : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text WRITE setText NOTIFY textChanged USER true)

    Q_PROPERTY( QString placeholderText READ placeholderText
        WRITE setPlaceholderText NOTIFY placeholderTextChanged )

    Q_PROPERTY( QskFontRole fontRole READ fontRole
        WRITE setFontRole RESET resetFontRole NOTIFY fontRoleChanged )

    Q_PROPERTY( QFont font READ font )

    Q_PROPERTY( Qt::Alignment alignment READ alignment
        WRITE setAlignment RESET resetAlignment NOTIFY alignmentChanged )

    Q_PROPERTY( QskTextOptions::WrapMode wrapMode READ wrapMode
        WRITE setWrapMode NOTIFY wrapModeChanged )

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

    Q_PROPERTY( bool panel READ hasPanel
        WRITE setPanel NOTIFY panelChanged )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Panel, Text, PlaceholderText, PanelSelected, TextSelected )
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

    Q_ENUM( EchoMode )

    QskTextField( QQuickItem* parent = nullptr );
    QskTextField( const QString& text, QQuickItem* parent = nullptr );

    ~QskTextField() override;

    void setupFrom( const QQuickItem* );

    QString text() const;

    void setPlaceholderText( const QString& );
    QString placeholderText() const;

    void setPanel( bool );
    bool hasPanel() const;

    void setFontRole( const QskFontRole& role );
    void resetFontRole();
    QskFontRole fontRole() const;

    void setAlignment( Qt::Alignment );
    void resetAlignment();
    Qt::Alignment alignment() const;

    void setWrapMode( QskTextOptions::WrapMode );
    QskTextOptions::WrapMode wrapMode() const;

    void setActivationModes( ActivationModes );
    ActivationModes activationModes() const;

    bool isEditing() const;

    QFont font() const;

    bool isReadOnly() const;
    void setReadOnly( bool );

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

    bool overwriteMode() const;
    void setOverwriteMode( bool );

    bool hasAcceptableInput() const;
    bool fixup();

    QVariant inputMethodQuery( Qt::InputMethodQuery ) const override;
    QVariant inputMethodQuery( Qt::InputMethodQuery, const QVariant& argument ) const;

    bool canUndo() const;
    bool canRedo() const;

    Qt::InputMethodHints inputMethodHints() const;
    void setInputMethodHints( Qt::InputMethodHints );

    void ensureVisible( int position );

  public Q_SLOTS:
    void setText( const QString& );
    void setEditing( bool );

  Q_SIGNALS:
    void editingChanged( bool );

    void activationModesChanged();
    void readOnlyChanged( bool );
    void panelChanged( bool );

    void textChanged( const QString& );
    void displayTextChanged( const QString& );

    void textEdited( const QString& );
    void placeholderTextChanged( const QString& );

    void fontRoleChanged();
    void alignmentChanged();
    void wrapModeChanged( QskTextOptions::WrapMode );

    void overwriteModeChanged( bool );
    void maximumLengthChanged( int );

    void echoModeChanged( EchoMode );
    void passwordMaskDelayChanged();
    void passwordCharacterChanged();

    void validatorChanged();
    void inputMaskChanged( const QString& );

  protected:
    bool event( QEvent* ) override;

    void inputMethodEvent( QInputMethodEvent* ) override;

    void focusInEvent( QFocusEvent* ) override;
    void focusOutEvent( QFocusEvent* ) override;

    void mousePressEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;
    void mouseDoubleClickEvent( QMouseEvent* ) override;

    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

    QSizeF layoutSizeHint( Qt::SizeHint, const QSizeF& ) const override;

    void updateLayout() override;
    void updateNode( QSGNode* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( QskTextField::ActivationModes )
Q_DECLARE_METATYPE( QskTextField::ActivationModes )

#endif
