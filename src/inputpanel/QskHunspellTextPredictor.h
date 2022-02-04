/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_HUNSPELL_TEXT_PREDICTOR_H
#define QSK_HUNSPELL_TEXT_PREDICTOR_H

#include "QskTextPredictor.h"

#include <QPair>

#include <memory>

class QSK_EXPORT QskHunspellTextPredictor : public QskTextPredictor
{
    Q_OBJECT

    using Inherited = QskTextPredictor;

  public:
    QskHunspellTextPredictor( const QLocale& locale, QObject* = nullptr );
    ~QskHunspellTextPredictor() override;

  protected:
    void request( const QString& ) override;
    void reset() override;
    virtual QPair< QString, QString > affAndDicFile( const QString&, const QLocale& );

  private:
    Q_INVOKABLE void loadDictionaries();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
