/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PINYIN_COMPOSITION_MODEL_H
#define QSK_PINYIN_COMPOSITION_MODEL_H

#include "QskInputCompositionModel.h"
#include <memory>

class QskPinyinCompositionModel : public QskInputCompositionModel
{
    using Inherited = QskInputCompositionModel;

public:
    QskPinyinCompositionModel( QObject* );
    virtual ~QskPinyinCompositionModel() override;

    virtual int candidateCount() const override;
    virtual QString candidate( int ) const override;

protected:
    virtual void requestCandidates( const QString& ) override;
    virtual void resetCandidates() override;

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
