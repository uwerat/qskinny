/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_PINYIN_TEXT_PREDICTOR_H
#define QSK_PINYIN_TEXT_PREDICTOR_H

#include "QskInputContextGlobal.h"
#include <QskTextPredictor.h>
#include <memory>

class QSK_INPUTCONTEXT_EXPORT QskPinyinTextPredictor : public QskTextPredictor
{
    Q_OBJECT

    using Inherited = QskTextPredictor;

  public:
    QskPinyinTextPredictor( QObject* = nullptr );
    ~QskPinyinTextPredictor() override;

  protected:
    void request( const QString& ) override;
    void reset() override;

  private:
    Q_INVOKABLE void loadDictionary();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
