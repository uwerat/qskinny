/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PINYIN_COMPOSITION_MODEL_H
#define QSK_PINYIN_COMPOSITION_MODEL_H

#include "QskInputCompositionModel.h"

class QskPinyinCompositionModel : public QskInputCompositionModel
{
    using Inherited = QskInputCompositionModel;

public:
    QskPinyinCompositionModel( QskInputContext* );
    virtual ~QskPinyinCompositionModel() override;

    virtual bool supportsSuggestions() const override final;

    virtual int candidateCount() const override;
    virtual QString candidate( int ) const override;

protected:
    // Used for text composition
    virtual bool hasIntermediate() const override;
    virtual QString polishPreedit( const QString& preedit ) override;

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
