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
    enum Attribute
    {
        Words = 1 << 0
    };

    Q_ENUM( Attribute )
    Q_DECLARE_FLAGS( Attributes, Attribute )

    ~QskTextPredictor() override;

    virtual void request( const QString& text ) = 0;
    virtual void reset() = 0;

    virtual int candidateCount() const = 0;
    virtual QString candidate( int ) const = 0;

    virtual QStringList candidates() const;

    Attributes attributes() const;

  Q_SIGNALS:
    void predictionChanged();

  protected:
    QskTextPredictor( Attributes, QObject* );

  private:
    const Attributes m_attributes;
};

#endif
