/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_PANEL_BOX_H
#define QSK_INPUT_PANEL_BOX_H

#include "QskBox.h"

class QskInputEngine;

class QString;
class QLocale;

class QSK_EXPORT QskInputPanelBox : public QskBox
{
    Q_OBJECT

    using Inherited = QskBox;

    Q_PROPERTY( PanelHints panelHints READ panelHints
        WRITE setPanelHints NOTIFY panelHintsChanged )

    Q_PROPERTY( QString inputPrompt READ inputPrompt
        WRITE setInputPrompt NOTIFY inputPromptChanged )

  public:
    QSK_SUBCONTROLS( Panel, ProxyPanel, ProxyText )

    enum PanelHint
    {
        InputProxy = 1 << 0,
        Prediction = 1 << 1
    };

    Q_ENUM( PanelHint )
    Q_DECLARE_FLAGS( PanelHints, PanelHint )

    QskInputPanelBox( QQuickItem* parent = nullptr );
    ~QskInputPanelBox() override;

    void attachInputItem( QQuickItem* );
    QQuickItem* attachedInputItem() const;

    void setPanelHint( PanelHint, bool on );

    void setPanelHints( PanelHints );
    PanelHints panelHints() const;

    QQuickItem* inputProxy() const;

    QString inputPrompt() const;

    QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol ) const override;

  Q_SIGNALS:
    void panelHintsChanged();
    void inputPromptChanged( const QString& );

    void keySelected( int keyCode );
    void predictiveTextSelected( int );

  public Q_SLOTS:
    void setInputPrompt( const QString& );
    void setPrediction( const QStringList& );

  protected:
    void keyPressEvent( QKeyEvent* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( QskInputPanelBox::PanelHints )
Q_DECLARE_METATYPE( QskInputPanelBox::PanelHints )

#endif
