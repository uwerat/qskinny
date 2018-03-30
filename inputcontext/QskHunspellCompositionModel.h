#ifndef QSKHUNSPELLCOMPOSITIONMODEL_H
#define QSKHUNSPELLCOMPOSITIONMODEL_H

#include "QskInputCompositionModel.h"

#include <QVector>

class Hunhandle;

class QskHunspellCompositionModel : public QskInputCompositionModel
{
public:
    QskHunspellCompositionModel();
    ~QskHunspellCompositionModel() override;

    virtual bool supportsSuggestions() const override final;

    virtual void commitCandidate( int index ) override;
    int candidateCount() const override;
    QString candidate( int pos ) const override;

protected:
    bool hasIntermediate() const override;
    virtual QString polishPreedit( const QString& preedit ) override;
    bool isComposable( const QStringRef& preedit ) const override;

private:
    Hunhandle* m_hunspellHandle;
    QVector< QString > m_candidates;
};

#endif // QSKHUNSPELLCOMPOSITIONMODEL_H
