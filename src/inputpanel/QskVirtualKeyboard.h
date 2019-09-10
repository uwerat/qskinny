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
    ~QskVirtualKeyboard() override;

    void setMode( Mode );
    Mode mode() const;

    void updateLocale( const QLocale& );

    QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol ) const override;

    bool hasKey( int keyCode ) const;

  Q_SIGNALS:
    void modeChanged( Mode );
    void keySelected( int keyCode );

  protected:
    void updateLayout() override;
    QSizeF layoutSizeHint( Qt::SizeHint, const QSizeF& ) const override;

  private:
    void buttonPressed();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
