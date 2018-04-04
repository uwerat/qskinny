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

class QSK_EXPORT QskVirtualKeyboardCandidateButton : public QskPushButton
{
    Q_OBJECT

    using Inherited = QskPushButton;

public:
    QSK_SUBCONTROLS( Panel, Text )

    QskVirtualKeyboardCandidateButton( QskVirtualKeyboard* inputPanel, QQuickItem* parent = nullptr );
    void setIndexAndText( int index, const QString& text );

    virtual QskAspect::Subcontrol effectiveSubcontrol( QskAspect::Subcontrol subControl ) const override;

    static int maxCandidates();
    // add a setter here as well if needed

private:
    QskVirtualKeyboard* m_inputPanel;
    int m_index;
    QString m_text;
};

class QSK_EXPORT QskVirtualKeyboardButton : public QskPushButton
{
    Q_OBJECT

    using Inherited = QskPushButton;

public:
    QSK_SUBCONTROLS( Panel, Text )

    QskVirtualKeyboardButton( int keyIndex,
        QskVirtualKeyboard*, QQuickItem* parent = nullptr );

    virtual QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol ) const override;

    int keyIndex() const;

public Q_SLOTS:
    void updateText();

private:
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
        SelectCandidate = 0x11,
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

    virtual QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol ) const override;

    void updateLocale( const QLocale& );

    void setMode( QskVirtualKeyboard::Mode );
    Mode mode() const;

    const KeyDataSet& keyData( QskVirtualKeyboard::Mode = CurrentMode ) const;

    QString textForKey( int ) const;
    QString displayLanguageName() const;

    void handleKey( int keyIndex );
    KeyData& keyDataAt( int ) const;
    QString currentTextForKeyIndex( int keyIndex ) const;

    void handleCandidateKey( int index, const QString& text );
    void setCandidateBarVisible( bool visible );

public Q_SLOTS:
    void setPreeditCandidates( const QVector< QString >& );

protected:
    virtual bool eventFilter( QObject*, QEvent* ) override;
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
    void modeChanged( QskVirtualKeyboard::Mode );

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_TYPEINFO( QskVirtualKeyboard::KeyData, Q_PRIMITIVE_TYPE );

#endif
