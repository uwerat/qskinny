/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_COMPOSITION_MODEL_H
#define QSK_INPUT_COMPOSITION_MODEL_H

#include <QObject>
#include <memory>

class QskInputContext;

class QskInputCompositionModel : public QObject
{
    Q_OBJECT

public:
    QskInputCompositionModel( QskInputContext* context );
    virtual ~QskInputCompositionModel();

    // to determine whether to show the suggestion bar:
    virtual bool supportsSuggestions() const;

    void commit( const QString& );
    virtual void commitCandidate( int );

    void composeKey( int key );

    virtual int candidateCount() const;
    virtual QString candidate( int ) const;

protected:
    virtual bool hasIntermediate() const;
    virtual QString polishPreedit( const QString& preedit );

Q_SIGNALS:
    void candidatesChanged();

private:
    void clearPreedit();
    QskInputContext* context() const;

    void sendPreeditTextEvent( const QString& );
    void sendKeyEvents( int key );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
