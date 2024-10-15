/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_INPUT_H
#define QSK_TEXT_INPUT_H

#include "QskControl.h"
#include "QskGraphic.h"

class QValidator;
class QskFontRole;

class QSK_EXPORT QskTextInput : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( QString inputText READ inputText
        WRITE setInputText NOTIFY inputTextChanged USER true )

    Q_PROPERTY( QString labelText READ labelText
        WRITE setLabelText NOTIFY labelTextChanged )

    Q_PROPERTY( QString hintText READ hintText
        WRITE setHintText NOTIFY hintTextChanged )

    Q_PROPERTY( QString supportingText READ supportingText
        WRITE setSupportingText NOTIFY supportingTextChanged )

    Q_PROPERTY( QskFontRole fontRole READ fontRole
        WRITE setFontRole RESET resetFontRole NOTIFY fontRoleChanged )

    Q_PROPERTY( QFont font READ font CONSTANT )

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

    Q_PROPERTY( bool panel READ hasPanel
        WRITE setPanel NOTIFY panelChanged )

    Q_PROPERTY( Emphasis emphasis READ emphasis
        WRITE setEmphasis NOTIFY emphasisChanged )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Panel, LeadingIcon, LabelText, InputText,
        TrailingIconRipple, TrailingIcon, HintText, SupportingText,
        CharacterCount )

    QSK_STATES( ReadOnly, Editing, Selected, Error, TextPopulated )

    enum Emphasis
    {
        LowEmphasis      = -1,
        NoEmphasis       = 0,
    };
    Q_ENUM( Emphasis )

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

    QskTextInput( QQuickItem* parent = nullptr );
    QskTextInput( const QString&, QQuickItem* parent = nullptr );

    ~QskTextInput() override;

    void setupFrom( const QQuickItem* );

    void setEmphasis( Emphasis );
    Emphasis emphasis() const;

    QString inputText() const;

    QString labelText() const;

    QskGraphic leadingIcon() const;
    void setLeadingIcon( const QskGraphic& );

    void setHintText( const QString& );
    QString hintText() const;

    void setSupportingText( const QString& );
    QString supportingText() const;

    void setPanel( bool );
    bool hasPanel() const;

    void setFontRole( const QskFontRole& role );
    void resetFontRole();
    QskFontRole fontRole() const;

    void setAlignment( Qt::Alignment );
    void resetAlignment();
    Qt::Alignment alignment() const;

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

    QskAspect::Variation effectiveVariation() const override;

  public Q_SLOTS:
    void setInputText( const QString& );
    void setLabelText( const QString& );

    void setEditing( bool );

  Q_SIGNALS:
    void emphasisChanged( Emphasis );

    void editingChanged( bool );

    void activationModesChanged();
    void readOnlyChanged( bool );
    void panelChanged( bool );

    void inputTextChanged( const QString& );
    void labelTextChanged( const QString& );

    void displayTextChanged( const QString& );

    void textEdited( const QString& );
    void hintTextChanged( const QString& );
    void supportingTextChanged( const QString& );

    void textOptionsChanged();
    void fontRoleChanged();
    void alignmentChanged();

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

    void hoverEnterEvent( QHoverEvent* ) override;
    void hoverMoveEvent( QHoverEvent* ) override;
    void hoverLeaveEvent( QHoverEvent* ) override;

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

Q_DECLARE_OPERATORS_FOR_FLAGS( QskTextInput::ActivationModes )
Q_DECLARE_METATYPE( QskTextInput::ActivationModes )

#endif
