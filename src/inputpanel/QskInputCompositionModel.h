/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_COMPOSITION_MODEL_H
#define QSK_INPUT_COMPOSITION_MODEL_H

#include <QObject>

class QskInputCompositionModel : public QObject
{
    Q_OBJECT

public:
    enum Attribute
    {
        Words = 1 << 0
    };

    Q_ENUM( Attribute )
    Q_DECLARE_FLAGS( Attributes, Attribute )

    virtual ~QskInputCompositionModel();

    virtual void requestCandidates( const QString& preedit ) = 0;
    virtual void resetCandidates() = 0;

    virtual int candidateCount() const = 0;
    virtual QString candidate( int ) const = 0;

    Attributes attributes() const;

Q_SIGNALS:
    void candidatesChanged();

protected:
    QskInputCompositionModel( Attributes, QObject* );

private:
    const Attributes m_attributes;
};

#endif
