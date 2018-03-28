/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_VIRTUAL_KEYBOARD_H
#define QSK_VIRTUAL_KEYBOARD_H

#include "QskBox.h"
#include "QskPushButton.h"

#include <QRectF>

class QskInputCompositionModel;
class QskVirtualKeyboard;

class QSK_EXPORT QskVirtualKeyboardButton : public QskPushButton
{
        Q_OBJECT

        using Inherited = QskPushButton;

    public:
        QSK_SUBCONTROLS( Panel, Text, TextCancelButton )
        QskVirtualKeyboardButton( int keyIndex, QskVirtualKeyboard* inputPanel, QQuickItem* parent = nullptr );

        virtual QskAspect::Subcontrol effectiveSubcontrol( QskAspect::Subcontrol subControl ) const override;

        int keyIndex() const;

    public Q_SLOTS:
        void updateText();

    private:
        bool isCancelButton() const;

        const int m_keyIndex;
        QskVirtualKeyboard* m_inputPanel;
};

class QSK_EXPORT QskVirtualKeyboard : public QskBox
{
    Q_OBJECT

    Q_PROPERTY( QString displayLanguageName READ displayLanguageName
            NOTIFY displayLanguageNameChanged )

    using Inherited = QskBox;

public:

    QSK_SUBCONTROLS( Panel )

    struct KeyData
    {
        int key = 0;
        bool isSuggestionKey = false;
        QRectF rect;
    };

    enum Action
    {
        Compose = 0x10,
        SelectGroup = 0x11,
        SelectCandidate = 0x12,
    };
    Q_ENUM( Action )

    enum Mode
    {
        CurrentMode = -1,
        LowercaseMode,
        UppercaseMode,
        SpecialCharacterMode,
        ModeCount
    };
    Q_ENUM( Mode )

    enum
    {
        RowCount = 5,
        KeyCount = 12
    };

    using KeyRow = Qt::Key[KeyCount];
    using KeyDataRow = KeyData[KeyCount];
    using KeyDataSet = KeyDataRow[RowCount];

    QskVirtualKeyboard( QQuickItem* parent = nullptr );
    virtual ~QskVirtualKeyboard() override;

    virtual QskAspect::Subcontrol effectiveSubcontrol( QskAspect::Subcontrol subControl ) const override;

    void updateLocale( const QLocale& locale );

    void setMode( QskVirtualKeyboard::Mode index );
    Mode mode() const;

    const KeyDataSet& keyData( QskVirtualKeyboard::Mode = CurrentMode ) const;

    QString textForKey( int ) const;
    QString displayLanguageName() const;

    QRectF keyboardRect() const;

    // takes ownership:
    void registerCompositionModelForLocale( const QLocale& locale,
                                            QskInputCompositionModel* model );

    void handleKey( int keyIndex );
    KeyData& keyDataAt( int ) const;
    QString currentTextForKeyIndex( int keyIndex ) const;

public Q_SLOTS:
    void setPreeditGroups( const QVector< Qt::Key >& );
    void setPreeditCandidates( const QVector<Qt::Key> & );

protected:
    virtual void geometryChanged( const QRectF&, const QRectF& ) override;
    virtual void updateLayout() override;

private:
    void createUI();
    void updateUI(); // e.g. called when updating Pinyin suggestions

    void compose( int );
    void selectGroup( int );
    void selectCandidate( int );
    void setCandidateOffset( int );

    void updateKeyData();

Q_SIGNALS:
    void keyboardRectChanged();
    void displayLanguageNameChanged();
    void inputMethodRegistered( const QLocale& locale, QskInputCompositionModel* model );
    void inputMethodEventReceived( QInputMethodEvent* inputMethodEvent );
    void keyEventReceived( QKeyEvent* keyEvent );
    void modeChanged( QskVirtualKeyboard::Mode mode );
    void cancelPressed();

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_TYPEINFO( QskVirtualKeyboard::KeyData, Q_PRIMITIVE_TYPE );

#endif
