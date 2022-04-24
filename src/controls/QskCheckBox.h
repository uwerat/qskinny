/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_CHECK_BOX_H
#define QSK_CHECK_BOX_H

#include "QskAbstractButton.h"

class QskTextOptions;

class QSK_EXPORT QskCheckBox : public QskAbstractButton
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text WRITE setText NOTIFY textChanged )
    Q_PROPERTY( QskTextOptions textOptions READ textOptions
        WRITE setTextOptions NOTIFY textOptionsChanged )


    using Inherited = QskAbstractButton;

  public:
    QSK_SUBCONTROLS( Panel, IndicatorBox, IndicatorTic, Text)

    QskCheckBox( QQuickItem* parent = nullptr );
    ~QskCheckBox() override;

    bool isCheckable() const override final;

    QString text() const;
    QskTextOptions textOptions() const;

  Q_SIGNALS:
    void textChanged( const QString& );
    void textOptionsChanged( const QskTextOptions& );

  public Q_SLOTS:
    void setText( const QString& );
    void setTextOptions( const QskTextOptions& );


  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
