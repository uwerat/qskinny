/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_ENGINE_H
#define QSK_INPUT_ENGINE_H

#include "QskGlobal.h"
#include <QObject>
#include <memory>

class QskTextPredictor;
class QQuickItem;
class QStringList;

class QSK_EXPORT QskInputEngine : public QObject
{
    Q_OBJECT

    using Inherited = QObject;

public:
    class Result
    {
    public:
        int key = 0;

        QString text;
        bool isFinal = true;
    };

    QskInputEngine( QObject* parent = nullptr );
    virtual ~QskInputEngine();

    void setPredictor( QskTextPredictor* );
    QskTextPredictor* predictor() const;

    virtual Result processKey( int key,
        Qt::InputMethodHints, int spaceLeft = -1 );

    QString predictiveText( int ) const;
    QStringList prediction() const;

    void reset();

Q_SIGNALS:
    void predictionChanged();

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
