/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_EDIT_H
#define QSK_TEXT_EDIT_H

#include "QskControl.h"
#include "QskTextOptions.h"

class QValidator;
class QskFontRole;

class QSK_EXPORT QskTextEdit : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text
        WRITE setText NOTIFY textChanged USER true )

    Q_PROPERTY( QskFontRole fontRole READ fontRole
        WRITE setFontRole RESET resetFontRole NOTIFY fontRoleChanged )

    Q_PROPERTY( QFont font READ font )

    Q_PROPERTY( Qt::Alignment alignment READ alignment
        WRITE setAlignment RESET resetAlignment NOTIFY alignmentChanged )

    Q_PROPERTY( QskTextOptions::WrapMode wrapMode READ wrapMode
        WRITE setWrapMode NOTIFY wrapModeChanged )

    Q_PROPERTY( int lineCount READ lineCount NOTIFY lineCountChanged )

    Q_PROPERTY( QskTextOptions::TextFormat textFormat READ textFormat
        WRITE setTextFormat NOTIFY textFormatChanged )

    Q_PROPERTY( ActivationModes activationModes READ activationModes
        WRITE setActivationModes NOTIFY activationModesChanged )

    Q_PROPERTY( bool selectByMouse READ selectByMouse
        WRITE setSelectByMouse NOTIFY selectByMouseChanged )

    Q_PROPERTY( bool editing READ isEditing
        WRITE setEditing NOTIFY editingChanged )

    Q_PROPERTY( qreal tabStopDistance READ tabStopDistance
        WRITE setTabStopDistance NOTIFY tabStopDistanceChanged )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( TextPanel, Text )
    QSK_STATES( ReadOnly, Editing, Error )

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

    QskTextEdit( QQuickItem* parent = nullptr );

    ~QskTextEdit() override;

    void setupFrom( const QQuickItem* );

    QString text() const;

    void setFontRole( const QskFontRole& role );
    void resetFontRole();
    QskFontRole fontRole() const;

    void setAlignment( Qt::Alignment );
    void resetAlignment();
    Qt::Alignment alignment() const;

    void setWrapMode( QskTextOptions::WrapMode );
    QskTextOptions::WrapMode wrapMode() const;

    void setTextFormat( QskTextOptions::TextFormat );
    QskTextOptions::TextFormat textFormat() const;

    int lineCount() const;

    void setActivationModes( ActivationModes );
    ActivationModes activationModes() const;

    void setSelectByMouse( bool );
    bool selectByMouse() const;

    bool isEditing() const;

    QFont font() const;

    bool isReadOnly() const;
    void setReadOnly( bool );

    int cursorPosition() const;
    void setCursorPosition( int );

    QString preeditText() const;

    bool overwriteMode() const;
    void setOverwriteMode( bool );

    QVariant inputMethodQuery( Qt::InputMethodQuery ) const override;
    QVariant inputMethodQuery( Qt::InputMethodQuery, const QVariant& argument ) const;

    bool canUndo() const;
    bool canRedo() const;

    Qt::InputMethodHints inputMethodHints() const;
    void setInputMethodHints( Qt::InputMethodHints );

    int tabStopDistance() const;
    void setTabStopDistance( qreal );

  public Q_SLOTS:
    void clear();
    void selectAll();
    void setText( const QString& );
    void setEditing( bool );

  Q_SIGNALS:
    void textChanged( const QString& );
    void preeditTextChanged( const QString& );

    void editingChanged( bool );

    void activationModesChanged();
    void readOnlyChanged( bool );
    void panelChanged( bool );

    void displayTextChanged( const QString& );

    void textEdited( const QString& );
    void placeholderTextChanged( const QString& );

    void fontRoleChanged();
    void alignmentChanged();
    void wrapModeChanged( QskTextOptions::WrapMode );

    void lineCountChanged( int );

    void selectByMouseChanged( bool );

    void textFormatChanged( QskTextOptions::TextFormat );

    void overwriteModeChanged( bool );

    void tabStopDistanceChanged( qreal );

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

    void updateLayout() override;
    void updateNode( QSGNode* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( QskTextEdit::ActivationModes )
Q_DECLARE_METATYPE( QskTextEdit::ActivationModes )

#endif
