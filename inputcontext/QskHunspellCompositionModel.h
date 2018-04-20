/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_HUNSPELL_COMPOSITION_MODEL_H
#define QSK_HUNSPELL_COMPOSITION_MODEL_H

#include "QskInputCompositionModel.h"
#include <memory>

class QskHunspellCompositionModel : public QskInputCompositionModel
{
    using Inherited = QskInputCompositionModel;

public:
    QskHunspellCompositionModel( QskInputContext* context );
    virtual ~QskHunspellCompositionModel() override;

    virtual int candidateCount() const override;
    virtual QString candidate( int pos ) const override;

protected:
    virtual void requestCandidates( const QString& ) override;
    virtual void resetCandidates() override;

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
