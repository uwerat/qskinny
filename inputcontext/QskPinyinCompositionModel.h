/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PINYIN_COMPOSITION_MODEL_H
#define QSK_PINYIN_COMPOSITION_MODEL_H

#include "QskInputCompositionModel.h"

class QskPinyinCompositionModel : public QskInputCompositionModel
{
    Q_OBJECT

public:
    QskPinyinCompositionModel();
    virtual ~QskPinyinCompositionModel() override;

    int candidateCount() const override;
    Qt::Key candidate( int ) const override;

    QVector< Qt::Key > groups() const override;

protected:
    // Used for text composition
    bool hasIntermediate() const override;
    QString polishPreedit( const QString& preedit ) override;
    bool isComposable( const QStringRef& preedit ) const override;

private:
    void handleGroupIndexChanged();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif // QSK_PINYIN_COMPOSITION_MODEL_H
