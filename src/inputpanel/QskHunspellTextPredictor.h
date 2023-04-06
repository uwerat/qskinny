/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_HUNSPELL_TEXT_PREDICTOR_H
#define QSK_HUNSPELL_TEXT_PREDICTOR_H

#include "QskTextPredictor.h"

#include <qpair.h>
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
