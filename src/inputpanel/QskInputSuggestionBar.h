/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_SUGGESTION_BAR_H
#define QSK_INPUT_SUGGESTION_BAR_H

#include "QskBox.h"

class QSK_EXPORT QskInputSuggestionBar : public QskBox
{
    Q_OBJECT

    using Inherited = QskBox;

public:
    QSK_SUBCONTROLS( Panel, ButtonPanel, ButtonText )

    QskInputSuggestionBar( QQuickItem* parent = nullptr );
    virtual ~QskInputSuggestionBar();

    virtual QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol subControl ) const override;

    QVector< QString > candidates() const;

Q_SIGNALS:
    void suggested( int );

public Q_SLOTS:
    void setCandidates( const QVector< QString >& );

private:
    void candidateClicked();
    void setCandidateOffset( int );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
