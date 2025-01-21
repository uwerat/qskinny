/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_INPUT_H
#define QSK_TEXT_INPUT_H

#include "QskAbstractTextInput.h"
#include "QskTextOptions.h"

class QValidator;

class QSK_EXPORT QskTextInput : public QskAbstractTextInput
{
    Q_OBJECT

    Q_PROPERTY( EchoMode echoMode READ echoMode
        WRITE setEchoMode NOTIFY echoModeChanged )

    Q_PROPERTY( QString passwordCharacter READ passwordCharacter
        WRITE setPasswordCharacter RESET resetPasswordCharacter
        NOTIFY passwordCharacterChanged )

    Q_PROPERTY( int passwordMaskDelay READ passwordMaskDelay
        WRITE setPasswordMaskDelay RESET resetPasswordMaskDelay
        NOTIFY passwordMaskDelayChanged )

    using Inherited = QskAbstractTextInput;

  public:
    QSK_SUBCONTROLS( TextPanel )

    QSK_STATES( Error )

    enum EchoMode : quint8
    {
        Normal,
        NoEcho,
        Password,
        PasswordEchoOnEdit
    };

    Q_ENUM( EchoMode )

    QskTextInput( QQuickItem* parent = nullptr );
    ~QskTextInput() override;

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

    bool hasAcceptableInput() const;
    bool fixup();
    bool acceptInput() override;

    void ensureVisible( int position );

  Q_SIGNALS:
    void maximumLengthChanged( int );

    void echoModeChanged( EchoMode );
    void passwordMaskDelayChanged();
    void passwordCharacterChanged();

    void validatorChanged();
    void inputMaskChanged( const QString& );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
