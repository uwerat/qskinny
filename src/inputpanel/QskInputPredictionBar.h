/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_INPUT_PREDICTION_BAR_H
#define QSK_INPUT_PREDICTION_BAR_H

#include "QskBox.h"

class QSK_EXPORT QskInputPredictionBar : public QskBox
{
    Q_OBJECT

    using Inherited = QskBox;

  public:
    QSK_SUBCONTROLS( Panel, ButtonPanel, ButtonText )

    QskInputPredictionBar( QQuickItem* parent = nullptr );
    ~QskInputPredictionBar() override;

    QStringList candidates() const;

  Q_SIGNALS:
    void predictiveTextSelected( int );

  public Q_SLOTS:
    void setPrediction( const QStringList& );

  protected:
    QskAspect::Subcontrol substitutedSubcontrol(
        QskAspect::Subcontrol ) const override;

    void buttonClicked();
    void setScrollOffset( int );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
