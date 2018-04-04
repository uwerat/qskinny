/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_HUNSPELL_COMPOSITION_MODEL_H
#define QSK_HUNSPELL_COMPOSITION_MODEL_H

#include "QskInputCompositionModel.h"

class QskHunspellCompositionModel : public QskInputCompositionModel
{
    using Inherited = QskInputCompositionModel;

public:
    QskHunspellCompositionModel( QskInputContext* context );
    virtual ~QskHunspellCompositionModel() override;

    virtual bool supportsSuggestions() const override final;

    virtual void commitCandidate( int index ) override;
    virtual int candidateCount() const override;
    virtual QString candidate( int pos ) const override;

protected:
    virtual bool hasIntermediate() const override;
    virtual QString polishPreedit( const QString& ) override;

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
