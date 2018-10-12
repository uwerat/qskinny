/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_HUNSPELL_TEXT_PREDICTOR_H
#define QSK_HUNSPELL_TEXT_PREDICTOR_H

#include "QskInputContextGlobal.h"
#include <QskTextPredictor.h>
#include <memory>

class QSK_INPUTCONTEXT_EXPORT QskHunspellTextPredictor : public QskTextPredictor
{
    using Inherited = QskTextPredictor;

  public:
    QskHunspellTextPredictor( QObject* = nullptr );
    ~QskHunspellTextPredictor() override;

    int candidateCount() const override;
    QString candidate( int pos ) const override;

  protected:
    void request( const QString& ) override;
    void reset() override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
