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

    Q_PROPERTY( QString displayText READ displayText NOTIFY displayTextChanged )

    Q_PROPERTY( int maximumLength READ maxLength
        WRITE setMaxLength NOTIFY maximumLengthChanged )

    Q_PROPERTY( QString inputMask READ inputMask
        WRITE setInputMask NOTIFY inputMaskChanged )

    Q_PROPERTY( bool acceptableInput READ hasAcceptableInput
        NOTIFY acceptableInputChanged)

    Q_PROPERTY( bool autoScroll READ autoScroll
        WRITE setAutoScroll NOTIFY autoScrollChanged )

    Q_PROPERTY( EchoMode echoMode READ echoMode
        WRITE setEchoMode NOTIFY echoModeChanged )

    Q_PROPERTY( QString passwordCharacter READ passwordCharacter
        WRITE setPasswordCharacter RESET resetPasswordCharacter
        NOTIFY passwordCharacterChanged )

    Q_PROPERTY( int passwordMaskDelay READ passwordMaskDelay
        WRITE setPasswordMaskDelay RESET resetPasswordMaskDelay
        NOTIFY passwordMaskDelayChanged )

    Q_PROPERTY( QValidator* validator READ validator
        WRITE setValidator NOTIFY validatorChanged )

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

    bool autoScroll() const;
    void setAutoScroll( bool );

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

    void autoScrollChanged( bool );

    void echoModeChanged( EchoMode );
    void passwordMaskDelayChanged();
    void passwordCharacterChanged();

    void validatorChanged( const QValidator* );
    void inputMaskChanged( const QString& );
    void acceptableInputChanged( bool );

    void displayTextChanged( const QString& );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
