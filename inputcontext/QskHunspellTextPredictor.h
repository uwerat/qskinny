/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_HUNSPELL_TEXT_PREDICTOR_H
#define QSK_HUNSPELL_TEXT_PREDICTOR_H

#include "QskTextPredictor.h"
#include <memory>

class QSK_EXPORT QskHunspellTextPredictor : public QskTextPredictor
{
    using Inherited = QskTextPredictor;

public:
    QskHunspellTextPredictor( QObject* = nullptr );
    virtual ~QskHunspellTextPredictor() override;

    virtual int candidateCount() const override;
    virtual QString candidate( int pos ) const override;

protected:
    virtual void request( const QString& ) override;
    virtual void reset() override;

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
