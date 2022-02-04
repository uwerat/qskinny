/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TEXT_PREDICTOR_H
#define QSK_TEXT_PREDICTOR_H

#include <QskGlobal.h>
#include <qobject.h>

// abstract base class for input methods for retrieving predictive text

class QSK_EXPORT QskTextPredictor : public QObject
{
    Q_OBJECT

  public:
    ~QskTextPredictor() override;

  public Q_SLOTS:
    virtual void request( const QString& text ) = 0;
    virtual void reset() = 0;

  Q_SIGNALS:
    void predictionChanged( const QString& text, const QStringList& candidates );

  protected:
    QskTextPredictor( QObject* );
};

#endif
