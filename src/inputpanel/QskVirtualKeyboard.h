/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_VIRTUAL_KEYBOARD_H
#define QSK_VIRTUAL_KEYBOARD_H

#include "QskBox.h"
#include "QskPushButton.h"

class QSK_EXPORT QskVirtualKeyboardLayouts
{
  public:
    using Layout = QVector< QVector< QVector< int > > >;

    Layout bg; // Bulgarian
    Layout cs; // Czech
    Layout de; // German
    Layout da; // Danish
    Layout el; // Greek
    Layout en_GB; // English (GB)
    Layout en_US; // English (US)
    Layout es; // Spanish
    Layout fi; // Finnish
    Layout fr; // French
    Layout hu; // Hungarian
    Layout it; // Italian
    Layout ja; // Japanese
    Layout lv; // Latvian
    Layout lt; // Lithuanian
    Layout nl; // Dutch
    Layout pt; // Portuguese
    Layout ro; // Romanian
    Layout ru; // Russian
    Layout sl; // Slovene
    Layout sk; // Slovak
    Layout tr; // Turkish
    Layout zh; // Chinese
};

class QSK_EXPORT QskVirtualKeyboard : public QskBox
{
    Q_OBJECT

    Q_PROPERTY( Mode mode READ mode
        WRITE setMode NOTIFY modeChanged FINAL )

    using Inherited = QskBox;

  public:
    QSK_SUBCONTROLS( Panel, ButtonPanel, ButtonText )

    enum Mode
    {
        CurrentMode = -1,
        LowercaseMode,
        UppercaseMode,
        SpecialCharacterMode,
        ModeCount
    };
    Q_ENUM( Mode )

    enum KeyType
    {
        NormalType,
        EnterType,
        BackspaceType,
        CapsSwitchType,
        ModeSwitchType,
        SpecialCharacterType
    };

    QskVirtualKeyboard( QQuickItem* parent = nullptr );
    ~QskVirtualKeyboard() override;

    void setMode( Mode );
    Mode mode() const;

    void updateLocale( const QLocale& );

    bool hasKey( int keyCode ) const;

    int rowCount() const;
    void setRowCount( int );

    int columnCount() const;
    void setColumnCount( int );

    QskVirtualKeyboardLayouts layouts() const;
    void setLayouts( const QskVirtualKeyboardLayouts& );

  Q_SIGNALS:
    void modeChanged( QskVirtualKeyboard::Mode );
    void keyboardLayoutChanged();
    void keySelected( int keyCode );

  protected:
    virtual qreal keyStretch( int ) const;
    virtual bool isKeyVisible( int ) const;
    virtual QString textForKey( int ) const;
    virtual KeyType typeForKey( int ) const;

    void updateLayout() override;
    QSizeF layoutSizeHint( Qt::SizeHint, const QSizeF& ) const override;

  private:
    void ensureButtons();
    void buttonPressed();
    void buttonClicked();

    void updateKeyCodes();
    QskPushButton::Emphasis emphasisForType( KeyType );

    qreal rowStretch( const QVector< int >& );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
