/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ABSTRACT_TEXT_INPUT_H
#define QSK_ABSTRACT_TEXT_INPUT_H

#include "QskControl.h"
#include "QskTextOptions.h"

class QskFontRole;

class QSK_EXPORT QskAbstractTextInput : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text
        WRITE setText NOTIFY textChanged USER true )

    Q_PROPERTY( int length READ length NOTIFY textChanged )

    Q_PROPERTY( QString preeditText READ preeditText
        NOTIFY preeditTextChanged )

    Q_PROPERTY( bool editing READ isEditing
        WRITE setEditing NOTIFY editingChanged )

    Q_PROPERTY( bool readOnly READ isReadOnly
        WRITE setReadOnly NOTIFY readOnlyChanged )

    Q_PROPERTY( ActivationModes activationModes READ activationModes
        WRITE setActivationModes NOTIFY activationModesChanged )

    Q_PROPERTY( Qt::InputMethodHints inputMethodHints READ inputMethodHints
        WRITE setInputMethodHints NOTIFY inputMethodHintsChanged )

    Q_PROPERTY( bool inputMethodComposing READ isInputMethodComposing
        NOTIFY inputMethodComposingChanged )

    Q_PROPERTY( QskFontRole fontRole READ fontRole
        WRITE setFontRole RESET resetFontRole NOTIFY fontRoleChanged )

    Q_PROPERTY( QFont font READ font )

    Q_PROPERTY( bool overwriteMode READ overwriteMode
        WRITE setOverwriteMode NOTIFY overwriteModeChanged )

    Q_PROPERTY( bool cursorVisible READ isCursorVisible
        WRITE setCursorVisible NOTIFY cursorVisibleChanged )

    Q_PROPERTY( int cursorPosition READ cursorPosition
        WRITE setCursorPosition NOTIFY cursorPositionChanged )

    Q_PROPERTY( Qt::Alignment alignment READ alignment
        WRITE setAlignment RESET resetAlignment NOTIFY alignmentChanged )

    Q_PROPERTY( QskTextOptions::WrapMode wrapMode READ wrapMode
        WRITE setWrapMode NOTIFY wrapModeChanged )

    Q_PROPERTY( bool persistentSelection READ persistentSelection
        WRITE setPersistentSelection NOTIFY persistentSelectionChanged )

    Q_PROPERTY( bool canUndo READ canUndo NOTIFY canUndoChanged )
    Q_PROPERTY( bool canRedo READ canRedo NOTIFY canRedoChanged )
    Q_PROPERTY( bool canPaste READ canPaste NOTIFY canPasteChanged )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Text )
    QSK_STATES( ReadOnly, Editing, Selected )

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

    ~QskAbstractTextInput() override;

    QString text() const;
    QString preeditText() const;

    int length() const;

    bool isReadOnly() const;
    void setReadOnly( bool );

    bool isEditing() const;
    bool isInputMethodComposing() const;

    void setActivationModes( ActivationModes );
    ActivationModes activationModes() const;

    void setSelectByMouse( bool );
    bool selectByMouse() const;

    void setPersistentSelection( bool );
    bool persistentSelection() const;

    void setAlignment( Qt::Alignment );
    void resetAlignment();
    Qt::Alignment alignment() const;

    void setWrapMode( QskTextOptions::WrapMode );
    QskTextOptions::WrapMode wrapMode() const;

    void setFontRole( const QskFontRole& role );
    void resetFontRole();
    QskFontRole fontRole() const;

    QFont font() const;

    bool overwriteMode() const;
    void setOverwriteMode( bool );

    bool isCursorVisible() const;
    void setCursorVisible( bool );

    int cursorPosition() const;
    void setCursorPosition( int );

    QVariant inputMethodQuery( Qt::InputMethodQuery ) const override;
    QVariant inputMethodQuery( Qt::InputMethodQuery, const QVariant& ) const;

    Qt::InputMethodHints inputMethodHints() const;
    void setInputMethodHints( Qt::InputMethodHints );

    bool canUndo() const;
    bool canRedo() const;
    bool canPaste() const;

    QSizeF unwrappedTextSize() const;

  public Q_SLOTS:
    void setText( const QString& );
    void setEditing( bool );

    void clear();
    void deselect();
    void selectAll();

  Q_SIGNALS:
    void editingChanged( bool );
    void readOnlyChanged( bool );
    void activationModesChanged();
    void inputMethodHintsChanged( Qt::InputMethodHints );
    void fontRoleChanged();
    void overwriteModeChanged( bool );
    void cursorPositionChanged( int );
    void cursorVisibleChanged( bool );
    void selectByMouseChanged( bool );
    void persistentSelectionChanged( bool );

    void wrapModeChanged( QskTextOptions::WrapMode );
    void alignmentChanged();

    void inputMethodComposingChanged( bool );

    void textChanged();
    void textEdited( const QString& );
    void preeditTextChanged();

#if 1
    void canUndoChanged( bool );
    void canRedoChanged( bool );
    void canPasteChanged( bool );
#endif

  protected:
    QskAbstractTextInput( QQuickItem* parent = nullptr );
    void setup( QQuickItem* );

    void forwardEvent( QEvent* );

    bool event( QEvent* ) override;

    void mousePressEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;
    void mouseDoubleClickEvent( QMouseEvent* ) override;

    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

    void focusInEvent( QFocusEvent* ) override;
    void focusOutEvent( QFocusEvent* ) override;

    void inputMethodEvent( QInputMethodEvent* ) override;

    void updateLayout() override;
    void updateNode( QSGNode* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( QskAbstractTextInput::ActivationModes )
Q_DECLARE_METATYPE( QskAbstractTextInput::ActivationModes )

#endif
