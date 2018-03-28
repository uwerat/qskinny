/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_PANEL_H
#define QSK_INPUT_PANEL_H

#include "QskControl.h"
#include "QskPushButton.h"

#include <QRectF>

class QskInputCompositionModel;
class QskInputPanel;

class QskKeyButton : public QskPushButton // ### rename to QskInputButton or so?
{
        Q_OBJECT

        using Inherited = QskPushButton;

    public:
        QSK_SUBCONTROLS( Panel, Text, TextCancelButton )
        QskKeyButton( int keyIndex, QskInputPanel* inputPanel, QQuickItem* parent = nullptr );

        virtual QskAspect::Subcontrol effectiveSubcontrol( QskAspect::Subcontrol subControl ) const override;

        int keyIndex() const;

    public Q_SLOTS:
        void updateText();

    private:
        bool isCancelButton() const;

        const int m_keyIndex;
        QskInputPanel* m_inputPanel;
};

class QSK_EXPORT QskInputPanel : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( QRectF keyboardRect READ keyboardRect NOTIFY keyboardRectChanged )

    Q_PROPERTY( QString displayLanguageName READ displayLanguageName
            NOTIFY displayLanguageNameChanged )

    using Inherited = QskControl;

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

    QskInputPanel( QQuickItem* parent = nullptr );
    virtual ~QskInputPanel() override;

    virtual QskAspect::Subcontrol effectiveSubcontrol( QskAspect::Subcontrol subControl ) const override;

    void updateLocale( const QLocale& locale );

    void setMode( QskInputPanel::Mode index );
    Mode mode() const;

    const KeyDataSet& keyData( QskInputPanel::Mode = CurrentMode ) const;

    QString textForKey( int ) const;
    QString displayLanguageName() const;

    QRectF keyboardRect() const;

    // takes ownership:
    void registerCompositionModelForLocale( const QLocale& locale,
                                            QskInputCompositionModel* model );

public Q_SLOTS:
    void setPreeditGroups( const QVector< Qt::Key >& );
    void setPreeditCandidates( const QVector< Qt::Key >& );

    void handleKey( int keyIndex );
    KeyData& keyDataAt( int ) const;
    QString currentTextForKeyIndex( int keyIndex ) const;

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
    void modeChanged( QskInputPanel::Mode mode );
    void cancelPressed();

public:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_TYPEINFO( QskInputPanel::KeyData, Q_PRIMITIVE_TYPE );

#endif
