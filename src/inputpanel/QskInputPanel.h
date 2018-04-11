/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_PANEL_H
#define QSK_INPUT_PANEL_H

#include "QskGlobal.h"
#include "QskControl.h"

class QString;
class QLocale;

template class QVector< QString >;

class QSK_EXPORT QskInputPanel: public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

public:
    enum Action
    {
        Compose = 0x10,
        SelectCandidate = 0x11,
    };
    Q_ENUM( Action )

    QskInputPanel( QQuickItem* parent = nullptr );
    virtual ~QskInputPanel() override;

    bool isCandidatesEnabled() const;
    QVector< QString > candidates() const;

public Q_SLOTS:
    void setCandidatesEnabled( bool );
    void setCandidates( const QVector< QString >& );

private:
    void commitKey( Qt::Key );
    void commitCandidate( int );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

QSK_EXPORT QString qskNativeLocaleString( const QLocale& );

#endif
