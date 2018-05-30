/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_VIRTUAL_KEYBOARD_H
#define QSK_VIRTUAL_KEYBOARD_H

#include "QskBox.h"

class QSK_EXPORT QskVirtualKeyboard : public QskBox
{
    Q_OBJECT

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

    QskVirtualKeyboard( QQuickItem* parent = nullptr );
    virtual ~QskVirtualKeyboard() override;

    void setMode( Mode );
    Mode mode() const;

    void updateLocale( const QLocale& );

    virtual qreal heightForWidth( qreal width ) const override;
    virtual qreal widthForHeight( qreal height ) const override;

    virtual QSizeF contentsSizeHint() const override;

    virtual QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol ) const override;

Q_SIGNALS:
    void modeChanged( Mode );
    void keySelected( int keyCode );

protected:
    virtual void updateLayout() override;

private:
    void buttonPressed();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
