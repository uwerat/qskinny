#ifndef QSK_SKIN_TRANSITION_H
#define QSK_SKIN_TRANSITION_H

#include "QskAspect.h"
#include <memory>

class QskSkin;
class QskAnimationHint;
class QQuickWindow;
class QVariant;

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

    void setSourceSkin( QskSkin* );
    QskSkin* sourceSkin() const;

    void setTargetSkin( QskSkin* );
    QskSkin* targetSkin() const;

    void setAnimation( QskAnimationHint );
    QskAnimationHint animation() const;

    void setMask( Type );
    Type mask() const;

    void process();

    static bool isRunning();
    static QVariant animatedHint( const QQuickWindow*, QskAspect );
    static QVariant animatedGraphicFilter( const QQuickWindow*, int graphicRole );

  protected:
    virtual void updateSkin( QskSkin*, QskSkin* );

  private:
    Q_DISABLE_COPY( QskSkinTransition )

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
