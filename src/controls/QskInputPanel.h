/******************************************************************************
 * Qsk Controls
 * Copyright (C) 2016 Uwe Rathmann
 *
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef QSK_INPUT_PANEL_H
#define QSK_INPUT_PANEL_H

#include "QskControl.h"

#include <QRectF>

class QSK_EXPORT QskInputPanel : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( QRectF keyboardRect READ keyboardRect NOTIFY keyboardRectChanged )

    Q_PROPERTY( QString displayLanguageName READ displayLanguageName
        NOTIFY displayLanguageNameChanged )

    using Inherited = QskControl;

public:
    QSK_SUBCONTROLS( Panel, KeyFrame, KeyGlyph )
    QSK_STATES( Checked, Pressed )

    struct KeyData
    {
        Qt::Key key = Qt::Key( 0 );
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
    virtual ~QskInputPanel();

    void updateLocale( const QLocale& locale );

    void setMode( QskInputPanel::Mode index );
    Mode mode() const;

    const KeyDataSet& keyData( QskInputPanel::Mode = CurrentMode ) const;

    QString textForKey( Qt::Key ) const;
    QString displayLanguageName() const;

    QRectF keyboardRect() const;

public Q_SLOTS:
    void setPreeditGroups( const QVector< Qt::Key >& );
    void setPreeditCandidates( const QVector< Qt::Key >& );
    bool advanceFocus( bool = true );
    bool activateFocusKey();
    bool deactivateFocusKey();
    void clearFocusKey();

protected:
    virtual void geometryChanged( const QRectF&, const QRectF& ) override;

    virtual void mousePressEvent( QMouseEvent* ) override;
    virtual void mouseMoveEvent( QMouseEvent* ) override;
    virtual void mouseReleaseEvent( QMouseEvent* ) override;
    virtual void touchEvent( QTouchEvent* ) override;
    virtual void timerEvent( QTimerEvent* ) override;

private:
    KeyData& keyDataAt( int );

    void handleKey( int );
    void compose( Qt::Key );
    void selectGroup( int );
    void selectCandidate( int );
    void setCandidateOffset( int );

    void updateKeyData();

Q_SIGNALS:
    void keyboardRectChanged();
    void displayLanguageNameChanged();

public:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_TYPEINFO( QskInputPanel::KeyData, Q_PRIMITIVE_TYPE );

#endif
