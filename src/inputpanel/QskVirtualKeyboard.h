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

    virtual QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol ) const override;

    void updateLocale( const QLocale& );

    void setMode( Mode );
    Mode mode() const;

Q_SIGNALS:
    void modeChanged( Mode );
    void keySelected( Qt::Key );

protected:
    virtual bool event( QEvent* ) override;
    virtual void updateLayout() override;

private Q_SLOTS:
    void buttonPressed();

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
