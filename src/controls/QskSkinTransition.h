#ifndef QSK_SKIN_TRANSITION_H
#define QSK_SKIN_TRANSITION_H

#include "QskAnimationHint.h"
#include "QskAspect.h"

class QskSkin;
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
    QskSkin* m_skins[ 2 ];
    QskAnimationHint m_animationHint;
    Type m_mask : 2;
};

#endif
