/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SETUP_H
#define QSK_SETUP_H

#include "QskGlobal.h"
#include "QskItem.h"

#include <qobject.h>
#include <memory>

class QQuickItem;
class QskGraphicProvider;

#if defined( qskSetup )
#undef qskSetup
#endif

#define qskSetup QskSetup::instance()

class QSK_EXPORT QskSetup : public QObject
{
    Q_OBJECT

  public:

    static QskSetup* instance();

    void setItemUpdateFlags( QskItem::UpdateFlags );
    void resetItemUpdateFlags();
    QskItem::UpdateFlags itemUpdateFlags() const;

    void setItemUpdateFlag( QskItem::UpdateFlag, bool on = true );
    void resetItemUpdateFlag( QskItem::UpdateFlag );
    bool testItemUpdateFlag( QskItem::UpdateFlag );

    void addGraphicProvider( const QString& providerId, QskGraphicProvider* );
    QskGraphicProvider* graphicProvider( const QString& providerId ) const;

    static void setup();
    static void cleanup();

  Q_SIGNALS:
    void itemUpdateFlagsChanged();

  private:
    QskSetup();
    ~QskSetup() override;

    static QskSetup* s_instance;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline QskSetup* QskSetup::instance()
{
    Q_ASSERT( s_instance );
    return s_instance;
}

#endif
