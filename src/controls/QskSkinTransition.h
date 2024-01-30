/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SKIN_TRANSITION_H
#define QSK_SKIN_TRANSITION_H

#include "QskAspect.h"
#include <memory>

class QskAnimationHint;
class QskSkin;

class QQuickWindow;
class QVariant;
template< typename Key, typename T > class QHash;

class QSK_EXPORT QskSkinTransition
{
  public:
    enum Type
    {
        Color = 1,
        Metric = 2,

        AllTypes = Color | Metric
    };

    QskSkinTransition();
    virtual ~QskSkinTransition();

    void setSourceSkin( const QskSkin* );
    void setTargetSkin( const QskSkin* );

    void setMask( Type );
    Type mask() const;

    void run( const QskAnimationHint& );

    static bool isRunning();
    static QVariant animatedHint( const QQuickWindow*, QskAspect );
    static QVariant animatedGraphicFilter( const QQuickWindow*, int graphicRole );

  private:
    Q_DISABLE_COPY( QskSkinTransition )

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
