/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_COMPOSITION_MODEL_H
#define QSK_INPUT_COMPOSITION_MODEL_H

#include <QObject>

class QskInputContext;

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

    void composeKey( const QString& text, int spaceLeft );

    virtual int candidateCount() const = 0;
    virtual QString candidate( int ) const = 0;

    void reset();

    QString preeditText() const;
    void setPreeditText( const QString& );

    Attributes attributes() const;

protected:
    QskInputCompositionModel( Attributes, QskInputContext* );

    virtual void requestCandidates( const QString& preedit ) = 0;
    virtual void resetCandidates() = 0;

    QskInputContext* context() const;

Q_SIGNALS:
    void candidatesChanged();

private:
    QString m_preedit;
    const Attributes m_attributes;
};

inline QString QskInputCompositionModel::preeditText() const
{
    return m_preedit;
}

inline QskInputCompositionModel::Attributes
QskInputCompositionModel::attributes() const
{
    return m_attributes;
}

#endif
